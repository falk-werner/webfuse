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

DOCKER_VERSION = 18.09.6
IMAGE_BUILDARGS += DOCKER_VERSION=$(DOCKER_VERSION)
FETCH_TARGETS += $(FETCHDIR)/docker-$(DOCKER_VERSION)-x86_64.tgz
$(FETCHDIR)/docker-$(DOCKER_VERSION)-x86_64.tgz: URL = https://download.docker.com/linux/static/stable/x86_64/docker-$(DOCKER_VERSION).tgz
$(SKIP_MD5SUM)$(FETCHDIR)/docker-$(DOCKER_VERSION)-x86_64.tgz: MD5 = a6be1e734421d05abfc4d3e28997e271

#######################################################################################################################
# Architecture-specific rule target configuration

DOCKER_TARGETS += $(call target_properties_combine,\
    ,\
    x86_64,\
    alpine@3.9 alpine@3.10,\
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
OUTDIRS += $(OUTDIR)/src/docker-$(DOCKER_VERSION)-x86_64
OUTDIRS += $(OUTDIR)/assets/ca-certificates

EXTRACT_TARGETS += $(patsubst $(FETCHDIR)/%.tar.gz,$(OUTDIR)/src/%,$(FETCH_TARGETS))
EXTRACT_TARGETS += $(patsubst $(DOBUILDDIR)/%,$(OUTDIR)/%,$(wildcard $(DOBUILDDIR)/assets/ca-certificates/*.crt))
EXTRACT_TARGETS += $(OUTDIR)/bin/docker-$(DOCKER_VERSION)-x86_64

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

$(OUTDIR)/assets/ca-certificates/%.crt : $(DOBUILDDIR)/assets/ca-certificates/%.crt | $(OUTDIRS)
	cp $< $@

$(OUTDIR)/bin/docker-$(DOCKER_VERSION)-x86_64 : $(OUTDIR)/src/docker-$(DOCKER_VERSION)-x86_64/docker/docker
	$(SILENT) \
	     $(call echo_if_silent_cmd,cp $< $@) \
	  && cp $< $@ \
	  && chmod +x $@

$(OUTDIR)/src/docker-$(DOCKER_VERSION)-x86_64/docker/docker: $(OUTDIR)/src/docker-$(DOCKER_VERSION)-x86_64/docker

$(OUTDIR)/src/docker-$(DOCKER_VERSION)-x86_64/docker: $(FETCHDIR)/docker-$(DOCKER_VERSION)-x86_64.tgz | $(OUTDIRS)
	$(SILENT) \
	     $(call echo_if_silent_cmd,tar -C $(dir $@) -xf $<) \
	  && tar -I 'gzip -n' -C $(dir $@) -xf $< \
	  && touch $@

$(OUTDIR)/src/%: $(FETCHDIR)/%.tar.gz | $(OUTDIRS)
	$(SILENT) \
	     $(call echo_if_silent_cmd,tar -C $(dir $@) -xf $<) \
	  && tar -I 'gzip -n' -C $(dir $@) -xf $< \
	  && touch $@

$(FETCHDIR):
	$(SILENT)mkdir -p $@

.DELETE_ON_ERROR: $(FETCH_TARGETS)

