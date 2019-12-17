MAKEFLAGS += $(_PARALLELMFLAGS) --no-builtin-rules
MAKEFILE := $(lastword $(MAKEFILE_LIST))
SHELL := /bin/sh

.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Project macros

filter_targets = $(shell echo '$2' | sed -e 's@\s@\n@g' | sed -n$(foreach REGEX,$1, -e 's@$(REGEX)@\0@p'))

regex_march_distro = '$1-$2-.*'

#######################################################################################################################
# Overridable project defaults

PROJECTNAME ?= webfuse
PROJECTDIR ?= $(patsubst %/,%,$(dir $(MAKEFILE)))
SCRIPTDIR ?= $(PROJECTDIR)/build/mkdocker
OUTDIR ?= $(PROJECTDIR)/.build
FETCHDIR ?= $(PROJECTDIR)/.deps
BUILDTYPE ?= Debug

SKIP_FETCH ?= 

SKIP_DEFAULT_BUILDTARGET ?= $(or $(MARCH),$(DISTRO))
$(SKIP_DEFAULT_BUILDTARGET)BUILDTARGET ?= amd64-ubuntu-builder
MARCH ?= '.*'
DISTRO ?= '.*'
FILTER ?= $(call regex_march_distro,$(MARCH),$(DISTRO))

CONTAINER_USER ?= user
CONTAINER_GROUP ?= user

UBUNTU_CODENAME ?= bionic
ALPINE_CODENAME ?= 3.9

SKIP_MD5SUM ?= $(call filter_out_command,md5sum)
SKIP_MD5SUM := $(SKIP_MD5SUM)

#######################################################################################################################
# Project dependencies

DUMB_INIT_VERSION ?= 1.2.2
DOCKER_BUILDARGS += DUMB_INIT_VERSION=$(DUMB_INIT_VERSION)
FETCH_TARGETS += $(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz
$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: URL := https://github.com/Yelp/dumb-init/archive/v${DUMB_INIT_VERSION}.tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: MD5 := 6166084b05772cdcf615a762c6f3b32e

GTEST_VERSION ?= 1.10.0
DOCKER_BUILDARGS += GTEST_VERSION=$(GTEST_VERSION)
FETCH_TARGETS += $(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz
$(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz: URL := https://github.com/google/googletest/archive/release-$(GTEST_VERSION).tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz: MD5 := ecd1fa65e7de707cd5c00bdac56022cd

FUSE_VERSION ?= 3.9.0
DOCKER_BUILDARGS += FUSE_VERSION=$(FUSE_VERSION)
FETCH_TARGETS += $(FETCHDIR)/libfuse-fuse-$(FUSE_VERSION).tar.gz
$(FETCHDIR)/libfuse-fuse-$(FUSE_VERSION).tar.gz: URL := https://github.com/libfuse/libfuse/archive/fuse-$(FUSE_VERSION).tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/libfuse-fuse-$(FUSE_VERSION).tar.gz: MD5 := 31b19277c039ae225927edc29193d34f

WEBSOCKETS_VERSION ?= 3.2.0
DOCKER_BUILDARGS += WEBSOCKETS_VERSION=$(WEBSOCKETS_VERSION)
FETCH_TARGETS += $(FETCHDIR)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz
$(FETCHDIR)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz: URL := https://github.com/warmcat/libwebsockets/archive/v$(WEBSOCKETS_VERSION).tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/libwebsockets-$(WEBSOCKETS_VERSION).tar.gz: MD5 := 1d06f5602604e67e6f50cef9857c6b0c

JANSSON_VERSION ?= 2.12
DOCKER_BUILDARGS += JANSSON_VERSION=$(JANSSON_VERSION)
FETCH_TARGETS += $(FETCHDIR)/jansson-$(JANSSON_VERSION).tar.gz
$(FETCHDIR)/jansson-$(JANSSON_VERSION).tar.gz: URL := https://github.com/akheron/jansson/archive/v$(JANSSON_VERSION).tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/jansson-$(JANSSON_VERSION).tar.gz: MD5 := c4b106528d5ffb521178565de1ba950d

QEMU_VERSION ?= v4.1.0-1
DOCKER_BUILDARGS += QEMU_VERSION_=$(QEMU_VERSION)
FETCH_TARGETS += $(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION)
$(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION): URL := https://github.com/multiarch/qemu-user-static/releases/download/$(QEMU_VERSION)/qemu-arm-static
$(SKIP_MD5SUM)$(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION): MD5 := e508e6e4dd7f3a851207aac245a4653f

#######################################################################################################################
# Architecture-specific rule target configuration

CMAKE_TARGETS += amd64-ubuntu-builder
CMAKE_TARGETS += amd64-alpine-builder
CMAKE_TARGETS += arm32v7-ubuntu-builder
CMAKE_TARGETS += arm32v7-alpine-builder

MEMCHECK_FILTER = $(call regex_march_distro,'$(HOST_MARCH)','.*')

UBUNTU_FILTER = $(call regex_march_distro,'.*','ubuntu')
UBUNTU_TARGETS = $(addprefix $(OUTDIR)/docker/,$(call filter_targets,$(UBUNTU_FILTER),$(TARGETS)))

ALPINE_FILTER = $(call regex_march_distro,'.*','alpine')
ALPINE_TARGETS = $(addprefix $(OUTDIR)/docker/,$(call filter_targets,$(ALPINE_FILTER),$(TARGETS)))

#######################################################################################################################
# Common rule target configuration

CURLFLAGS += -s

DOCKER_RUNFLAGS += --device /dev/fuse
DOCKER_RUNFLAGS += --cap-add SYS_ADMIN
DOCKER_RUNFLAGS += --security-opt apparmor:unconfined

DOCKER_RUNFLAGS += --cap-add SYS_PTRACE
DOCKER_RUNFLAGS += --security-opt seccomp=unconfined

DOCKER_BUILDARGS += CODENAME=$(CODENAME)

OUTDIRS += $(OUTDIR)/src

EXTRACT_TARGETS += $(OUTDIR)/docker/qemu-arm-static-$(QEMU_VERSION)
EXTRACT_TARGETS += $(patsubst $(FETCHDIR)/%.tar.gz,$(OUTDIR)/src/%,$(FETCH_TARGETS))

#######################################################################################################################
# Makefile dependencies

$(SKIP_FETCH)MAKEFILE_DEPS += curl
MAKEFILE_DEPS += gunzip
MAKEFILE_DEPS += tar
MAKEFILE_DEPS += chmod
MAKEFILE_DEPS += touch
MAKEFILE_DEPS += test
MAKEFILE_DEPS += cp
MAKEFILE_DEPS += sed

#######################################################################################################################
# Rules

include $(SCRIPTDIR)/cmake.mk
include $(SCRIPTDIR)/docker.mk
include $(SCRIPTDIR)/standardrules.mk

$(CHECK_TARGETS): GOALS := test

$(UBUNTU_TARGETS): CODENAME := $(UBUNTU_CODENAME)

$(ALPINE_TARGETS): CODENAME := $(ALPINE_CODENAME)

$(FETCH_TARGETS): | $(FETCHDIR)
	$(SILENT)$(call curl,$@,$(URL),$(MD5))

$(OUTDIR)/docker/%.dockerfile : $(PROJECTDIR)/build/%.dockerfile | $(OUTDIRS)
	cp $< $@

$(OUTDIR)/docker/qemu-arm-static-$(QEMU_VERSION) : $(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION) | $(OUTDIRS)
	$(SILENT) \
	     $(call echo_if_silent,cp $< $@) \
	  && cp $< $@ \
	  && chmod +x $@ 

$(OUTDIR)/src/%: $(FETCHDIR)/%.tar.gz | $(OUTDIRS)
	$(SILENT) \
	     $(call echo_if_silent,tar -C $(dir $@) -xf $<) \
	  && tar -C $(dir $@) -xf $< \
	  && touch $@

$(FETCHDIR):
	$(SILENT)mkdir -p $@

.DELETE_ON_ERROR: $(FETCH_TARGETS)

