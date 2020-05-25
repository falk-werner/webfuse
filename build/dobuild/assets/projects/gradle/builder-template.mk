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
DOBUILD_DOCKERFILE ?= $(PROJECTDIR)/%MARCH%-%DISTRIB_ID%.dockerfile

PROJECTDIR = $(DOBUILD_PROJECTDIR)
DOBUILDDIR ?= $(PROJECTDIR)/../../..

include $(DOBUILDDIR)/defaults.mk

#######################################################################################################################
# Project defaults and macros

DEFAULTTARGET = x86_64-graalvm~-ce@20.0.0-linux-java11+6.3

FETCH_TARGETS =
FETCHDIR = $(BUILDDIR)/.deps

dobuild_image_prerequisites = %OUTDIR%/bin/gradle-%ID%

#######################################################################################################################
# Project dependencies

FETCH_TARGETS += $(FETCHDIR)/gradle-6.3.zip
$(FETCHDIR)/gradle-6.3.zip: URL = https://services.gradle.org/distributions/gradle-6.3-bin.zip
$(SKIP_MD5SUM)$(FETCHDIR)/gradle-6.3.zip: MD5 = 737c68904f35e6480fa013b1eb3c9c50

FETCH_TARGETS += $(FETCHDIR)/gradle-5.6.zip
$(FETCHDIR)/gradle-5.6.zip: URL = https://services.gradle.org/distributions/gradle-5.6-bin.zip
$(SKIP_MD5SUM)$(FETCHDIR)/gradle-5.6.zip: MD5 = 2dde6806b36fe0832a7438752be6ed36

#######################################################################################################################
# Architecture-specific rule target configuration

DOCKER_TARGETS += $(call target_properties_combine,\
    ,\
    x86_64,\
    graalvm~-ce,\
    linux,\
    java11 java8,\
    6.3 5.6,\
    20.0.0,\
    \
  )

#######################################################################################################################
# Common rule target configuration

CURLFLAGS += -s

OUTDIRS += $(OUTDIR)/bin

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += unzip

#######################################################################################################################
# Rules

include $(DOBUILDDIR)/docker.mk
include $(DOBUILDDIR)/standardrules.mk

$(FETCH_TARGETS): | $(FETCHDIR)
	$(SILENT)$(call curl,$@,$(URL),$(MD5))

$(OUTDIR)/bin/%: $(FETCHDIR)/%.zip | $(OUTDIRS)
	$(SILENT) \
	     $(call echo_if_silent_cmd,unzip -d $(dir $@) $<) \
	  && unzip -o -d $(dir $@) $< \
	  && touch $@

$(FETCHDIR):
	$(SILENT)mkdir -p $@

.DELETE_ON_ERROR: $(FETCH_TARGETS)
