.PHONY: default
default: all

# Overridable defaults

VERBOSE ?=
PROJECT_NAME ?= webfs
PROJECT_ROOT ?= .
OUT ?= $(PROJECT_ROOT)/.build
VERSION ?= $(shell cat $(PROJECT_ROOT)/VERSION)
PARALLELMFLAGS ?= -j$(shell nproc)
UID ?= $(shell id -u)
DOCKER ?= docker
CONTAINER_USER ?= $(UID)
CONTAINER_GROUP ?= $(shell id -g)
CONTAINER_WORKSPACE ?= /workspace

UBUNTU_CODENAME ?= bionic

QEMU_VERSION ?= v3.1.0-2

GTEST_VERSION ?= 1.8.1
DOCKER_BUILDARGS += GTEST_VERSION=$(GTEST_VERSION)

FUSE_VERSION ?= 3.1.1
DOCKER_BUILDARGS += FUSE_VERSION=$(FUSE_VERSION)

WEBSOCKETS_VERSION ?= 3.1.0
DOCKER_BUILDARGS += WEBSOCKETS_VERSION=$(WEBSOCKETS_VERSION)

JANSSON_VERSION ?= 2.12
DOCKER_BUILDARGS += JANSSON_VERSION=$(JANSSON_VERSION)

# Target configuration

MARCH_AMD64 := $(filter-out amd64,$(MARCH))
$(MARCH_AMD64)MARCHS += amd64
$(MARCH_AMD64)TARGETS += amd64-ubuntu-builder

MARCH_ARM32V7 := $(filter-out arm32v7,$(MARCH))
$(MARCH_ARM32V7)MARCHS += arm32v7
$(MARCH_ARM32V7)TARGETS += arm32v7-ubuntu-builder

ARM_TARGETS = $(filter arm%,$(TARGETS))
UBUNTU_TARGETS = $(filter $(addsuffix -ubuntu%,$(MARCHS)),$(TARGETS))

MAKEFLAGS += $(PARALLELMFLAGS) --no-builtin-rules

CMAKEFLAGS += -GNinja

DOCKER_RUNFLAGS += --interactive
DOCKER_RUNFLAGS += --rm
DOCKER_RUNFLAGS += --tty
DOCKER_RUNFLAGS += --init
DOCKER_RUNFLAGS += --user $(CONTAINER_USER):$(CONTAINER_GROUP)
DOCKER_RUNFLAGS += --device /dev/fuse --cap-add SYS_ADMIN --security-opt apparmor:unconfined
DOCKER_RUNFLAGS += --env SOURCE_DATE_EPOCH

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

FETCH_TARGETS += $(OUT)/docker/qemu-arm-static-$(QEMU_VERSION)
FETCH_TARGETS += $(OUT)/googletest-release-$(GTEST_VERSION).tar.gz
FETCH_TARGETS += $(OUT)/libfuse-fuse-$(FUSE_VERSION).tar.gz
FETCH_TARGETS += $(OUT)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz
FETCH_TARGETS += $(OUT)/jansson-$(JANSSON_VERSION).tar.gz

EXTRACT_TARGETS += $(patsubst $(OUT)/%.tar.gz,$(OUT)/src/%,$(FETCH_TARGETS))

echo_if_silent = VERBOSE=1
$(VERBOSE)echo_if_silent = echo $1
$(VERBOSE)SILENT := @

CONTAINER_GROUP := $(CONTAINER_GROUP)
VERSION := $(VERSION)
PROJECT_ROOT := $(PROJECT_ROOT)
OUT := $(OUT)

# Rules

$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION): URL := https://github.com/multiarch/qemu-user-static/releases/download/$(QEMU_VERSION)/qemu-arm-static
$(OUT)/googletest-release-$(GTEST_VERSION).tar.gz: URL := https://github.com/google/googletest/archive/release-$(GTEST_VERSION).tar.gz
$(OUT)/libfuse-fuse-$(FUSE_VERSION).tar.gz: URL := https://github.com/libfuse/libfuse/archive/fuse-$(FUSE_VERSION).tar.gz
$(OUT)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz: URL := https://github.com/warmcat/libwebsockets/archive/v$(WEBSOCKETS_VERSION).tar.gz
$(OUT)/jansson-$(JANSSON_VERSION).tar.gz: URL := https://github.com/akheron/jansson/archive/v$(JANSSON_VERSION).tar.gz

$(addprefix $(OUT)/docker/,$(ARM_TARGETS)): $(OUT)/docker/qemu-arm-static-$(QEMU_VERSION)

$(addprefix $(OUT)/docker/,$(UBUNTU_TARGETS)): CODENAME := $(UBUNTU_CODENAME)

.PHONY: all
all: $(BUILD_TARGETS)

.PHONY: check
check: $(CHECK_TARGETS)

.PHONY: clean
clean: $(CLEAN_TARGETS)
	$(SILENT)-rm -rf $(OUT_DIRS)

.PHONY: check-%
check-%: build-%;

.PHONY: build-%
build-%: $(OUT)/%/CMakeCache.txt
	$(SILENT) \
	     $(call echo_if_silent,ninja $(PARALLELMFLAGS) $(GLOAS)) \
	  && $(DOCKER) run $(DOCKER_RUNFLAGS) \
	       --volume '$(realpath $(PROJECT_ROOT)):$(CONTAINER_WORKSPACE)' \
	       --volume '$(realpath $(dir $<)):$(CONTAINER_WORKSPACE)/$(notdir $(OUT))' \
	       --workdir '$(CONTAINER_WORKSPACE)/$(notdir $(OUT))' \
	       $(PROJECT_NAME)-$*:$(VERSION) \
	       ninja $(PARALLELMFLAGS) $(GLOAS)

.PHONY: get-deps
get-deps: $(EXTRACT_TARGETS)

.PHONY: debug-print-%
debug-print-%:
	@printf '%s\n' '$*:' $($*)

$(BUILD_TARGETS):

$(CHECK_TARGETS): GLOAS := test
$(CHECK_TARGETS): CONTAINER_USER := user

$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION):
	$(SILENT) \
	     $(call echo_if_silent,curl -fsSL -o $@ $(URL)) \
	  && curl -fsSL -o $@ $(URL) \
	  && chmod +x $@ 

$(OUT)/% : $(PROJECT_ROOT)/% | $(OUT_DIRS)
	cp $< $@

$(OUT)/docker/%: $(OUT)/docker/%.dockerfile $(EXTRACT_TARGETS) $(PROJECT_ROOT)/Makefile | $(OUT_DIRS)
	$(SILENT) \
	     $(call echo_if_silent,docker build $(PROJECT_NAME)-$*:$(VERSION) $(OUT)) \
	  && $(DOCKER) build $(DOCKER_BUILDFLAGS) --iidfile $@ --file $< --tag $(PROJECT_NAME)-$*:$(VERSION) $(OUT)

$(OUT)/%.tar.gz: | $(OUT_DIRS)
	curl -fsSL -o $@ $(URL)

$(OUT)/src/%: $(OUT)/%.tar.gz | $(OUT_DIRS)
	$(SILENT) \
	     $(call echo_if_silent,tar -C $(dir $@) -xf $<) \
	  && tar -C $(dir $@) -xf $< \
	  && touch $@

$(OUT)/%/CMakeCache.txt: $(PROJECT_ROOT)/CMakeLists.txt $(OUT)/docker/% | $(OUT_DIRS)
	$(SILENT) \
	     $(call echo_if_silent,cmake $(CMAKEFLAGS) ..) \
	  && $(DOCKER) run $(DOCKER_RUNFLAGS) \
	       --volume '$(realpath $(PROJECT_ROOT)):$(CONTAINER_WORKSPACE)' \
	       --volume '$(realpath $(dir $@)):$(CONTAINER_WORKSPACE)/$(notdir $(OUT))' \
	       --workdir '$(CONTAINER_WORKSPACE)/$(notdir $(OUT))' \
	       $(PROJECT_NAME)-$*:$(VERSION) \
	       cmake $(CMAKEFLAGS) .. && touch $@

$(OUT_DIRS):
	$(SILENT)mkdir -p $@

.PRECIOUS: $(OUT)/docker/% $(OUT)/%/CMakeCache.txt
.DELETE_ON_ERROR: $(OUT)/%/CMakeCache.txt

