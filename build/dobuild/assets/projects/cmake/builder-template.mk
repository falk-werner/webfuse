SHELL := /bin/sh
MAKEFILE := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFLAGS += --no-builtin-rules

.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Overridable project defaults

DOBUILD_TOPDIR ?= $(DOBUILDDIR)
DOBUILD_PROJECTDIR ?= $(patsubst %/,%,$(dir $(MAKEFILE)))
DOBUILD_PROJECTVERSIONFILE ?= $(DOBUILDDIR)/VERSION

PROJECTDIR = $(DOBUILD_PROJECTDIR)
DOBUILDDIR ?= $(PROJECTDIR)/../../..

include $(DOBUILDDIR)/defaults.mk

#######################################################################################################################
# Project defaults and macros

DEFAULTTARGET = x86_64-alpine@3.10+builder-template

FETCH_TARGETS =
FETCHDIR = $(BUILDDIR)/.deps

#######################################################################################################################
# Project dependencies

DUMB_INIT_VERSION = 1.2.2
IMAGE_BUILDARGS += DUMB_INIT_VERSION=$(DUMB_INIT_VERSION)
IMAGE_BUILDARGS += DUMB_INIT_MTIME=$(call mtime,$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz)
FETCH_TARGETS += $(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz
$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: URL = https://github.com/Yelp/dumb-init/archive/v$(DUMB_INIT_VERSION).tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: MD5 = 6166084b05772cdcf615a762c6f3b32e

QEMU_VERSION = v3.1.0-2
IMAGE_BUILDARGS += QEMU_VERSION_=$(QEMU_VERSION)
FETCH_TARGETS += $(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION)
$(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION): URL = https://github.com/multiarch/qemu-user-static/releases/download/$(QEMU_VERSION)/qemu-arm-static
$(SKIP_MD5SUM)$(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION): MD5 = 8ebd24e63fdfa07c557d45373bd831b1

#######################################################################################################################
# Architecture-specific rule target configuration

DOCKER_TARGETS += $(call target_properties_combine,\
    ,\
    x86_64 arm32v7,\
    ubuntu@xenial ubuntu@bionic alpine@3.9 alpine@3.10,\
    ,\
    ,\
    builder-template,\
    ,\
    \
  )

#######################################################################################################################
# Common rule target configuration

CURLFLAGS += -s

OUTDIRS += $(OUTDIR)/src
OUTDIRS += $(OUTDIR)/bin

EXTRACT_TARGETS += $(OUTDIR)/bin/qemu-arm-static-$(QEMU_VERSION)
EXTRACT_TARGETS += $(patsubst $(FETCHDIR)/%.tar.gz,$(OUTDIR)/src/%,$(FETCH_TARGETS))

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += gzip
MAKEFILE_DEPS += tar
MAKEFILE_DEPS += chmod
MAKEFILE_DEPS += touch
MAKEFILE_DEPS += cp
MAKEFILE_DEPS += mkdir

#######################################################################################################################
# Rules

include $(DOBUILDDIR)/docker.mk
include $(DOBUILDDIR)/standardrules.mk

$(FETCH_TARGETS): | $(FETCHDIR)
	$(SILENT)$(call curl,$@,$(URL),$(MD5))

$(OUTDIR)/bin/qemu-arm-static-$(QEMU_VERSION) : $(FETCHDIR)/qemu-arm-static-$(QEMU_VERSION) | $(OUTDIRS)
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

