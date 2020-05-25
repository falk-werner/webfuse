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

PROJECTDIR = $(DOBUILD_PROJECTDIR)
DOBUILDDIR ?= $(PROJECTDIR)/../../..

include $(DOBUILDDIR)/defaults.mk

#######################################################################################################################
# Project defaults and macros

DEFAULTTARGET = x86_64-ubuntu@bionic+builder

FETCHDIR = $(BUILDDIR)/.deps

#######################################################################################################################
# Project dependencies

DUMB_INIT_VERSION = 1.2.2
IMAGE_BUILDARGS += DUMB_INIT_VERSION=$(DUMB_INIT_VERSION)
IMAGE_BUILDARGS += DUMB_INIT_MTIME=$(call mtime,$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz)
FETCH_TARGETS += $(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz
$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: URL = https://github.com/Yelp/dumb-init/archive/v$(DUMB_INIT_VERSION).tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/dumb-init-$(DUMB_INIT_VERSION).tar.gz: MD5 = 6166084b05772cdcf615a762c6f3b32e

GTEST_VERSION = 1.8.0
IMAGE_BUILDARGS += GTEST_VERSION=$(GTEST_VERSION)
IMAGE_BUILDARGS += GTEST_MTIME=$(call mtime,$(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz)
FETCH_TARGETS += $(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz
$(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz: URL := https://github.com/google/googletest/archive/release-$(GTEST_VERSION).tar.gz
$(SKIP_MD5SUM)$(FETCHDIR)/googletest-release-$(GTEST_VERSION).tar.gz: MD5 := 16877098823401d1bf2ed7891d7dce36

#######################################################################################################################
# Architecture-specific rule target configuration

CMAKE_TARGETS += x86_64-ubuntu@bionic+builder
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
MAKEFILE_DEPS += cp
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

$(OUTDIR)/docker/%.dockerfile : $(PROJECTDIR)/%.dockerfile | $(OUTDIRS)
	cp $< $@

$(FETCH_TARGETS): | $(FETCHDIR)
	$(SILENT)$(call curl,$@,$(URL),$(MD5))

$(FETCHDIR):
	$(SILENT)mkdir -p $@

.DELETE_ON_ERROR: $(FETCH_TARGETS)

