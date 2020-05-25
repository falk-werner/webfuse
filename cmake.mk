#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

ifndef cmake_include_guard
cmake_include_guard := 1

current_makefile := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

ifndef defaults_include_guard
  include $(patsubst %/,%,$(dir $(current_makefile)))/defaults.mk
endif

#######################################################################################################################
# Overridable CMake defaults

DOBUILD_CMAKE_GENERATOR ?= make

#######################################################################################################################
# Overridable cmake macros, hooks to customize target default values

# hook called to retrieve the target cmake extension directory,
# may return an empty value
# $(call dobuild_cmake_extdir,target-name)
dobuild_cmake_extdir ?= $(call dobuild_generic_extdir,$1)

# hook called to retrieve the target cmake adapter name
# $(call dobuild_cmake_adapter,target-name)
dobuild_cmake_adapter ?= cmake

# hook called to retrieve the target cmake prerequisites,
# may return an empty list
# $(call dobuild_cmake_prerequisites,target-name)
dobuild_cmake_prerequisites ?= $(call dobuild_generic_prerequisites,$1)

# hook called to retrieve the target cmake order-only prerequisites,
# may return an empty list
# $(call dobuild_cmake_orderonly_prerequisites,target-name)
dobuild_cmake_orderonly_prerequisites ?= $(call dobuild_generic_orderonly_prerequisites,$1)

# hook called to retrieve the target cmake prepare step,
# may return an empty value
# $(call dobuild_cmake_prepare,target-name)
dobuild_cmake_prepare ?= $(call dobuild_generic_prepare,$1)

# hook called to retrieve the target cmake assemble step
# $(call dobuild_cmake_assemble,target-name)
dobuild_cmake_assemble ?= $(call dobuild_generic_assemble,$1)

# hook called to retrieve the target cmake save artifacts step,
# may return an empty value
# $(call dobuild_cmake_saveartifacts,target-name)
dobuild_cmake_saveartifacts ?= $(call dobuild_generic_saveartifacts,$1)

# hook called to retrieve the target cmake lint step,
# may return an empty value
# $(call dobuild_cmake_lint,target-name)
dobuild_cmake_lint ?= $(call dobuild_generic_lint,$1)

# hook called to retrieve the target cmake check step,
# may return an empty value
# $(call dobuild_cmake_check,target-name)
dobuild_cmake_check ?= $(call dobuild_generic_check,$1)

# hook called to retrieve the target cmake memcheck step,
# may return an empty value
# $(call dobuild_cmake_memcheck,target-name)
dobuild_cmake_memcheck ?= $(call dobuild_generic_memcheck,$1)

# hook called to retrieve the target cmake package step,
# may return an empty value
# $(call dobuild_cmake_package,target-name)
dobuild_cmake_package ?= $(call dobuild_generic_package,$1)

# hook called to retrieve the target cmake install step,
# may return an empty value
# $(call dobuild_cmake_install,target-name)
dobuild_cmake_install ?= $(call dobuild_generic_install,$1)

# hook called to retrieve the target cmake delegate step
# $(call dobuild_cmake_delegate,target-name)
dobuild_cmake_delegate ?= $(call dobuild_generic_delegate,$1)

# hook called to retrieve the target cmake build testing option,
# may return an empty value in which case BUILD_TESTING is used
# $(call dobuild_cmake_buildtesting,target-name)
dobuild_cmake_buildtesting ?= $(call dobuild_generic_buildtesting,$1)

# hook called to retrieve the target cmake generator name,
# may return an empty value in which case CMAKE_GENERATOR is used
# $(call dobuild_cmake_generator,target-name)
dobuild_cmake_generator ?= 

#######################################################################################################################
# CMake macros

# retrieves the target cmake default prerequisites
# $(call cmake_default_prerequisites,target-name)
cmake_default_prerequisites = \
  $(OUTDIR)/$1/DoBuildFiles/cmake.properties \
  $(wildcard $(PROJECTDIR)/CMakeLists.txt)

# retrieves the target cmake generator
# $(call cmake_generator,target-name)
cmake_generator = $(cmake_generator_$(cache.$1.cmake_generator))

# retrieves the target cmake generator command
# $(call cmake_generator_cmd,target-name)
cmake_generator_cmd = $(cache.$1.cmake_generator)

# creates a cmake properties generation cmd
# $(call cmake_props_cmd,target-name,input-file)
cmake_props_cmd = sed \
  -e 's!%CMAKE_GENERATOR_CMD%!$(call escape,$(call cmake_generator_cmd,$1),!)!g' \
  -e 's!%CMAKE_GENERATOR%!$(call escape,$(call cmake_generator,$1),!)!g' \
  $2

# creates a cmake properties generation rule
# $(call cmake_props_rule,target-name)
cmake_props_rule = \
  $$(OUTDIR)/$1/DoBuildFiles/cmake.properties: $$(DOBUILDDIR)/assets/templates/cmake.properties.template $$(MAKEFILE_LIST); \
    $$(SILENT)mkdir -p $$(dir $$@); $$(call cmake_props_cmd,$1,$$<) > $$@

#######################################################################################################################
# CMake rule target configuration

CMAKE_GENERATOR = $(call memorize,CMAKE_GENERATOR,$(DOBUILD_CMAKE_GENERATOR))

docker_runflags += --env NINJA_STATUS

cmake_generator_ninja = Ninja
cmake_generator_make = Unix Makefiles

cmake_rule_targets = $(addsuffix /cmakerules.mk,$(OUTDIR))

GENERIC_TARGETS += $(CMAKE_TARGETS)
RULE_TARGETS += $(cmake_rule_targets)

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += touch
MAKEFILE_DEPS += echo
MAKEFILE_DEPS += sed

#######################################################################################################################
# CMake rules

$(cmake_rule_targets):
	$(SILENT) \
	{ \
	  echo '$(\#) generated file - do not edit!!!'; \
	  echo; \
	  ID='$(call id,$@)'; \
	  echo "ifndef $${ID}_include_guard"; \
	  echo "$${ID}_include_guard := 1"; \
	  echo; \
	$(foreach target,$(CMAKE_TARGETS),\
	  echo '$(\#)$(\#) BEGIN of cmake $(target) configuration'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) defaults'; \
	  echo '$(target) ?= $$(call memorize,$(target),$(call target_properties_parse,$(target)))'; \
	  echo '$(target).generic_adapter = $$(call $$1.cmake_adapter,$$1)'; \
	  echo '$(target).generic_extdir = $$(call $$1.cmake_extdir,$$1)'; \
	  echo '$(target).generic_prerequisites = $$(call $$1.cmake_prerequisites,$$1) $$(call cmake_default_prerequisites,$$1)'; \
	  echo '$(target).generic_orderonly_prerequisites ?= $$(call $$1.cmake_orderonly_prerequisites,$$1)'; \
	  echo '$(target).generic_prepare = $$(call $$1.cmake_prepare,$$1)'; \
	  echo '$(target).generic_assemble = $$(call $$1.cmake_assemble,$$1)'; \
	  echo '$(target).generic_saveartifacts = $$(call $$1.cmake_saveartifacts,$$1)'; \
	  echo '$(target).generic_lint = $$(call $$1.cmake_lint,$$1)'; \
	  echo '$(target).generic_check = $$(call $$1.cmake_check,$$1)'; \
	  echo '$(target).generic_memcheck = $$(call $$1.cmake_memcheck,$$1)'; \
	  echo '$(target).generic_package = $$(call $$1.cmake_package,$$1)'; \
	  echo '$(target).generic_install = $$(call $$1.cmake_install,$$1)'; \
	  echo '$(target).generic_delegate = $$(call $$1.cmake_delegate,$$1)'; \
	  echo '$(target).generic_buildtesting = $$(call $$1.cmake_buildtesting,$$1)'; \
	  echo '$(target).cmake_adapter ?= $$(call dobuild_cmake_adapter,$$1)'; \
	  echo '$(target).cmake_extdir ?= $$(call dobuild_cmake_extdir,$$1)'; \
	  echo '$(target).cmake_prerequisites ?= $$(call dobuild_cmake_prerequisites,$$1)'; \
	  echo '$(target).cmake_orderonly_prerequisites ?= $$(call dobuild_cmake_orderonly_prerequisites,$$1)'; \
	  echo '$(target).cmake_prepare ?= $$(call dobuild_cmake_prepare,$$1)'; \
	  echo '$(target).cmake_assemble ?= $$(call dobuild_cmake_assemble,$$1)'; \
	  echo '$(target).cmake_saveartifacts ?= $$(call dobuild_cmake_saveartifacts,$$1)'; \
	  echo '$(target).cmake_lint ?= $$(call dobuild_cmake_lint,$$1)'; \
	  echo '$(target).cmake_check ?= $$(call dobuild_cmake_check,$$1)'; \
	  echo '$(target).cmake_memcheck ?= $$(call dobuild_cmake_memcheck,$$1)'; \
	  echo '$(target).cmake_package ?= $$(call dobuild_cmake_package,$$1)'; \
	  echo '$(target).cmake_install ?= $$(call dobuild_cmake_install,$$1)'; \
	  echo '$(target).cmake_delegate ?= $$(call dobuild_cmake_delegate,$$1)'; \
	  echo '$(target).cmake_buildtesting ?= $$(call dobuild_cmake_buildtesting,$$1)'; \
	  echo '$(target).cmake_generator ?= $$(call dobuild_cmake_generator,$$1)'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) cached values'; \
	  echo 'cache.$(target).cmake_generator = $$(call memorize,cache.$(target).cmake_generator,$$(or $$(call $(target).cmake_generator,$(target)),$$(CMAKE_GENERATOR)))'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) rules'; \
	  echo '$(call cmake_props_rule,$(target))'; \
	  echo; \
	  echo '$(\#)$(\#) END of cmake $(target) configuration'; \
	  echo; \
	) \
	  echo 'endif'; \
	  echo; \
	} > $@

endif

