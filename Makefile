.PHONY: default
default: all

# Overridable defaults

$(VERBOSE)SILENT ?= @
PROJECT_ROOT ?= .
OUT ?= $(PROJECT_ROOT)/.build
VERSION ?= $(shell cat $(PROJECT_ROOT)/VERSION)
PARALLELMFLAGS ?= -j$(shell nproc)
UID ?= $(shell id -u)
DOCKER ?= docker
DOCKER_RUNUSER ?= $(UID)
DOCKER_RUNGROUP ?= $(shell id -g)

UBUNTU_CODENAME ?= bionic

QEMU_VERSION ?= v3.1.0-2
GTEST_VERSION ?= 1.8.1
FUSE_VERSION ?= 3.1.1
WEBSOCKETS_VERSION ?= 3.1.0
JANSSON_VERSION ?= 2.12

# Target configuration

MARCH_AMD64 := $(filter-out amd64,$(MARCH))
MARCH_ARM32V7 := $(filter-out arm32v7,$(MARCH))

MAKEFLAGS += $(PARALLELMFLAGS) --no-builtin-rules

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
DOCKER_BUILDARGS += PARALLELMFLAGS=$(PARALLELMFLAGS)
DOCKER_BUILDARGS += USERID=$(UID)
DOCKER_BUILDARGS += SRC=$(realpath $(PROJECT_ROOT))
DOCKER_BUILDARGS += OUT=$(realpath $(OUT))

DOCKER_BUILDFLAGS += $(addprefix --build-arg ,$(DOCKER_BUILDARGS))

OUT_DIRS += $(OUT)
OUT_DIRS += $(OUT)/docker
OUT_DIRS += $(OUT)/src
OUT_DIRS += $(addprefix $(OUT)/,$(TARGETS))

ALL_TARGETS += $(addprefix compile-,$(TARGETS)) 
CHECK_TARGETS += $(addprefix check-,$(TARGETS))
CONFIGURE_TARGETS += $(addsuffix /CMakeCache.txt,$(addprefix $(OUT)/,$(TARGETS)))

FETCH_TARGETS += $(OUT)/docker/qemu-arm-static-$(QEMU_VERSION)
FETCH_TARGETS += $(OUT)/googletest-release-$(GTEST_VERSION).tar.gz
FETCH_TARGETS += $(OUT)/libfuse-fuse-$(FUSE_VERSION).tar.gz
FETCH_TARGETS += $(OUT)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz
FETCH_TARGETS += $(OUT)/jansson-$(JANSSON_VERSION).tar.gz

EXTRACT_TARGETS += $(patsubst $(OUT)/%.tar.gz,$(OUT)/src/%,$(FETCH_TARGETS))

DOCKER_RUNGROUP := $(DOCKER_RUNGROUP)
VERSION := $(VERSION)
PROJECT_ROOT := $(PROJECT_ROOT)
OUT := $(OUT)

# Rules

$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION): URL := https://github.com/multiarch/qemu-user-static/releases/download/$(QEMU_VERSION)/qemu-arm-static
$(OUT)/googletest-release-$(GTEST_VERSION).tar.gz: URL := https://github.com/google/googletest/archive/release-$(GTEST_VERSION).tar.gz
$(OUT)/libfuse-fuse-$(FUSE_VERSION).tar.gz: URL := https://github.com/libfuse/libfuse/archive/fuse-$(FUSE_VERSION).tar.gz
$(OUT)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz: URL := https://github.com/warmcat/libwebsockets/archive/v$(WEBSOCKETS_VERSION).tar.gz
$(OUT)/jansson-$(JANSSON_VERSION).tar.gz: URL := https://github.com/akheron/jansson/archive/v$(JANSSON_VERSION).tar.gz

.PHONY: all
all: $(ALL_TARGETS)

.PHONY: check
check: $(CHECK_TARGETS)

.PHONY: clean
clean: $(CLEAN_TARGETS)
	$(SILENT)-rm -rf $(OUT_DIRS)

.PHONY: get-deps
get-deps: $(EXTRACT_TARGETS)

.PHONY: debug-print-%
debug-print-%:
	@printf '%s\n' '$*:' $($*)

.PHONY: check-%
check-%: compile-%;

.PHONY: compile-%
compile-%: $(OUT)/%/CMakeCache.txt
	$(SILENT)$(DOCKER) run $(DOCKER_RUNFLAGS) \
	  --volume '$(realpath $(PROJECT_ROOT)):/tmp' \
	  --volume '$(realpath $(dir $<)):/tmp/$(notdir $(OUT))' \
	  --workdir '/tmp/$(notdir $(OUT))' \
	  $*:$(VERSION) \
	  ninja $(PARALLELMFLAGS) $(GLOAS)

$(CHECK_TARGETS): GLOAS := test
$(CHECK_TARGETS): DOCKER_RUNUSER := user

$(OUT)/docker/qemu-arm-static-$(QEMU_VERSION):
	$(SILENT) \
	     curl -fsSL -o $@ $(URL) \
	  && chmod +x $@ 

$(OUT)/docker/wsfs-builder-arm32v7-ubuntu: $(OUT)/docker/qemu-arm-static-$(QEMU_VERSION)

$(OUT)/% : $(PROJECT_ROOT)/% | $(OUT_DIRS)
	cp $< $@

%-ubuntu: CODENAME := $(UBUNTU_CODENAME)

$(OUT)/docker/%: $(OUT)/docker/%.dockerfile $(EXTRACT_TARGETS) $(PROJECT_ROOT)/Makefile | $(OUT_DIRS)
	$(SILENT)$(DOCKER) build --rm $(DOCKER_BUILDFLAGS) --iidfile $@ --file $< --tag $*:$(VERSION) $(OUT)

$(OUT)/%.tar.gz: | $(OUT_DIRS)
	curl -fsSL -o $@ $(URL)

$(OUT)/src/%: $(OUT)/%.tar.gz | $(OUT_DIRS)
	$(SILENT) \
	     tar -C $(dir $@) -xf $< \
	  && touch $@

$(OUT)/%/CMakeCache.txt: $(PROJECT_ROOT)/CMakeLists.txt $(OUT)/docker/% | $(OUT_DIRS)
	$(SILENT)$(DOCKER) run $(DOCKER_RUNFLAGS) \
	  --volume '$(realpath $(PROJECT_ROOT)):/tmp' \
	  --volume '$(realpath $(dir $@)):/tmp/$(notdir $(OUT))' \
	  --workdir '/tmp/$(notdir $(OUT))' \
	  $*:$(VERSION) \
	  cmake -GNinja $(CMAKEFLAGS) .. && touch $@

$(OUT_DIRS):
	$(SILENT)mkdir -p $@

.PRECIOUS: $(OUT)/docker/% $(OUT)/%/CMakeCache.txt
.DELETE_ON_ERROR: $(OUT)/%/CMakeCache.txt

