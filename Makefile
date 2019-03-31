PARALLELMFLAGS ?= -j$(shell nproc)
MAKEFLAGS += $(PARALLELMFLAGS) --no-builtin-rules

.PHONY: default
default: all

# Overridable defaults

export SOURCE_DATE_EPOCH ?= $(shell $(PROJECT_ROOT)/build/get_source_date_epoch.sh)
export BUILDTIME ?= $(shell date -u -d '@$(SOURCE_DATE_EPOCH)' --rfc-3339 ns 2>/dev/null | sed -e 's/ /T/')
export DOCKER ?= docker

DOCKER_BUILDKIT ?= 

VERBOSE ?= 
BUILDVERBOSE ?= 
BUILDTARGET ?= 
BUILDTYPE ?= Debug
MARCH ?= $(call march,$(BUILDTARGET))

PROJECT_NAME ?= webfuse
PROJECT_ROOT ?= .
VERSION ?= $(shell cat $(PROJECT_ROOT)/VERSION)
OUT ?= $(PROJECT_ROOT)/.build

UID ?= $(shell id -u)

CONTAINER_USER ?= user
CONTAINER_GROUP ?= user
CONTAINER_CGROUP_PARENT ?= 

HOST_CONTAINER ?= $(shell $(PROJECT_ROOT)/build/get_container_id.sh)
HOST_CONTAINER := $(HOST_CONTAINER)

$(HOST_CONTAINER)PORTABLE_WORSPACE ?= 
CONTAINER_PROJECT_ROOT ?= /workspace/src
CONTAINER_OUT ?= /workspace/out
$(PORTABLE_WORSPACE)CONTAINER_PROJECT_ROOT = $(abspath $(PROJECT_ROOT))
$(PORTABLE_WORSPACE)CONTAINER_OUT = $(abspath $(OUT))

UBUNTU_CODENAME ?= bionic

# Dependencies

DUMB_INIT_VERISON ?= 1.2.2
DOCKER_BUILDARGS += DUMB_INIT_VERISON=$(DUMB_INIT_VERISON)
FETCH_TARGETS += $(OUT)/dumb-init-$(DUMB_INIT_VERISON).tar.gz
$(OUT)/dumb-init-$(DUMB_INIT_VERISON).tar.gz: URL := https://github.com/Yelp/dumb-init/archive/v${DUMB_INIT_VERISON}.tar.gz

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

march = $(shell echo '$1' | sed -n -e 's@\([^-]*\)-.*@\1@p')

TARGETS += $(BUILDTARGET)
MARCH := $(MARCH)

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

CMAKEFLAGS += '-GNinja'
CMAKEFLAGS += '-DCMAKE_BUILD_TYPE=$(BUILDTYPE)'

BUILDSILENT := $(if $(BUILDVERBOSE),,1)
$(BUILDSILENT)NINJAFLAGS += -v

DOCKER_RUNFLAGS += --device /dev/fuse
DOCKER_RUNFLAGS += --cap-add SYS_ADMIN
DOCKER_RUNFLAGS += --security-opt apparmor:unconfined

DOCKER_RUNFLAGS += --cap-add SYS_PTRACE
DOCKER_RUNFLAGS += --security-opt seccomp=unconfined

DOCKER_RUNFLAGS += --interactive
DOCKER_RUNFLAGS += --rm
DOCKER_RUNFLAGS += --user $(CONTAINER_USER):$(CONTAINER_GROUP)
DOCKER_RUNFLAGS += --env SOURCE_DATE_EPOCH
DOCKER_RUNFLAGS += --env BUILDTIME
DOCKER_RUNFLAGS += --env NINJA_STATUS

DOCKER_BUILDARGS += CODENAME=$(CODENAME)
DOCKER_BUILDARGS += PARALLELMFLAGS=$(PARALLELMFLAGS)
DOCKER_BUILDARGS += USERID=$(UID)
DOCKER_BUILDARGS += PROJECT_ROOT=$(CONTAINER_PROJECT_ROOT)
DOCKER_BUILDARGS += OUT=$(CONTAINER_OUT)

DOCKER_BUILDFLAGS += --rm
DOCKER_BUILDFLAGS += $(addprefix --build-arg ,$(DOCKER_BUILDARGS))

OUT_TARGETS += $(addprefix $(OUT)/,$(TARGETS))

OUT_DIRS += $(OUT)
OUT_DIRS += $(OUT)/docker
OUT_DIRS += $(OUT)/src
OUT_DIRS += $(addsuffix /$(BUILDTYPE),$(OUT_TARGETS))

BUILD_TARGETS += $(addprefix build-,$(TARGETS))
CHECK_TARGETS += $(addprefix check-,$(TARGETS))
CLEAN_TARGETS += $(addprefix clean-,$(TARGETS))
RUN_TARGETS += $(addprefix run-,$(firstword $(TARGETS)))
EXTRACT_TARGETS += $(patsubst $(OUT)/%.tar.gz,$(OUT)/src/%,$(FETCH_TARGETS))
DISCOVER_CC_TARGETS += $(addprefix discover-cc-,$(firstword $(TARGETS)))
RULE_TARGETS += $(addsuffix /rules.mk,$(OUT_TARGETS))

TARGETS := $(sort $(TARGETS))

# Macros

echo_if_silent = VERBOSE=1
$(VERBOSE)echo_if_silent = echo $1
$(VERBOSE)SILENT := @

$(HOST_CONTAINER)image_run_volumes += '$(realpath $(PROJECT_ROOT)):$(CONTAINER_PROJECT_ROOT):cached'
$(HOST_CONTAINER)image_run_volumes += '$(realpath $(OUT)/$1):$(CONTAINER_OUT)/$1:delegated'

image_name = $(REGISTRY_PREFIX)$(subst -,/,$1)/$(PROJECT_NAME):$(VERSION)
image_run = $(DOCKER) run $(DOCKER_RUNFLAGS) $3 \
  $(addprefix --volume ,$(call image_run_volumes,$1)) \
  --workdir '$(CONTAINER_OUT)/$1/$(BUILDTYPE)' \
  $(call image_name,$1) \
  $2

image_rule = \
  $$(OUT)/docker/$1: $$(OUT)/docker/$1.dockerfile $$(EXTRACT_TARGETS) $$(PROJECT_ROOT)/Makefile; \
    $$(SILENT)$$(call image,$1)
image = \
     $(call echo_if_silent,TARGET=$1 docker build $(call image_name,$1) $(OUT)) \
  && $(DOCKER) build $(DOCKER_BUILDFLAGS) --iidfile $@ --file $< --tag $(call image_name,$1) $(OUT)

configure_rule = \
  $$(OUT)/$1/$$(BUILDTYPE)/CMakeCache.txt: $$(PROJECT_ROOT)/CMakeLists.txt $$(OUT)/docker/$1; \
    $$(SILENT)$$(call configure,$1)
configure = \
     $(call run,$1,sh -c 'cmake $(CMAKEFLAGS) $(CONTAINER_PROJECT_ROOT) && $(CONTAINER_PROJECT_ROOT)/build/discover_cc_settings.sh $(notdir $@) $(realpath $(dir $@))') \
  && touch $(addprefix $(dir $@)/,include_dirs.txt) $@

build_rule = \
  build-$1: $$(OUT)/$1/$$(BUILDTYPE)/CMakeCache.txt; \
    $$(SILENT)$$(call build,$1)
build = $(call run,$1,ninja $(PARALLELMFLAGS) $(NINJAFLAGS) $(GOALS))

check_rule = \
  check-$1: build-$1;

memcheck_rule = \
  memcheck-$1: build-$1; \
    $$(SILENT)$$(call memcheck,$1)
memcheck = $(call run,$1,ctest -T memcheck $(CTESTFLAGS))

run_rule = \
  run-$1: $$(OUT)/docker/$1; \
    $$(SILENT)$$(call run,$1,bash,--tty) || true
run = $(call echo_if_silent,TARGET=$1 BUILDTYPE=$(BUILDTYPE) $2) && $(call image_run,$1,$2,$3)

clean_rule = \
  clean-$1: ; \
    $$(SILENT)-$$(call clean,$1)
clean = rm -rf $(OUT)/$1

discover_cc_settings_rule = \
  $$(OUT)/$1/$$(BUILDTYPE)/include_dirs.txt: $$(OUT)/$1/$$(BUILDTYPE)/CMakeCache.txt; \
    $$(SILENT)$$(call discover_cc_settings,$1)
discover_cc_settings = \
  $(call run,$1,$(CONTAINER_PROJECT_ROOT)/build/discover_cc_settings.sh $(notdir $<) $(realpath $(dir $<)))

discover_cc_rule = \
  discover-cc-$1: $$(OUT)/$1/$$(BUILDTYPE)/include_dirs.txt; \
    $$(SILENT)$$(call discover_cc,$1)
discover_cc = cat $<

wrapper_rule = \
  $$(OUT)/$1/$$(BUILDTYPE)/gdbserver: $$(PROJECT_ROOT)/build/run_image.template $$(OUT)/docker/$1; \
    $$(SILENT)$$(call wrapper,$1)
wrapper = \
     $(call echo_if_silent,generating $@) \
  && sed \
       -e 's@%PROJECT_ROOT%@$(abspath $(PROJECT_ROOT))@g' \
       -e 's@%DOCKER%@$(DOCKER)@g' \
       -e 's@%RUNFLAGS%@$(DOCKER_RUNFLAGS) $(call image_run_volumes,$1)@g' \
       -e 's@%IMAGE%@$(call image_name,$1)@g' \
       -e 's@%RUNCMD%@$(notdir $@)@g' \
       $< > $@ \
  && chmod +x $@

# Rules

ifneq ($(MAKECMDGOALS),distclean)
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
		echo; \
		echo '$(call clean_rule,$(TARGET))'; \
		echo; \
		echo '$(call discover_cc_settings_rule,$(TARGET))'; \
		echo; \
		echo '$(call discover_cc_rule,$(TARGET))'; \
		echo; \
		echo '$(call wrapper_rule,$(TARGET))'; \
	} > $@

.PHONY: all build-%
all: $(BUILD_TARGETS);

.PHONY: check check-%
check: $(CHECK_TARGETS);

.PHONY: memcheck memcheck-%
memcheck: $(MEMCHECK_TARGETS);

.PHONY: lint lint-%
lint: $(LINT_TARGETS);

.PHONY: run run-%
run: $(RUN_TARGETS);

.PHONY: clean clean-%
clean: $(CLEAN_TARGETS);

.PHONY: get-deps get-deps-%
get-deps: $(EXTRACT_TARGETS);

.PHONY: discover-cc discover-cc-%
discover-cc: $(DISCOVER_CC_TARGETS);

.PHONY: distclean
distclean:
	$(SILENT)-rm -rf $(OUT_DIRS)

.PHONY: debug-print-%
debug-print-%:
	@printf '%s\n' '$*:' $($*)

$(CHECK_TARGETS): GOALS := test

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

