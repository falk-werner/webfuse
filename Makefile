.PHONY: default
default: all

# Overridable defaults

export SOURCE_DATE_EPOCH ?= $(shell $(PROJECT_ROOT)/build/get_source_date_epoch.sh)
export BUILDTIME ?= $(shell date -u -d '@$(SOURCE_DATE_EPOCH)' --rfc-3339 ns 2>/dev/null | sed -e 's/ /T/')

VERBOSE ?=
MARCH ?= 

PROJECT_NAME ?= webfs
PROJECT_ROOT ?= .
VERSION ?= $(shell cat $(PROJECT_ROOT)/VERSION)
OUT ?= $(PROJECT_ROOT)/.build

PARALLELMFLAGS ?= -j$(shell nproc)
UID ?= $(shell id -u)

DOCKER ?= docker
DOCKER_BUILDKIT ?= 

CONTAINER_USER ?= $(UID)
CONTAINER_GROUP ?= $(shell id -g)
CONTAINER_WORKSPACE ?= /workspace

UBUNTU_CODENAME ?= bionic

# Dependencies

GTEST_VERSION ?= 1.8.1
DOCKER_BUILDARGS += GTEST_VERSION=$(GTEST_VERSION)
FETCH_TARGETS += $(OUT)/googletest-release-$(GTEST_VERSION).tar.gz
$(OUT)/googletest-release-$(GTEST_VERSION).tar.gz: URL := https://github.com/google/googletest/archive/release-$(GTEST_VERSION).tar.gz

FUSE_VERSION ?= 3.1.1
DOCKER_BUILDARGS += FUSE_VERSION=$(FUSE_VERSION)
FETCH_TARGETS += $(OUT)/libfuse-fuse-$(FUSE_VERSION).tar.gz
$(OUT)/libfuse-fuse-$(FUSE_VERSION).tar.gz: URL := https://github.com/libfuse/libfuse/archive/fuse-$(FUSE_VERSION).tar.gz

WEBSOCKETS_VERSION ?= 3.1.0
DOCKER_BUILDARGS += WEBSOCKETS_VERSION=$(WEBSOCKETS_VERSION)
FETCH_TARGETS += $(OUT)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz
$(OUT)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz: URL := https://github.com/warmcat/libwebsockets/archive/v$(WEBSOCKETS_VERSION).tar.gz

JANSSON_VERSION ?= 2.12
DOCKER_BUILDARGS += JANSSON_VERSION=$(JANSSON_VERSION)
FETCH_TARGETS += $(OUT)/jansson-$(JANSSON_VERSION).tar.gz
$(OUT)/jansson-$(JANSSON_VERSION).tar.gz: URL := https://github.com/akheron/jansson/archive/v$(JANSSON_VERSION).tar.gz

QEMU_VERSION ?= v3.1.0-2
FETCH_TARGETS += $(OUT)/docker/qemu-arm-static-$(QEMU_VERSION)
$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION): URL := https://github.com/multiarch/qemu-user-static/releases/download/$(QEMU_VERSION)/qemu-arm-static

# Architecture-specific rule target configuration

MARCH_AMD64 := $(filter-out amd64,$(MARCH))
$(MARCH_AMD64)MARCHS += amd64
$(MARCH_AMD64)TARGETS += amd64-ubuntu-builder
$(OUT)/amd64-ubuntu-builder/rules.mk: TARGET := amd64-ubuntu-builder

MARCH_ARM32V7 := $(filter-out arm32v7,$(MARCH))
$(MARCH_ARM32V7)MARCHS += arm32v7
$(MARCH_ARM32V7)TARGETS += arm32v7-ubuntu-builder
$(OUT)/arm32v7-ubuntu-builder/rules.mk: TARGET := arm32v7-ubuntu-builder

$(MARCH_AMD64)MEMCHECK_TARGETS += $(addprefix memcheck-,$(TARGETS))

ARM_TARGETS = $(filter arm%,$(TARGETS))
$(addprefix $(OUT)/docker/,$(ARM_TARGETS)): $(OUT)/docker/qemu-arm-static-$(QEMU_VERSION)

UBUNTU_TARGETS = $(filter $(addsuffix -ubuntu%,$(MARCHS)),$(TARGETS))
$(addprefix $(OUT)/docker/,$(UBUNTU_TARGETS)): CODENAME := $(UBUNTU_CODENAME)

# Common rule target configuration

MAKEFLAGS += $(PARALLELMFLAGS) --no-builtin-rules

CMAKEFLAGS += -GNinja

DOCKER_RUNFLAGS += --interactive
DOCKER_RUNFLAGS += --rm
DOCKER_RUNFLAGS += --init
DOCKER_RUNFLAGS += --user $(CONTAINER_USER):$(CONTAINER_GROUP)
DOCKER_RUNFLAGS += --device /dev/fuse --cap-add SYS_ADMIN --security-opt apparmor:unconfined
DOCKER_RUNFLAGS += --env SOURCE_DATE_EPOCH
DOCKER_RUNFLAGS += --env BUILDTIME

DOCKER_BUILDARGS += CODENAME=$(CODENAME)
DOCKER_BUILDARGS += PARALLELMFLAGS=$(PARALLELMFLAGS)
DOCKER_BUILDARGS += USERID=$(UID)
DOCKER_BUILDARGS += WORKSPACE=$(CONTAINER_WORKSPACE)

DOCKER_BUILDFLAGS += --rm
DOCKER_BUILDFLAGS += $(addprefix --build-arg ,$(DOCKER_BUILDARGS))

OUT_DIRS += $(OUT)
OUT_DIRS += $(OUT)/docker
OUT_DIRS += $(OUT)/src
OUT_DIRS += $(addprefix $(OUT)/,$(TARGETS))

BUILD_TARGETS += $(addprefix build-,$(TARGETS))
CHECK_TARGETS += $(addprefix check-,$(TARGETS))
EXTRACT_TARGETS += $(patsubst $(OUT)/%.tar.gz,$(OUT)/src/%,$(FETCH_TARGETS))
RULE_TARGETS = $(addprefix $(OUT)/,$(addsuffix /rules.mk,$(TARGETS)))

BUILD_TARGETS := $(BUILD_TARGETS)
CHECK_TARGETS := $(CHECK_TARGETS)
MEMCHECK_TARGETS := $(MEMCHECK_TARGETS)
CONTAINER_GROUP := $(CONTAINER_GROUP)
VERSION := $(VERSION)
PROJECT_ROOT := $(PROJECT_ROOT)
OUT := $(OUT)

# Macros

echo_if_silent = VERBOSE=1
$(VERBOSE)echo_if_silent = echo $1
$(VERBOSE)SILENT := @

image_rule = $$(OUT)/docker/$1: $$(OUT)/docker/$1.dockerfile $$(EXTRACT_TARGETS) $$(PROJECT_ROOT)/Makefile; $$(call image,$1)
image = $(SILENT) \
     $(call echo_if_silent,docker build $(PROJECT_NAME)-$1:$(VERSION) $(OUT)) \
  && $(DOCKER) build $(DOCKER_BUILDFLAGS) --iidfile $@ --file $< --tag $(PROJECT_NAME)-$1:$(VERSION) $(OUT)

run_rule = run-$1: $$(OUT)/docker/$1; $$(call run,$1,/bin/bash,--tty)
run = $(SILENT) \
     $(call echo_if_silent,TARGET=$1 $2) \
  && $(DOCKER) run $(DOCKER_RUNFLAGS) $3 \
       --volume '$(realpath $(PROJECT_ROOT)):$(CONTAINER_WORKSPACE)' \
       --volume '$(realpath $(OUT)/$1):$(CONTAINER_WORKSPACE)/$(notdir $(OUT))' \
       --workdir '$(CONTAINER_WORKSPACE)/$(notdir $(OUT))' \
       $(PROJECT_NAME)-$1:$(VERSION) \
       $2

configure_rule = $$(OUT)/$1/CMakeCache.txt: $$(PROJECT_ROOT)/CMakeLists.txt $$(OUT)/docker/$1; $$(call configure,$1)
configure = $(call run,$1,cmake $(CMAKEFLAGS) ..) && touch $@

build_rule = build-$1: $$(OUT)/$1/CMakeCache.txt; $$(call build,$1)
build = $(call run,$1,ninja $(PARALLELMFLAGS) $(GLOAS))

check_rule = check-$1: build-$1; $$(call check,$1)
check = $(call run,$1,ctest $(CTESTFLAGS))

memcheck_rule = memcheck-$1: build-$1; $$(call memcheck,$1)
memcheck = $(call run,$1,ctest -T memcheck $(CTESTFLAGS))

# Rules

ifneq ($(MAKECMDGOALS),clean)
-include $(RULE_TARGETS)
endif

$(RULE_TARGETS): $(PROJECT_ROOT)/Makefile | $(OUT_DIRS)
	$(SILENT) \
	{ \
		echo; \
		echo '$(call image_rule,$(TARGET))'; \
		echo; \
		echo '$(call configure_rule,$(TARGET))'; \
		echo; \
		echo '$(call build_rule,$(TARGET))'; \
		echo; \
		echo '$(call check_rule,$(TARGET))'; \
		echo; \
		echo '$(call memcheck_rule,$(TARGET))'; \
		echo; \
		echo '$(call run_rule,$(TARGET))'; \
	} > $@

.PHONY: all
all: $(BUILD_TARGETS)

.PHONY: check
check: $(CHECK_TARGETS)

.PHONY: memcheck
memcheck: $(MEMCHECK_TARGETS)

.PHONY: clean
clean: $(CLEAN_TARGETS)
	$(SILENT)-rm -rf $(OUT_DIRS)

.PHONY: get-deps
get-deps: $(EXTRACT_TARGETS)

.PHONY: debug-print-%
debug-print-%:
	@printf '%s\n' '$*:' $($*)

$(CHECK_TARGETS) $(MEMCHECK_TARGETS): CONTAINER_USER := user

$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION):
	$(SILENT) \
	     $(call echo_if_silent,curl -fsSL -o $@ $(URL)) \
	  && curl -fsSL -o $@ $(URL) \
	  && chmod +x $@ 

$(OUT)/docker/% : $(PROJECT_ROOT)/build/% | $(OUT_DIRS)
	cp $< $@

$(OUT)/%.tar.gz: | $(OUT_DIRS)
	curl -fsSL -o $@ $(URL)

$(OUT)/src/%: $(OUT)/%.tar.gz | $(OUT_DIRS)
	$(SILENT) \
	     $(call echo_if_silent,tar -C $(dir $@) -xf $<) \
	  && tar -C $(dir $@) -xf $< \
	  && touch $@

$(OUT_DIRS):
	$(SILENT)mkdir -p $@

.PRECIOUS: $(OUT)/docker/% $(OUT)/%/CMakeCache.txt
.DELETE_ON_ERROR: $(OUT)/%/CMakeCache.txt

