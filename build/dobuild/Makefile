#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

MAKEFLAGS += --no-builtin-rules
MAKEFILE := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
SHELL := /bin/sh

.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Overridable defaults

DOBUILD_PROJECTDIR ?= $(patsubst %/,%,$(dir $(MAKEFILE)))

FETCHDIR = $(DOBUILD_PROJECTDIR)/.deps
CMAKE_BUILDER_OPTS += -f '$(DOBUILD_PROJECTDIR)/assets/projects/cmake/builder-template.mk'
DOBUILD_BUILDER_OPTS += -f '$(DOBUILD_PROJECTDIR)/assets/projects/dobuild/builder-template.mk'
GRADLE_BUILDER_OPTS += -f '$(DOBUILD_PROJECTDIR)/assets/projects/gradle/builder-template.mk'

#######################################################################################################################
# Standard rule target configuration

BUILD_TARGETS += cmake-builder-template-all
BUILD_TARGETS += dobuild-builder-template-all
BUILD_TARGETS += gradle-builder-template-all

CLEAN_TARGETS += cmake-builder-template-clean
CLEAN_TARGETS += dobuild-builder-template-clean
CLEAN_TARGETS += gradle-builder-template-clean

DISTCLEAN_TARGETS += cmake-builder-template-distclean
DISTCLEAN_TARGETS += dobuild-builder-template-distclean
DISTCLEAN_TARGETS += gradle-builder-template-distclean

DIST_TARGETS += cmake-builder-template-dist
DIST_TARGETS += dobuild-builder-template-dist
DIST_TARGETS += gradle-builder-template-dist

CHECK_TARGETS += 

MAKEOVERRIDES := FETCHDIR=$(FETCHDIR) $(MAKEOVERRIDES)

#######################################################################################################################
# Shell exports

#######################################################################################################################
# Standard rule targets

.PHONY: all
all: $(BUILD_TARGETS);

.PHONY: check
check: $(CHECK_TARGETS)
	$(DOBUILD_PROJECTDIR)/run_tests

.PHONY: run
run:
	-$(DOBUILD_PROJECTDIR)/run_tests bash

.PHONY: clean
clean: $(CLEAN_TARGETS);

.PHONY: dist
dist: $(DIST_TARGETS);

.PHONY: distclean
distclean: $(DISTCLEAN_TARGETS);

cmake-builder-template-%:
	$(MAKE) $(MFLAGS) $(CMAKE_BUILDER_OPTS) $* $(MAKEOVERRIDES)

dobuild-builder-template-%:
	$(MAKE) $(MFLAGS) $(DOBUILD_BUILDER_OPTS) $* $(MAKEOVERRIDES)

gradle-builder-template-%:
	$(MAKE) $(MFLAGS) $(GRADLE_BUILDER_OPTS) $* $(MAKEOVERRIDES)
