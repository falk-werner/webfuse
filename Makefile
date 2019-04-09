MAKEFLAGS += $(_PARALLELMFLAGS) --no-builtin-rules

filter_targets = $(shell echo '$2' | sed -e 's@\s@\n@g' | sed -n$(foreach REGEX,$1, -e 's@$(REGEX)@\0@p'))

filter_out_command = $(filter $1,$(foreach CMD,$1,$(shell command -v $(CMD) 2>&1 1>/dev/null || echo $(CMD))))

regex_march_distro = '$1-$2-.*'

.PHONY: default
default: all

# Overridable defaults

DOCKER ?= docker
DOCKER_BUILDKIT ?= 

PARALLELMFLAGS ?= 
NPROC ?= $(shell echo '$(PARALLELMFLAGS)' | sed -n -e 's@.*-j.*\([0-9]\+\)@\1@p')

VERBOSE ?= 
BUILDVERBOSE ?= 

DISABLE_DEFAULT_BUILDTARGET := $(or $(MARCH),$(DISTRO))
$(DISABLE_DEFAULT_BUILDTARGET)BUILDTARGET ?= amd64-ubuntu-builder
BUILDTYPE ?= Debug
MARCH ?= '.*'
DISTRO ?= '.*'
FILTER ?= $(call regex_march_distro,$(MARCH),$(DISTRO))

PROJECT_NAME ?= webfuse
PROJECT_ROOT ?= .
OUT ?= $(PROJECT_ROOT)/.build

VERSION ?= $(shell cat $(PROJECT_ROOT)/VERSION)
VERSION := $(VERSION)

USERID ?= $(shell id -u)
USERID := $(USERID)

CONTAINER_USER ?= user
CONTAINER_GROUP ?= user
CONTAINER_CGROUP_PARENT ?= 

HOST_MARCH ?= $(shell dpkg --print-architecture 2>/dev/null)
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

CMAKE_TARGETS += amd64-ubuntu-builder
$(OUT)/amd64-ubuntu-builder/cmake_rules.mk: TARGET := amd64-ubuntu-builder

CMAKE_TARGETS += amd64-debian-builder
$(OUT)/amd64-debian-builder/cmake_rules.mk: TARGET := amd64-debian-builder

CMAKE_TARGETS += arm32v7-ubuntu-builder
$(OUT)/arm32v7-ubuntu-builder/cmake_rules.mk: TARGET := arm32v7-ubuntu-builder

CMAKE_TARGETS += arm32v7-debian-builder
$(OUT)/arm32v7-debian-builder/cmake_rules.mk: TARGET := arm32v7-debian-builder

CMAKE_RULE_TARGETS += $(addsuffix /cmake_rules.mk,$(addprefix $(OUT)/,$(CMAKE_TARGETS)))

MEMCHECK_FILTER = $(call regex_march_distro,'$(HOST_MARCH)','.*')

UBUNTU_FILTER = $(call regex_march_distro,'.*','ubuntu')
UBUNTU_TARGETS = $(addprefix $(OUT)/docker/,$(call filter_targets,$(UBUNTU_FILTER),$(TARGETS)))

DEBIAN_FILTER = $(call regex_march_distro,'.*','debian')
DEBIAN_TARGETS = $(addprefix $(OUT)/docker/,$(call filter_targets,$(DEBIAN_FILTER),$(TARGETS)))

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

DOCKER_RUNFLAGS += --user $(CONTAINER_USER):$(CONTAINER_GROUP)
DOCKER_RUNFLAGS += --env SOURCE_DATE_EPOCH
DOCKER_RUNFLAGS += --env BUILDTIME
DOCKER_RUNFLAGS += --env NINJA_STATUS

DOCKER_BUILDARGS += CODENAME=$(CODENAME)
DOCKER_BUILDARGS += PARALLELMFLAGS=$(_PARALLELMFLAGS)
DOCKER_BUILDARGS += USERID=$(USERID)
DOCKER_BUILDARGS += PROJECT_ROOT=$(CONTAINER_PROJECT_ROOT)
DOCKER_BUILDARGS += OUT=$(CONTAINER_OUT)
DOCKER_BUILDARGS += REGISTRY_PREFIX=$(REGISTRY_PREFIX)

DOCKER_BUILDFLAGS += $(addprefix --build-arg ,$(DOCKER_BUILDARGS))

OUT_DIRS += $(OUT)
OUT_DIRS += $(OUT)/docker
OUT_DIRS += $(OUT)/src

CMAKE_TARGET := $(filter $(BUILDTARGET),$(CMAKE_TARGETS))
$(CMAKE_TARGET)CMAKE_ACTIVE_TARGETS += $(CMAKE_TARGETS)
CMAKE_ACTIVE_TARGETS += $(CMAKE_TARGET)

CMAKE_BUILD_TARGETS += $(addprefix build-,$(CMAKE_TARGETS))
CMAKE_CLEAN_TARGETS += $(addprefix clean-,$(CMAKE_TARGETS))
CMAKE_CHECK_TARGETS += $(addprefix check-,$(CMAKE_TARGETS))
CMAKE_MEMCHECK_TARGETS += $(addprefix memcheck-,$(call filter_targets,$(MEMCHECK_FILTER),$(CMAKE_TARGETS)))
CMAKE_RUN_TARGETS += $(addprefix run-,$(firstword $(CMAKE_TARGETS)))
CMAKE_DISCOVER_CC_TARGETS += $(addprefix discover-cc-,$(firstword $(CMAKE_TARGETS)))
CMAKE_TARGETS := $(call filter_targets,$(FILTER),$(CMAKE_ACTIVE_TARGETS))
CMAKE_OUT_DIRS += $(addsuffix /$(BUILDTYPE),$(addprefix $(OUT)/,$(CMAKE_TARGETS)))

EXTRACT_TARGETS += $(patsubst $(OUT)/%.tar.gz,$(OUT)/src/%,$(FETCH_TARGETS))
BUILD_TARGETS += $(CMAKE_BUILD_TARGETS)
CHECK_TARGETS += $(CMAKE_CHECK_TARGETS)
MEMCHECK_TARGETS += $(CMAKE_MEMCHECK_TARGETS)
CLEAN_TARGETS += $(CMAKE_CLEAN_TARGETS)
RUN_TARGETS += $(CMAKE_RUN_TARGETS)
DISCOVER_CC_TARGETS += $(CMAKE_DISCOVER_CC_TARGETS)
RULE_TARGETS += $(CMAKE_RULE_TARGETS)
TARGETS += $(CMAKE_TARGETS)
OUT_DIRS += $(CMAKE_OUT_DIRS)

OUT_DIRS := $(sort $(OUT_DIRS))
TARGETS := $(sort $(TARGETS))
TASKS := $(words $(if $(TARGETS),$(TARGETS),_))

DISBALE_OSYNC ?= $(filter 1,$(TASKS))
$(DISBALE_OSYNC)GNUMAKEFLAGS += --output-sync

_NPROC = $(NPROC)
ifeq ($(strip $(_NPROC)),)
_NPROC = $(shell nproc)
endif
_PARALLELMFLAGS := $(addprefix -j,$(shell echo "$$(($(_NPROC)/$(TASKS)))"))

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

MISSING_MAKEDEPS := $(call filter_out_command,$(MAKEDEPS))

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

image_name = $(REGISTRY_PREFIX)$(subst -,/,$1)/$(PROJECT_NAME)$(addprefix :,$(VERSION))
image_run = $(DOCKER) run --rm --interactive $(DOCKER_RUNFLAGS) \
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
  && $(DOCKER) build --rm $(DOCKER_BUILDFLAGS) --iidfile $@ --file $< --tag $(call image_name,$1) $(OUT)

cmake_configure_rule = \
  $$(OUT)/$1/$$(BUILDTYPE)/CMakeCache.txt: $$(PROJECT_ROOT)/CMakeLists.txt $$(OUT)/docker/$1 | $$(OUT)/$1/$$(BUILDTYPE)/gdbserver; \
    $$(SILENT)$$(call cmake_configure,$1)
cmake_configure = \
     $(call run,$1,sh -c 'cmake $(CMAKEFLAGS) $(CONTAINER_PROJECT_ROOT) && $(CONTAINER_PROJECT_ROOT)/build/cmake_discover_cc_settings.sh $(notdir $@) $(realpath $(dir $@))') \
  && touch $(addprefix $(dir $@)/,include_dirs.txt) $@

ninja_build_rule = \
  build-$1: $$(OUT)/$1/$$(BUILDTYPE)/CMakeCache.txt; \
    $$(SILENT)$$(call ninja_build,$1)
ninja_build = $(call run,$1,ninja $(_PARALLELMFLAGS) $(NINJAFLAGS) $(GOALS))

check_rule = \
  check-$1: build-$1;

ctest_memcheck_rule = \
  memcheck-$1: build-$1; \
    $$(SILENT)$$(call ctest_memcheck,$1)
ctest_memcheck = $(call run,$1,ctest -T memcheck $(CTESTFLAGS))

run_rule = \
  run-$1: $$(OUT)/docker/$1; \
    $$(SILENT)$$(call run,$1,bash,--tty) || true
run = $(call echo_if_silent,TARGET=$1 BUILDTYPE=$(BUILDTYPE) $2) && $(call image_run,$1,$2,$3)

clean_rule = \
  clean-$1: ; \
    $$(SILENT)-$$(call clean,$1)
clean = rm -rf $(OUT)/$1

cmake_discover_cc_settings_rule = \
  $$(OUT)/$1/$$(BUILDTYPE)/include_dirs.txt: $$(OUT)/$1/$$(BUILDTYPE)/CMakeCache.txt; \
    $$(SILENT)$$(call cmake_discover_cc_settings,$1)
cmake_discover_cc_settings = \
  $(call run,$1,$(CONTAINER_PROJECT_ROOT)/build/cmake_discover_cc_settings.sh $(notdir $<) $(realpath $(dir $<)))

discover_cc_rule = \
  discover-cc-$1: $$(OUT)/$1/$$(BUILDTYPE)/include_dirs.txt; \
    $$(SILENT)$$(call discover_cc,$1)
discover_cc = cat $<

wrapper_rule = \
  $$(OUT)/$1/$$(BUILDTYPE)/gdbserver: $$(PROJECT_ROOT)/build/run_image.sh.template $$(OUT)/docker/$1; \
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

$(RULE_TARGETS): $(PROJECT_ROOT)/Makefile | $(MISSING_MAKEDEPS) $(OUT_DIRS)

$(CMAKE_RULE_TARGETS):
	$(SILENT) \
	{ \
	  echo '$(call image_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call cmake_configure_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call ninja_build_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call check_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call ctest_memcheck_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call run_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call clean_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call cmake_discover_cc_settings_rule,$(TARGET))'; \
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

$(UBUNTU_TARGETS): CODENAME := $(UBUNTU_CODENAME)

$(DEBIAN_TARGETS): CODENAME := $(DEBIAN_CODENAME)

$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION): $(PROJECT_ROOT)/Makefile
	$(SILENT)$(call curl,$@,$(URL),$(MD5)) && chmod +x $@ 

$(OUT)/docker/% : $(PROJECT_ROOT)/build/% | $(OUT_DIRS)
	cp $< $@

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

