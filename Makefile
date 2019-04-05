MAKEFLAGS += $(_PARALLELMFLAGS) --no-builtin-rules

.PHONY: default
default: all

# Overridable defaults

DOCKER ?= docker
DOCKER_BUILDKIT ?= 

PARALLELMFLAGS ?= 
NPROC ?= $(shell echo '$(PARALLELMFLAGS)' | sed -n -e 's@.*-j.*\([0-9]\+\)@\1@p')

VERBOSE ?= 
BUILDVERBOSE ?= 

$(MARCH)BUILDTARGET ?= amd64-ubuntu-builder
BUILDTYPE ?= Debug
MARCH ?= $(call march,$(BUILDTARGET))
DISTRO ?=

PROJECT_NAME ?= webfuse
PROJECT_ROOT ?= .
OUT ?= $(PROJECT_ROOT)/.build

VERSION ?= $(shell cat $(PROJECT_ROOT)/VERSION)
VERSION := $(VERSION)

UID ?= $(shell id -u)
UID := $(UID)

CONTAINER_USER ?= user
CONTAINER_GROUP ?= user
CONTAINER_CGROUP_PARENT ?= 

HOST_CONTAINER ?= $(shell $(PROJECT_ROOT)/build/get_container_id.sh)
HOST_CONTAINER := $(HOST_CONTAINER)

UBUNTU_CODENAME ?= bionic
DEBIAN_CODENAME ?= testing-slim

SOURCE_DATE_EPOCH ?= $(shell $(PROJECT_ROOT)/build/get_source_date_epoch.sh)
SOURCE_DATE_EPOCH := $(SOURCE_DATE_EPOCH)
export SOURCE_DATE_EPOCH

BUILDTIME ?= $(shell date -u -d '@$(SOURCE_DATE_EPOCH)' --rfc-3339 ns 2>/dev/null | sed -e 's/ /T/')
BUILDTIME := $(BUILDTIME)
export BUILDTIME

$(HOST_CONTAINER)PORTABLE_WORSPACE ?= 
CONTAINER_PROJECT_ROOT ?= /workspace/src
CONTAINER_OUT ?= /workspace/out
$(PORTABLE_WORSPACE)CONTAINER_PROJECT_ROOT = $(abspath $(PROJECT_ROOT))
$(PORTABLE_WORSPACE)CONTAINER_OUT = $(abspath $(OUT))

filter_out_command = $(filter $1,$(foreach CMD,$1,$(shell command -v $(CMD) 2>&1 1>/dev/null || echo $(CMD))))

DISABLE_MD5SUM ?= $(call filter_out_command,md5sum)
DISABLE_MD5SUM := $(DISABLE_MD5SUM)

# Project dependencies

DUMB_INIT_VERSION ?= 1.2.2
DOCKER_BUILDARGS += DUMB_INIT_VERSION=$(DUMB_INIT_VERSION)
FETCH_TARGETS += $(OUT)/dumb-init-$(DUMB_INIT_VERSION).tar.gz
$(OUT)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: URL := https://github.com/Yelp/dumb-init/archive/v${DUMB_INIT_VERSION}.tar.gz
$(DISABLE_MD5SUM)$(OUT)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: MD5 := 6166084b05772cdcf615a762c6f3b32e

GTEST_VERSION ?= 1.8.1
DOCKER_BUILDARGS += GTEST_VERSION=$(GTEST_VERSION)
FETCH_TARGETS += $(OUT)/googletest-release-$(GTEST_VERSION).tar.gz
$(OUT)/googletest-release-$(GTEST_VERSION).tar.gz: URL := https://github.com/google/googletest/archive/release-$(GTEST_VERSION).tar.gz
$(DISABLE_MD5SUM)$(OUT)/googletest-release-$(GTEST_VERSION).tar.gz: MD5 := 2e6fbeb6a91310a16efe181886c59596

FUSE_VERSION ?= 3.1.1
DOCKER_BUILDARGS += FUSE_VERSION=$(FUSE_VERSION)
FETCH_TARGETS += $(OUT)/libfuse-fuse-$(FUSE_VERSION).tar.gz
$(OUT)/libfuse-fuse-$(FUSE_VERSION).tar.gz: URL := https://github.com/libfuse/libfuse/archive/fuse-$(FUSE_VERSION).tar.gz
$(DISABLE_MD5SUM)$(OUT)/libfuse-fuse-$(FUSE_VERSION).tar.gz: MD5 := 097f194856938afdd98bea1a5c046edd

WEBSOCKETS_VERSION ?= 3.1.0
DOCKER_BUILDARGS += WEBSOCKETS_VERSION=$(WEBSOCKETS_VERSION)
FETCH_TARGETS += $(OUT)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz
$(OUT)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz: URL := https://github.com/warmcat/libwebsockets/archive/v$(WEBSOCKETS_VERSION).tar.gz
$(DISABLE_MD5SUM)$(OUT)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz: MD5 := 325359a25d5f6d22725ff5d086db1c76

JANSSON_VERSION ?= 2.12
DOCKER_BUILDARGS += JANSSON_VERSION=$(JANSSON_VERSION)
FETCH_TARGETS += $(OUT)/jansson-$(JANSSON_VERSION).tar.gz
$(OUT)/jansson-$(JANSSON_VERSION).tar.gz: URL := https://github.com/akheron/jansson/archive/v$(JANSSON_VERSION).tar.gz
$(DISABLE_MD5SUM)$(OUT)/jansson-$(JANSSON_VERSION).tar.gz: MD5 := c4b106528d5ffb521178565de1ba950d

QEMU_VERSION ?= v3.1.0-2
DOCKER_BUILDARGS += QEMU_VERSION_=$(QEMU_VERSION)
FETCH_TARGETS += $(OUT)/docker/qemu-arm-static-$(QEMU_VERSION)
$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION): URL := https://github.com/multiarch/qemu-user-static/releases/download/$(QEMU_VERSION)/qemu-arm-static
$(DISABLE_MD5SUM)$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION): MD5 := 8ebd24e63fdfa07c557d45373bd831b1

# Architecture-specific rule target configuration

march = $(shell echo '$1' | sed -n -e 's@\([^-]*\)-.*@\1@p')

TARGETS += $(BUILDTARGET)
MARCH := $(MARCH)

DISABLE_AMD64 := $(filter-out amd64,$(MARCH))
$(DISABLE_AMD64)MARCHS += amd64

$(DISABLE_AMD64)$(BUILDTARGET)TARGETS += amd64-ubuntu-builder
$(OUT)/amd64-ubuntu-builder/rules.mk: TARGET := amd64-ubuntu-builder

$(DISABLE_AMD64)$(BUILDTARGET)TARGETS += amd64-debian-builder
$(OUT)/amd64-debian-builder/rules.mk: TARGET := amd64-debian-builder

DISABLE_ARM32V7 := $(filter-out arm32v7,$(MARCH))
$(DISABLE_ARM32V7)MARCHS += arm32v7

$(DISABLE_ARM32V7)$(BUILDTARGET)TARGETS += arm32v7-ubuntu-builder
$(OUT)/arm32v7-ubuntu-builder/rules.mk: TARGET := arm32v7-ubuntu-builder

$(DISABLE_ARM32V7)$(BUILDTARGET)TARGETS += arm32v7-debian-builder
$(OUT)/arm32v7-debian-builder/rules.mk: TARGET := arm32v7-debian-builder

$(DISABLE_AMD64)MEMCHECK_TARGETS += $(addprefix memcheck-,$(TARGETS))

ARM_TARGETS = $(filter arm%,$(TARGETS))
$(addprefix $(OUT)/docker/,$(ARM_TARGETS)): $(OUT)/docker/qemu-arm-static-$(QEMU_VERSION)

UBUNTU_TARGETS = $(filter $(addsuffix -ubuntu%,$(MARCHS)),$(TARGETS))
$(addprefix $(OUT)/docker/,$(UBUNTU_TARGETS)): CODENAME := $(UBUNTU_CODENAME)

DEBIAN_TARGETS = $(filter $(addsuffix -debian%,$(MARCHS)),$(TARGETS))
$(addprefix $(OUT)/docker/,$(DEBIAN_TARGETS)): CODENAME := $(DEBIAN_CODENAME)

# Common rule target configuration

CMAKEFLAGS += '-GNinja'
CMAKEFLAGS += '-DCMAKE_BUILD_TYPE=$(BUILDTYPE)'

BUILDSILENT := $(if $(BUILDVERBOSE),,1)
$(BUILDSILENT)NINJAFLAGS += -v

CURLFLAGS += -s

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
DOCKER_BUILDARGS += PARALLELMFLAGS=$(_PARALLELMFLAGS)
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

uc = $(shell echo '$1' | sed -e 's/.*/\U&/g')

DISTRO_PREFIX = $(addsuffix _,$(call uc,$(DISTRO)))

MARCHS := $(sort $(MARCHS))
TARGETS := $(sort $($(DISTRO_PREFIX)TARGETS))

TASKS := $(words $(if $(TARGETS),$(TARGETS),_))

DISBALE_OSYNC ?= $(filter 1,$(TASKS))
$(DISBALE_OSYNC)GNUMAKEFLAGS += --output-sync

_NPROC = $(NPROC)
ifeq ($(strip $(_NPROC)),)
_NPROC = $(shell nproc)
endif
_PARALLELMFLAGS := $(addprefix -j,$(shell echo "$$(($(_NPROC)/$(TASKS)))"))

# Common macros

echo_if_silent = VERBOSE=1
$(VERBOSE)echo_if_silent = echo $1
$(VERBOSE)SILENT := @

curl = $(call echo_if_silent,curl -fSL $(CURLFLAGS) -o $1 $2) \
  && curl -fSL $(CURLFLAGS) -o $1 $2 \
  && { \
       if [ -n "$3" ]; then \
         echo "$3\t$1" > $1.md5; \
         md5sum -c $1.md5; \
       else \
         echo 'warning:$1: no md5 skipping verification' 1>&2; \
       fi; \
     }

$(HOST_CONTAINER)image_run_volumes += --volume '$(realpath $(PROJECT_ROOT)):$(CONTAINER_PROJECT_ROOT):cached'
$(HOST_CONTAINER)image_run_volumes += --volume '$(realpath $(OUT)/$1):$(CONTAINER_OUT)/$1:delegated'
image_run_volumes += $(addprefix --volumes-from ,$2)

image_name = $(REGISTRY_PREFIX)$(subst -,/,$1)/$(PROJECT_NAME):$(VERSION)
image_run = $(DOCKER) run $(DOCKER_RUNFLAGS) \
  $(call image_run_volumes,$1,$(HOST_CONTAINER)) \
  $(addprefix --cgroup-parent ,$(CONTAINER_CGROUP_PARENT)) \
  --workdir '$(CONTAINER_OUT)/$1/$(BUILDTYPE)' \
  $3 \
  $(call image_name,$1) \
  $2

image_rule = \
  $$(OUT)/docker/$1: $$(OUT)/docker/$1.dockerfile $$(EXTRACT_TARGETS) $$(PROJECT_ROOT)/Makefile; \
    $$(SILENT)$$(call image,$1)
image = \
     $(call echo_if_silent,TARGET=$1 docker build $(call image_name,$1) $(OUT)) \
  && $(DOCKER) build $(DOCKER_BUILDFLAGS) --iidfile $@ --file $< --tag $(call image_name,$1) $(OUT)

configure_rule = \
  $$(OUT)/$1/$$(BUILDTYPE)/CMakeCache.txt: $$(PROJECT_ROOT)/CMakeLists.txt $$(OUT)/docker/$1 | $$(OUT)/$1/$$(BUILDTYPE)/gdbserver; \
    $$(SILENT)$$(call configure,$1)
configure = \
     $(call run,$1,sh -c 'cmake $(CMAKEFLAGS) $(CONTAINER_PROJECT_ROOT) && $(CONTAINER_PROJECT_ROOT)/build/discover_cc_settings.sh $(notdir $@) $(realpath $(dir $@))') \
  && touch $(addprefix $(dir $@)/,include_dirs.txt) $@

build_rule = \
  build-$1: $$(OUT)/$1/$$(BUILDTYPE)/CMakeCache.txt; \
    $$(SILENT)$$(call build,$1)
build = $(call run,$1,ninja $(_PARALLELMFLAGS) $(NINJAFLAGS) $(GOALS))

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

# Makefile dependencies

MAKEDEPS += $(DOCKER)
MAKEDEPS += id
MAKEDEPS += cat
MAKEDEPS += cp
MAKEDEPS += rm
MAKEDEPS += mkdir
MAKEDEPS += sed
MAKEDEPS += chmod
MAKEDEPS += test
MAKEDEPS += touch
MAKEDEPS += curl
MAKEDEPS += tar
MAKEDEPS += gunzip

MISSING_MAKEDEPS += $(call filter_out_command,$(MAKEDEPS))

# Rules

ifneq ($(MAKECMDGOALS),distclean)
-include $(RULE_TARGETS)
endif

$(RULE_TARGETS): $(PROJECT_ROOT)/Makefile | $(MISSING_MAKEDEPS) $(OUT_DIRS)
	$(SILENT) \
	{ \
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

$(OUT)/docker/% : $(PROJECT_ROOT)/build/% | $(OUT_DIRS)
	cp $< $@

$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION): $(PROJECT_ROOT)/Makefile
	$(SILENT)$(call curl,$@,$(URL),$(MD5)) && chmod +x $@ 

$(OUT)/%.tar.gz: $(PROJECT_ROOT)/Makefile | $(OUT_DIRS)
	$(SILENT)$(call curl,$@,$(URL),$(MD5))

$(OUT)/src/%: $(OUT)/%.tar.gz | $(OUT_DIRS)
	$(SILENT) \
	     $(call echo_if_silent,tar -C $(dir $@) -xf $<) \
	  && tar -C $(dir $@) -xf $< \
	  && touch $@

$(OUT_DIRS):
	$(SILENT)mkdir -p $@

$(MISSING_MAKEDEPS):
	$(error 'required commands $(MISSING_MAKEDEPS) not found; install appropriate packages e.g. docker-ce, busybox and curl')

.DELETE_ON_ERROR: $(FETCH_TARGETS)

