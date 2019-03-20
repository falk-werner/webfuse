MAKEFLAGS += --no-builtin-rules

.PHONY: default
default: all

## Overridable defaults

$(VERBOSE)SILENT ?= @
PROJECT_ROOT ?= .
OUT ?= $(PROJECT_ROOT)/.build
VERSION ?= $(shell cat $(PROJECT_ROOT)/VERSION)
NPROC ?= $(shell nproc)
UID ?= $(shell id -u)
DOCKER ?= docker
DOCKER_RUNUSER ?= $(UID)
DOCKER_RUNGROUP ?= $(shell id -g)

UBUNTU_CODENAME ?= bionic
QEMU_VERSION ?= v3.1.0-2

MARCH_AMD64 := $(filter-out amd64,$(MARCH))
MARCH_ARM32V7 := $(filter-out arm32v7,$(MARCH))

## Defaults

$(MARCH_AMD64)TARGETS += wsfs-builder-amd64-ubuntu
$(MARCH_ARM32V7)TARGETS += wsfs-builder-arm32v7-ubuntu

DOCKER_RUNFLAGS += --interactive
DOCKER_RUNFLAGS += --rm
DOCKER_RUNFLAGS += --tty
DOCKER_RUNFLAGS += --init
DOCKER_RUNFLAGS += --user $(DOCKER_RUNUSER):$(DOCKER_RUNGROUP)
DOCKER_RUNFLAGS += --device /dev/fuse --cap-add SYS_ADMIN --security-opt apparmor:unconfined
DOCKER_RUNFLAGS += --env SOURCE_DATE_EPOCH

DOCKER_BUILDARGS += CODENAME=$(CODENAME)
DOCKER_BUILDARGS += NPROC=$(NPROC)
DOCKER_BUILDARGS += USERID=$(UID)

DOCKER_BUILDFLAGS += $(addprefix --build-arg ,$(DOCKER_BUILDARGS))

OUT_DIRS += $(OUT)
OUT_DIRS += $(OUT)/docker
OUT_DIRS += $(addprefix $(OUT)/,$(TARGETS))

PROJECT_RESOURCES = $(call glob_files,$(PROJECT_ROOT)/**)

ALL_TARGETS += $(addprefix compile-,$(TARGETS)) 
CHECK_TARGETS += $(addprefix check-,$(TARGETS))
CONFIGURE_TARGETS += $(addsuffix /CMakeCache.txt,$(addprefix $(OUT)/,$(TARGETS)))

## Helper macros

# $(call glob,include_pattern...,exclude_regex...,flags...)
glob = $(shell bash -c 'shopt -s globstar nullglob && find $1 -maxdepth 0 $3 | grep -v $(addprefix -e, $2)')
# $(call glob_files,include_pattern...,exclude_regex...)
glob_files = $(call glob,$1,^$(OUT) $2,-type f)

## Force eager evaluation

DOCKER_RUNGROUP := $(DOCKER_RUNGROUP)
VERSION := $(VERSION)
PROJECT_ROOT := $(PROJECT_ROOT)
OUT := $(OUT)

## Special targets

.PRECIOUS: $(OUT)/docker/%
.DELETE_ON_ERROR: $(OUT)/%/CMakeCache.txt $(OUT)/docker/%

## Targets

.PHONY: all
all: $(ALL_TARGETS)

.PHONY: check
check: $(CHECK_TARGETS)

.PHONY: clean
clean: $(CLEAN_TARGETS)
	$(SILENT)-rm -rf $(OUT_DIRS)

.PHONY: configure
configure: $(CONFIGURE_TARGETS) 

%-ubuntu: CODENAME := $(UBUNTU_CODENAME)

$(PROJECT_ROOT)/docker/qemu-arm-static-$(QEMU_VERSION):
	$(SILENT) \
	     curl -fSL -o $@ https://github.com/multiarch/qemu-user-static/releases/download/$(QEMU_VERSION)/qemu-arm-static \
	  && chmod +x $@ 

$(OUT)/docker/wsfs-builder-arm32v7-ubuntu: $(PROJECT_ROOT)/docker/qemu-arm-static-$(QEMU_VERSION)

$(OUT)/docker/%: $(PROJECT_ROOT)/docker/%.dockerfile $(PROJECT_RESOURCES) | $(OUT_DIRS)
	$(SILENT)$(DOCKER) build --rm $(DOCKER_BUILDFLAGS) --iidfile $@ --file $< --tag $*:$(VERSION) $(dir $<)

$(OUT)/%/CMakeCache.txt: $(PROJECT_ROOT)/CMakeLists.txt $(OUT)/docker/% | $(OUT_DIRS)
	$(SILENT)$(DOCKER) run $(DOCKER_RUNFLAGS) \
	  --volume '$(realpath $(PROJECT_ROOT)):/tmp' \
	  --volume '$(realpath $(dir $@)):/tmp/$(notdir $(OUT))' \
	  --workdir '/tmp/$(notdir $(OUT))' \
	  $*:$(VERSION) \
	  cmake -GNinja $(CMAKEFLAGS) .. && touch $@

$(CHECK_TARGETS): GLOAS := test
$(CHECK_TARGETS): DOCKER_RUNUSER := user

check-%: compile-%;

compile-%: $(OUT)/%/CMakeCache.txt
	$(SILENT)$(DOCKER) run $(DOCKER_RUNFLAGS) \
	  --volume '$(realpath $(PROJECT_ROOT)):/tmp' \
	  --volume '$(realpath $(dir $<)):/tmp/$(notdir $(OUT))' \
	  --workdir '/tmp/$(notdir $(OUT))' \
	  $*:$(VERSION) \
	  ninja -j$(NPROC) $(GLOAS)

$(OUT_DIRS):
	$(SILENT)mkdir -p $@

debug-print-%:
	@printf '%s\n' '$*:' $($*)

