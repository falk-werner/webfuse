#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

ifndef gradle_include_guard
gradle_include_guard := 1

current_makefile := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

ifndef defaults_include_guard
  include $(patsubst %/,%,$(dir $(current_makefile)))/defaults.mk
endif

#######################################################################################################################
# Overridable Gradle defaults

#######################################################################################################################
# Overridable gradle macros, hooks to customize target default values

# hook called to retrieve the target gradle extension directory,
# may return an empty value
# $(call dobuild_gradle_extdir,target-name)
dobuild_gradle_extdir ?= $(call dobuild_generic_extdir,$1)

# hook called to retrieve the target gradle adapter name
# $(call dobuild_gradle_adapter,target-name)
dobuild_gradle_adapter ?= gradle

# hook called to retrieve the target gradle prerequisites,
# may return an empty list
# $(call dobuild_gradle_prerequisites,target-name)
dobuild_gradle_prerequisites ?= $(call dobuild_generic_prerequisites,$1)

# hook called to retrieve the target gradle order-only prerequisites,
# may return an empty list
# $(call dobuild_gradle_orderonly_prerequisites,target-name)
dobuild_gradle_orderonly_prerequisites ?= $(call dobuild_generic_orderonly_prerequisites,$1)

# hook called to retrieve the target gradle prepare step,
# may return an empty value
# $(call dobuild_gradle_prepare,target-name)
dobuild_gradle_prepare ?= $(call dobuild_generic_prepare,$1)

# hook called to retrieve the target gradle assemble step
# $(call dobuild_gradle_assemble,target-name)
dobuild_gradle_assemble ?= $(call dobuild_generic_assemble,$1)

# hook called to retrieve the target gradle save artifacts step,
# may return an empty value
# $(call dobuild_gradle_saveartifacts,target-name)
dobuild_gradle_saveartifacts ?= $(call dobuild_generic_saveartifacts,$1)

# hook called to retrieve the target gradle lint step,
# may return an empty value
# $(call dobuild_gradle_lint,target-name)
dobuild_gradle_lint ?= $(call dobuild_generic_lint,$1)

# hook called to retrieve the target gradle check step,
# may return an empty value
# $(call dobuild_gradle_check,target-name)
dobuild_gradle_check ?= $(call dobuild_generic_check,$1)

# hook called to retrieve the target gradle memcheck step,
# may return an empty value
# $(call dobuild_gradle_memcheck,target-name)
dobuild_gradle_memcheck ?= $(call dobuild_generic_memcheck,$1)

# hook called to retrieve the target gradle package step,
# may return an empty value
# $(call dobuild_gradle_package,target-name)
dobuild_gradle_package ?= $(call dobuild_generic_package,$1)

# hook called to retrieve the target gradle install step,
# may return an empty value
# $(call dobuild_gradle_install,target-name)
dobuild_gradle_install ?= $(call dobuild_generic_install,$1)

# hook called to retrieve the target gradle delegate step
# $(call dobuild_gradle_delegate,target-name)
dobuild_gradle_delegate ?= $(call dobuild_generic_delegate,$1)

# hook called to retrieve the target gradle build testing option,
# may return an empty value in which case BUILD_TESTING is used
# $(call dobuild_gradle_buildtesting,target-name)
dobuild_gradle_buildtesting ?= $(call dobuild_generic_buildtesting,$1)

#######################################################################################################################
# Gradle macros

# retrieves the target generic default prerequisites
# $(call gradle_default_prerequisites,target-name)
gradle_default_prerequisites = \
  $(wildcard $(PROJECTDIR)/*.gradle)

#######################################################################################################################
# Gradle rule target configuration

gradle_rule_targets = $(addsuffix /gradlerules.mk,$(OUTDIR))

GENERIC_TARGETS += $(GRADLE_TARGETS)
RULE_TARGETS += $(gradle_rule_targets)

#######################################################################################################################
# Makefile dependencies

#######################################################################################################################
# CMake rules

$(gradle_rule_targets):
	$(SILENT) \
	{ \
	  echo '$(\#) generated file - do not edit!!!'; \
	  echo; \
	  ID='$(call id,$@)'; \
	  echo "ifndef $${ID}_include_guard"; \
	  echo "$${ID}_include_guard := 1"; \
	  echo; \
	$(foreach target,$(GRADLE_TARGETS),\
	  echo '$(\#)$(\#) BEGIN of gradle $(target) configuration'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) defaults'; \
	  echo '$(target) ?= $$(call memorize,$(target),$(call target_properties_parse,$(target)))'; \
	  echo '$(target).generic_adapter = $$(call $$1.gradle_adapter,$$1)'; \
	  echo '$(target).generic_extdir = $$(call $$1.gradle_extdir,$$1)'; \
	  echo '$(target).generic_prerequisites = $$(call $$1.gradle_prerequisites,$$1) $$(call gradle_default_prerequisites,$$1)'; \
	  echo '$(target).generic_orderonly_prerequisites ?= $$(call $$1.gradle_orderonly_prerequisites,$$1)'; \
	  echo '$(target).generic_prepare = $$(call $$1.gradle_prepare,$$1)'; \
	  echo '$(target).generic_assemble = $$(call $$1.gradle_assemble,$$1)'; \
	  echo '$(target).generic_saveartifacts = $$(call $$1.gradle_saveartifacts,$$1)'; \
	  echo '$(target).generic_lint = $$(call $$1.gradle_lint,$$1)'; \
	  echo '$(target).generic_check = $$(call $$1.gradle_check,$$1)'; \
	  echo '$(target).generic_memcheck = $$(call $$1.gradle_memcheck,$$1)'; \
	  echo '$(target).generic_package = $$(call $$1.gradle_package,$$1)'; \
	  echo '$(target).generic_install = $$(call $$1.gradle_install,$$1)'; \
	  echo '$(target).generic_delegate = $$(call $$1.gradle_delegate,$$1)'; \
	  echo '$(target).generic_buildtesting = $$(call $$1.gradle_buildtesting,$$1)'; \
	  echo '$(target).gradle_adapter ?= $$(call dobuild_gradle_adapter,$$1)'; \
	  echo '$(target).gradle_extdir ?= $$(call dobuild_gradle_extdir,$$1)'; \
	  echo '$(target).gradle_prerequisites ?= $$(call dobuild_gradle_prerequisites,$$1)'; \
	  echo '$(target).gradle_orderonly_prerequisites ?= $$(call dobuild_gradle_orderonly_prerequisites,$$1)'; \
	  echo '$(target).gradle_prepare ?= $$(call dobuild_gradle_prepare,$$1)'; \
	  echo '$(target).gradle_assemble ?= $$(call dobuild_gradle_assemble,$$1)'; \
	  echo '$(target).gradle_saveartifacts ?= $$(call dobuild_gradle_saveartifacts,$$1)'; \
	  echo '$(target).gradle_lint ?= $$(call dobuild_gradle_lint,$$1)'; \
	  echo '$(target).gradle_check ?= $$(call dobuild_gradle_check,$$1)'; \
	  echo '$(target).gradle_memcheck ?= $$(call dobuild_gradle_memcheck,$$1)'; \
	  echo '$(target).gradle_package ?= $$(call dobuild_gradle_package,$$1)'; \
	  echo '$(target).gradle_install ?= $$(call dobuild_gradle_install,$$1)'; \
	  echo '$(target).gradle_delegate ?= $$(call dobuild_gradle_delegate,$$1)'; \
	  echo '$(target).gradle_buildtesting ?= $$(call dobuild_gradle_buildtesting,$$1)'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) cached values'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) rules'; \
	  echo; \
	  echo '$(\#)$(\#) END of gradle $(target) configuration'; \
	  echo; \
	) \
	  echo 'endif'; \
	  echo; \
	} > $@

endif

