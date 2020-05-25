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
DOBUILD_DOCKERFILE ?= $(PROJECTDIR)/%ID%.dockerfile

PROJECTDIR = $(DOBUILD_PROJECTDIR)
DOBUILDDIR ?= $(PROJECTDIR)/../..

include $(DOBUILDDIR)/defaults.mk

#######################################################################################################################
# Project defaults and macros

DEFAULTTARGET = x86_64-ubuntu@bionic+builder@debug

FETCHDIR = $(BUILDDIR)/.deps

#######################################################################################################################
# Project dependencies

GTEST_VERSION = 1.8.1
IMAGE_BUILDARGS += GTEST_VERSION=$(GTEST_VERSION)
IMAGE_BUILDARGS += GTEST_MTIME=$(call mtime,$(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz)
FETCH_TARGETS += $(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz
$(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz: URL := https://github.com/google/googletest/archive/release-$(GTEST_VERSION).tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz: MD5 := 2e6fbeb6a91310a16efe181886c59596

#######################################################################################################################
# Architecture-specific rule target configuration

CMAKE_TARGETS += $(call target_properties_combine,\
    ,\
    x86_64 arm32v7,\
    ubuntu@bionic alpine@3.10,\
    ,\
    ,\
    builder,\
    ,\
    debug release \
  )
DOCKER_TARGETS += $(CMAKE_TARGETS)

#######################################################################################################################
# Common rule target configuration

CURLFLAGS += -s

DOCKER_RUNFLAGS += --cap-add SYS_PTRACE
DOCKER_RUNFLAGS += --security-opt seccomp=unconfined

OUTDIRS += $(OUTDIR)/src

EXTRACT_TARGETS += $(patsubst $(FETCHDIR)/%.tar.gz,$(OUTDIR)/src/%,$(FETCH_TARGETS))

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += gzip
MAKEFILE_DEPS += tar
MAKEFILE_DEPS += touch
MAKEFILE_DEPS += mkdir

#######################################################################################################################
# Rules

include $(DOBUILDDIR)/cmake.mk
include $(DOBUILDDIR)/docker.mk
include $(DOBUILDDIR)/standardrules.mk

$(OUTDIR)/src/%: $(FETCHDIR)/%.tar.gz | $(OUTDIRS)
	$(SILENT) \
	     $(call echo_if_silent_cmd,tar -C $(dir $@) -xf $<) \
	  && tar -I 'gzip -n' -C $(dir $@) -xf $< \
	  && touch $@

$(FETCH_TARGETS): | $(FETCHDIR)
	$(SILENT)$(call curl,$@,$(URL),$(MD5))

$(FETCHDIR):
	$(SILENT)mkdir -p $@

.DELETE_ON_ERROR: $(FETCH_TARGETS)

