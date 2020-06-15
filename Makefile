SHELL := /bin/sh
MAKEFILE := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFLAGS += --no-builtin-rules

.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Overridable project defaults

DOBUILD_DOCKERFILE ?= $(PROJECTDIR)/build/%MARCH%-%DISTRIB_ID%-%ID%.dockerfile
DOBUILD_EXTDIR ?= $(PROJECTDIR)/build/dobuild-extensions
DOBUILD_PROJECTDIR ?= $(patsubst %/,%,$(dir $(MAKEFILE)))
DOBUILD_GENERIC_ADAPTER ?= meson

DOBUILDDIR ?= $(PROJECTDIR)/build/dobuild
PROJECTDIR = $(DOBUILD_PROJECTDIR)

include $(DOBUILDDIR)/defaults.mk

#######################################################################################################################
# Project defaults and macros

DEFAULTTARGET = x86_64-ubuntu@bionic+builder@debug

FETCHDIR = $(BUILDDIR)/.deps

dobuild_image_buildargs = \
  $(addprefix MARCH=,%MARCH%) \
  $(addprefix DISTRIB_VERSION=,%DISTRIB_VERSION%) \
  $(addprefix USERID=,$(USERID))

#######################################################################################################################
# Project dependencies

DUMB_INIT_VERSION ?= 1.2.2
IMAGE_BUILDARGS += DUMB_INIT_VERSION=$(DUMB_INIT_VERSION)
FETCH_TARGETS += $(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz
$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: URL := https://github.com/Yelp/dumb-init/archive/v${DUMB_INIT_VERSION}.tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: MD5 := 6166084b05772cdcf615a762c6f3b32e

FUSE_VERSION ?= 3.9.2
IMAGE_BUILDARGS += FUSE_VERSION=$(FUSE_VERSION)
FETCH_TARGETS += $(FETCHDIR)/libfuse-fuse-$(FUSE_VERSION).tar.gz
$(FETCHDIR)/libfuse-fuse-$(FUSE_VERSION).tar.gz: URL := https://github.com/libfuse/libfuse/archive/fuse-$(FUSE_VERSION).tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/libfuse-fuse-$(FUSE_VERSION).tar.gz: MD5 := 5a829d2fcaa19580adc56633721d323c

QEMU_VERSION ?= v4.1.0-1
IMAGE_BUILDARGS += QEMU_VERSION_=$(QEMU_VERSION)
FETCH_TARGETS += $(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION)
$(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION): URL := https://github.com/multiarch/qemu-user-static/releases/download/$(QEMU_VERSION)/qemu-arm-static
$(SKIP_MD5SUM)$(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION): MD5 := e508e6e4dd7f3a851207aac245a4653f

#######################################################################################################################
# Architecture-specific rule target configuration

GENERIC_TARGETS += $(call target_properties_combine,\
    ,\
    x86_64 arm32v7,\
    ubuntu@bionic alpine@3.9,\
    ,\
    ,\
    builder,\
    ,\
    debug release coverage minsize \
  )
DOCKER_TARGETS += $(GENERIC_TARGETS)

#######################################################################################################################
# Common rule target configuration

CURLFLAGS += -s

DOCKER_RUNFLAGS += --device /dev/fuse
DOCKER_RUNFLAGS += --cap-add SYS_ADMIN
DOCKER_RUNFLAGS += --security-opt apparmor:unconfined

DOCKER_RUNFLAGS += --cap-add SYS_PTRACE
DOCKER_RUNFLAGS += --security-opt seccomp=unconfined

OUTDIRS += $(OUTDIR)/src
OUTDIRS += $(OUTDIR)/docker

EXTRACT_TARGETS += $(OUTDIR)/docker/qemu-arm-static-$(QEMU_VERSION)
EXTRACT_TARGETS += $(patsubst $(FETCHDIR)/%.tar.gz,$(OUTDIR)/src/%,$(FETCH_TARGETS))

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += gzip
MAKEFILE_DEPS += tar
MAKEFILE_DEPS += chmod
MAKEFILE_DEPS += touch
MAKEFILE_DEPS += cp

#######################################################################################################################
# Rules

include $(DOBUILDDIR)/docker.mk
include $(DOBUILDDIR)/standardrules.mk

$(FETCH_TARGETS): | $(FETCHDIR)
	$(SILENT)$(call curl,$@,$(URL),$(MD5))

$(OUTDIR)/docker/qemu-arm-static-$(QEMU_VERSION) : $(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION) | $(OUTDIRS)
	$(SILENT) \
	     $(call echo_if_silent_cmd,cp $< $@) \
	  && cp $< $@ \
	  && chmod +x $@ 

$(OUTDIR)/src/%: $(FETCHDIR)/%.tar.gz | $(OUTDIRS)
	$(SILENT) \
	     $(call echo_if_silent_cmd,tar -C $(dir $@) -xf $<) \
	  && tar -I 'gzip -n' -C $(dir $@) -xf $< \
	  && touch $@

$(FETCHDIR):
	$(SILENT)mkdir -p $@

.DELETE_ON_ERROR: $(FETCH_TARGETS)

