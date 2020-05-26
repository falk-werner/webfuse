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

DEFAULTTARGET = x86_64-gradle@6.3-linux-java11+builder

#######################################################################################################################
# Project dependencies

#######################################################################################################################
# Architecture-specific rule target configuration

GRADLE_TARGETS += $(call target_properties_combine,\
    ,\
    x86_64,\
    gradle,\
    linux,\
    java11,\
    builder,\
    6.3,\
    \
  )
DOCKER_TARGETS += $(GRADLE_TARGETS)

#######################################################################################################################
# Common rule target configuration

#######################################################################################################################
# Makefile dependencies

#######################################################################################################################
# Rules

include $(DOBUILDDIR)/gradle.mk
include $(DOBUILDDIR)/docker.mk
include $(DOBUILDDIR)/standardrules.mk

