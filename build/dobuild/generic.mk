#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

current_makefile := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

# Guard from user inclusion
ifndef standardrules_include_guard
  $(error $(current_makefile) is not intended to be included directly - include standardrules.mk instead)
endif

ifndef generic_include_guard
generic_include_guard := 1

ifndef defaults_include_guard
  include $(patsubst %/,%,$(dir $(current_makefile)))/defaults.mk
endif

#######################################################################################################################
# Overridable generic defaults

DOBUILD_GENERIC_ADAPTER ?= generic

#######################################################################################################################
# Overridable generic macros, hooks to customize target default values

# hook called to retrieve the target generic extension directory, may return an empty value
# $(call dobuild_generic_extdir,target-name)
dobuild_generic_extdir ?= 

# hook called to retrieve the target generic adapter name, may return an empty value
# $(call dobuild_generic_adapter,target-name)
dobuild_generic_adapter ?= 

# hook called to retrieve the target generic prerequisites, may return an empty list in which case the default generic
# prerequisites are used
# $(call dobuild_generic_prerequisites,target-name)
dobuild_generic_prerequisites ?= 

# hook called to retrieve the target generic order only prerequisites, may return an empty list in which case the
# default generic order only prerequisites are used
# $(call dobuild_generic_orderonly_prerequisites,target-name)
dobuild_generic_orderonly_prerequisites ?= 

# hook called to retrieve the target generic perpare step, may return an empty value
# $(call dobuild_generic_prepare,target-name)
dobuild_generic_prepare ?= prepare

# hook called to retrieve the target generic assemble step
# $(call dobuild_generic_assemble,target-name)
dobuild_generic_assemble ?= assemble

# hook called to retrieve the target generic save artifacts step, may return an empty value
# $(call dobuild_generic_saveartifacts,target-name)
dobuild_generic_saveartifacts ?= save-artifacts

# hook called to retrieve the target generic lint step, may return an empty value
# $(call dobuild_generic_lint,target-name)
dobuild_generic_lint ?= lint

# hook called to retrieve the target generic check step, may return an empty value
# $(call dobuild_generic_check,target-name)
dobuild_generic_check ?= check

# hook called to retrieve the target generic memcheck step, may return an empty value
# $(call dobuild_generic_memcheck,target-name)
dobuild_generic_memcheck ?= check-memcheck

# hook called to retrieve the target generic package step, may return an empty value
# $(call dobuild_generic_package,target-name)
dobuild_generic_package ?= package

# hook called to retrieve the target generic install step, may return an empty value
# $(call dobuild_generic_install,target-name)
dobuild_generic_install ?= package-install

# hook called to retrieve the target generic delegate step
# $(call dobuild_generic_delegate,target-name)
dobuild_generic_delegate ?= delegate

# hook called to retrieve the target generic build testing option,
# may return an empty value in which case BUILD_TESTING is used
# $(call dobuild_generic_buildtesting,target-name)
dobuild_generic_buildtesting ?= 

#######################################################################################################################
# Generic macros

# replaces template values of target field with target properties and resolves result against extension search 
# directories, returning the first existing resolved path
# $(call generic_resolve_ext,target-name,field)
generic_resolve_ext = $(firstword $(call target_subst_and_resolve,$1,$2,$(cache.$1.ext_search_paths)))

# retrieves the target generic default prerequisites
# $(call generic_default_prerequisites,target-name)
generic_default_prerequisites = \
  $(call image_buildtarget,$1) \
  $(OUTDIR)/$1/DoBuildFiles/generic.properties

# retrieves the target generic default extension directory
# $(call generic_default_extdir,target-name)
generic_default_extdir = $(addprefix $(addsuffix /,$(EXTDIR)),$(cache.$1.adapter))

# creates a generic run rule command
# $(call generic_run_cmd,target-name,command[,args],alternative-command[,output-command])
generic_run_cmd = $(call run_cmd,$1,$(or $(strip $(notdir $(wildcard $2))),$(strip $4)),$3,$5)

# create a generic perpare command
# $(call generic_prepare_cmd,target-name)
generic_prepare_cmd = $(call generic_run_cmd,$1,$(cache.$1.generic_prepare),,true,prepare)

# create a generic assemble command
# $(call generic_assemble_cmd,target-name)
generic_assemble_cmd = $(call generic_run_cmd,$1,$(cache.$1.generic_assemble),,$(call dobuild_generic_assemble,$1),assemble)

# create a generic save artifacts command
# $(call generic_saveartifacts_cmd,target-name)
generic_saveartifacts_cmd = { $(call generic_run_cmd,$1,$(cache.$1.generic_saveartifacts),,tar -cf - -T /dev/null,save-artifacts) | tar xvf - -C '$(OUTDIR)/$1/DoBuildFiles'; }

# create a generic lint command
# $(call generic_lint_cmd,target-name)
generic_lint_cmd = $(call generic_run_cmd,$1,$(cache.$1.generic_lint),,true,lint)

# create a generic check command
# $(call generic_check_cmd,target-name)
generic_check_cmd = $(call generic_run_cmd,$1,$(cache.$1.generic_check),,true,check)

# create a generic memcheck command
# $(call generic_memcheck_cmd,target-name)
generic_memcheck_cmd = $(call generic_run_cmd,$1,$(cache.$1.generic_memcheck),,true,memcheck)

# create a generic package command
# $(call generic_package_cmd,target-name)
generic_package_cmd = $(call generic_run_cmd,$1,$(cache.$1.generic_package),,true,package)

# create a generic install command
# $(call generic_install_cmd,target-name)
generic_install_cmd = $(call generic_run_cmd,$1,$(cache.$1.generic_install),$(generic_destdir),true,install)

# create a generic clean command
# $(call generic_clean_cmd,target-name)
generic_clean_cmd = rm -rf '$(OUTDIR)/$1'

# create a generic delegate command
# $(call generic_delegate_cmd,target-name,delegate-target-name)
generic_delegate_cmd = $(call generic_run_cmd,$1,$(cache.$1.generic_delegate),$2,$(call dobuild_generic_delegate,$1),delegate)

# creates a generic properties generation cmd
# $(call generic_props_cmd,target-name,input-file)
generic_props_cmd = sed \
  -e 's!%TARGET%!$(call escape,$1,!)!g' \
  -e 's!%DOBUILDDIR%!$(call escape,$(generic_dobuilddir),!)!g' \
  -e 's!%PROJECTDIR%!$(call escape,$(generic_projectdir),!)!g' \
  -e 's!%HOSTMARCH%!$(call escape,$(call target_host_arch,$1),!)!g' \
  -e 's!%MARCH%!$(call escape,$(call target_arch_sub,$1),!)!g' \
  -e 's!%DISTRIB%!$(call escape,$(call target_distrib,$1),!)!g' \
  -e 's!%DISTRIB_ID%!$(call escape,$(call target_distrib_id,$1),!)!g' \
  -e 's!%DISTRIB_VERSION%!$(call escape,$(call target_distrib_version,$1),!)!g' \
  -e 's!%SYS%!$(call escape,$(call target_sys,$1),!)!g' \
  -e 's!%ABI%!$(call escape,$(call target_abi,$1),!)!g' \
  -e 's!%ID%!$(call escape,$(call target_id,$1),!)!g' \
  -e 's!%VARIANT%!$(call escape,$(call target_variant,$1),!)!g' \
  -e 's!%BUILD_TESTING%!$(call escape,$(cache.$1.generic_buildtesting),!)!g' \
  $2

# creates a generic properties generation rule
# $(call generic_props_rule,target-name)
generic_props_rule = \
  $$(OUTDIR)/$1/DoBuildFiles/generic.properties: $$(DOBUILDDIR)/assets/templates/properties.template $$(MAKEFILE_LIST); \
    $$(SILENT)mkdir -p $$(dir $$@); $$(call generic_props_cmd,$1,$$<) > $$@

# creates a generic prepare rule
# $(call generic_prepare_rule,target-name)
generic_prepare_rule = \
  $$(OUTDIR)/$1/DoBuildFiles/prepare.stage: $$(cache.$1.generic_prepare) $$(cache.$1.generic_prerequisites) | $$(cache.$1.generic_orderonly_prerequisites); \
    $$(SILENT)$$(call generic_prepare_cmd,$1) && touch $$@

# creates a generic assemble rule
# $(call generic_assemble_rule,target-name)
generic_assemble_rule = \
  build/$1 $$(OUTDIR)/$1/DoBuildFiles/assemble.stage: $$(cache.$1.generic_assemble) $$(OUTDIR)/$1/DoBuildFiles/prepare.stage | $$(OUTDIR)/$1/DoBuildFiles/save-artifacts.stage; \
    $$(SILENT)$$(call generic_assemble_cmd,$1) && touch $$(OUTDIR)/$1/DoBuildFiles/assemble.stage

# creates a generic save artifacts rule
# $(call generic_saveartifacts_rule,target-name)
generic_saveartifacts_rule = \
  $$(OUTDIR)/$1/DoBuildFiles/save-artifacts.stage: $$(cache.$1.generic_saveartifacts) $$(OUTDIR)/$1/DoBuildFiles/prepare.stage; \
    $$(SILENT)$$(call generic_saveartifacts_cmd,$1) && touch $$@

# creates a generic lint rule
# $(call generic_lint_rule,target-name)
generic_lint_rule = \
  lint/$1 $$(OUTDIR)/$1/DoBuildFiles/lint.stage: $$(cache.$1.generic_lint) $$(OUTDIR)/$1/DoBuildFiles/assemble.stage; \
    $$(SILENT)$$(call generic_lint_cmd,$1) && touch $$(OUTDIR)/$1/DoBuildFiles/lint.stage

# creates a generic check rule
# $(call generic_check_rule,target-name)
generic_check_rule = \
  check/$1 $$(OUTDIR)/$1/DoBuildFiles/check.stage: $$(cache.$1.generic_check) $$(OUTDIR)/$1/DoBuildFiles/assemble.stage; \
    $$(SILENT)$$(call generic_check_cmd,$1) && touch $$(OUTDIR)/$1/DoBuildFiles/check.stage

# creates a generic memcheck rule
# $(call generic_memcheck_rule,target-name)
generic_memcheck_rule = \
  memcheck/$1 $$(OUTDIR)/$1/DoBuildFiles/memcheck.stage: $$(cache.$1.generic_memcheck) $$(OUTDIR)/$1/DoBuildFiles/assemble.stage; \
    $$(SILENT)$$(call generic_memcheck_cmd,$1) && touch $$(OUTDIR)/$1/DoBuildFiles/memcheck.stage

# creates a generic package rule
# $(call generic_package_rule,target-name)
generic_package_rule = \
  package/$1 $$(OUTDIR)/$1/DoBuildFiles/package.stage: $$(cache.$1.generic_package) $$(OUTDIR)/$1/DoBuildFiles/assemble.stage; \
    $$(SILENT)$$(call generic_package_cmd,$1) && touch $$(OUTDIR)/$1/DoBuildFiles/package.stage

# creates a generic install rule
# $(call generic_install_rule,target-name)
generic_install_rule = \
  install/$1: $$(wildcard $$(cache.$1.generic_install)) $$(OUTDIR)/$1/DoBuildFiles/assemble.stage; \
    $$(SILENT)$$(call generic_install_cmd,$1)

# creates a generic clean rule
# $(call generic_clean_rule,target-name)
generic_clean_rule = \
  clean/$1: ; \
    $$(SILENT)-$$(call generic_clean_cmd,$1)

# creates a generic delegate rule 
# $(call generic_delegate_rule,adapter,target-name,delegate-target-name)
generic_delegate_rule = \
  $1/$2/$3: $$(OUTDIR)/$2/DoBuildFiles/prepare.stage; \
    $$(SILENT)$$(call generic_delegate_cmd,$2,$3)

# creates a generic delegate pattern rule 
# $(call generic_delegate_pattern_rule,adapter,target-name)
generic_delegate_pattern_rule = \
  $1/$2/%: $$(OUTDIR)/$2/DoBuildFiles/prepare.stage; \
    $$(SILENT)$$(call generic_delegate_cmd,$2,$$*)

# creates a generic delegate makefile generation command, assuming target is the makefile to create and
# first prerequisite a file containing the delegate target names
# $(call generic_delegate_makefile_rule_cmd,target-name)
generic_delegate_makefile_rule_cmd = \
  { \
    echo '$(\\\#) generated file - do not edit!!!'; \
    echo; \
    ID='$(call id,$@)'; \
    echo "ifndef $${ID}_include_guard"; \
    echo "$${ID}_include_guard := 1"; \
    echo; \
    $(foreach target,$(shell cat '$<' 2>/dev/null),\
      echo '$(call generic_delegate_rule,$$(cache.$1.adapter),$1,$(target))'; \
      echo; \
    ) \
    echo 'endif'; \
    echo; \
  } > $@

# creates a generic delegate makefile generation rule 
# $(call generic_delegate_makefile_rule,target-name)
generic_delegate_makefile_rule = \
  $$(OUTDIR)/$1/DoBuildFiles/generic_delegaterules.mk: $$(wildcard $$(OUTDIR)/$1/DoBuildFiles/targets.txt) | $$(OUTDIR)/$1/DoBuildFiles; \
    $$(SILENT)$$(call generic_delegate_makefile_rule_cmd,$1)

#######################################################################################################################
# Generic rule target configuration

GENERIC_ADAPTER = $(call memorize,GENERIC_ADAPTER,$(DOBUILD_GENERIC_ADAPTER))

generic_projectdir = $(or $(container_projectdir),$(PROJECTDIR))
generic_dobuilddir = $(or $(container_dobuilddir),$(DOBUILDIR))
generic_destdir = $(or $(container_destdir),$(DESTDIR))

generic_defaulttarget = $(if $(SKIP_DEFAULTTARGET),,$(DEFAULTTARGET))
generic_targets = $(filter $(generic_defaulttarget),$(GENERIC_TARGETS))
generic_selected_targets = $(if $(SKIP_DEFAULTTARGET),$(GENERIC_TARGETS),$(generic_targets))
generic_active_targets = $(call memorize,generic_active_targets,$(call target_filter,$(FILTER),$(generic_selected_targets),$(EXCLUDEFILTER)))

generic_prepare_targets = $(addsuffix /DoBuildFiles/prepare.stage,$(addprefix $(OUTDIR)/,$(generic_active_targets)))
generic_build_targets = $(addprefix build/,$(generic_active_targets))
generic_clean_targets = $(addprefix clean/,$(generic_active_targets))
generic_check_targets = $(addprefix check/,$(generic_active_targets))
generic_memcheck_targets = $(addprefix memcheck/,$(call target_filter,$(MEMCHECKFILTER),$(generic_active_targets),))
generic_lint_targets = $(addprefix lint/,$(generic_active_targets))
generic_dist_targets = $(addprefix package/,$(generic_active_targets))
generic_install_targets = $(addprefix install/,$(generic_active_targets))
generic_rule_targets = $(addsuffix /genericrules.mk,$(OUTDIR))
generic_outdirs = $(addprefix $(OUTDIR)/,$(generic_active_targets))

project_targets += $(GENERIC_TARGETS)

PREPARE_TARGETS += $(generic_prepare_targets)
BUILD_TARGETS += $(generic_build_targets)
LINT_TARGETS += $(generic_lint_targets)
CHECK_TARGETS += $(generic_check_targets)
MEMCHECK_TARGETS += $(generic_memcheck_targets)
CLEAN_TARGETS += $(generic_clean_targets)
INSTALL_TARGETS += $(generic_install_targets)
DIST_TARGETS += $(generic_dist_targets)
RULE_TARGETS += $(generic_rule_targets)
TARGETS += $(generic_active_targets)
OUTDIRS += $(generic_outdirs)

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += touch
MAKEFILE_DEPS += echo
MAKEFILE_DEPS += sed
MAKEFILE_DEPS += tar

#######################################################################################################################
# Generic rules

$(generic_rule_targets):
	$(SILENT) \
	{ \
	  echo '$(\#) generated file - do not edit!!!'; \
	  echo; \
	  ID='$(call id,$@)'; \
	  echo "ifndef $${ID}_include_guard"; \
	  echo "$${ID}_include_guard := 1"; \
	  echo; \
	$(foreach target,$(GENERIC_TARGETS),\
	  echo '$(\#)$(\#) BEGIN of generic $(target) configuration'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) defaults'; \
	  echo '$(target) ?= $$(call memorize,$(target),$(call target_properties_parse,$(target)))'; \
	  echo '$(target).env_path = $$(call env_default_path,$$1)'; \
	  echo '$(target).adapter = $$(call $$1.generic_adapter,$$1)'; \
	  echo '$(target).extdir = $$(call $$1.generic_extdir,$$1)'; \
	  echo '$(target).generic_adapter ?= $$(call dobuild_generic_adapter,$$1)'; \
	  echo '$(target).generic_extdir ?= $$(call dobuild_generic_extdir,$$1)'; \
	  echo '$(target).generic_prerequisites ?= $$(call dobuild_generic_prerequisites,$$1)'; \
	  echo '$(target).generic_orderonly_prerequisites ?= $$(call dobuild_generic_orderonly_prerequisites,$$1)'; \
	  echo '$(target).generic_prepare ?= $$(call dobuild_generic_prepare,$$1)'; \
	  echo '$(target).generic_assemble ?= $$(call dobuild_generic_assemble,$$1)'; \
	  echo '$(target).generic_saveartifacts ?= $$(call dobuild_generic_saveartifacts,$$1)'; \
	  echo '$(target).generic_lint ?= $$(call dobuild_generic_lint,$$1)'; \
	  echo '$(target).generic_check ?= $$(call dobuild_generic_check,$$1)'; \
	  echo '$(target).generic_memcheck ?= $$(call dobuild_generic_memcheck,$$1)'; \
	  echo '$(target).generic_package ?= $$(call dobuild_generic_package,$$1)'; \
	  echo '$(target).generic_install ?= $$(call dobuild_generic_install,$$1)'; \
	  echo '$(target).generic_delegate ?= $$(call dobuild_generic_delegate,$$1)'; \
	  echo '$(target).generic_buildtesting ?= $$(call dobuild_generic_buildtesting,$$1)'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) cached values'; \
	  echo 'cache.$(target).ext_search_paths = $$(call memorize,cache.$(target).ext_search_paths,$$(call split_s,$$(call env_host_path,$(target)),:))'; \
	  echo 'cache.$(target).env_path = $$(call memorize,cache.$(target).env_path,$$(call $(target).env_path,$(target)))'; \
	  echo 'cache.$(target).adapter = $$(call memorize,cache.$(target).adapter,$$(call target_get_and_subst,$(target),adapter,$(GENERIC_ADAPTER)))'; \
	  echo 'cache.$(target).extdir = $$(call memorize,cache.$(target).extdir,$$(call target_get_and_subst,$(target),extdir,$$(call generic_default_extdir,$(target))))'; \
	  echo 'cache.$(target).generic_prerequisites = $$(call memorize,cache.$(target).generic_prerequisites,$$(call target_get_and_subst,$(target),generic_prerequisites,) $$(call generic_default_prerequisites,$(target)))'; \
	  echo 'cache.$(target).generic_orderonly_prerequisites = $$(call memorize,cache.$(target).generic_orderonly_prerequisites,$$(call target_get_and_subst,$(target),generic_orderonly_prerequisites,))'; \
	  echo 'cache.$(target).generic_prepare = $$(call memorize,cache.$(target).generic_prepare,$$(call generic_resolve_ext,$(target),generic_prepare))'; \
	  echo 'cache.$(target).generic_assemble = $$(call memorize,cache.$(target).generic_assemble,$$(call generic_resolve_ext,$(target),generic_assemble))'; \
	  echo 'cache.$(target).generic_saveartifacts = $$(call memorize,cache.$(target).generic_saveartifacts,$$(call generic_resolve_ext,$(target),generic_saveartifacts))'; \
	  echo 'cache.$(target).generic_lint = $$(call memorize,cache.$(target).generic_lint,$$(call generic_resolve_ext,$(target),generic_lint))'; \
	  echo 'cache.$(target).generic_check = $$(call memorize,cache.$(target).generic_check,$$(call generic_resolve_ext,$(target),generic_check))'; \
	  echo 'cache.$(target).generic_memcheck = $$(call memorize,cache.$(target).generic_memcheck,$$(call generic_resolve_ext,$(target),generic_memcheck))'; \
	  echo 'cache.$(target).generic_package = $$(call memorize,cache.$(target).generic_package,$$(call generic_resolve_ext,$(target),generic_package))'; \
	  echo 'cache.$(target).generic_install = $$(call memorize,cache.$(target).generic_install,$$(call generic_resolve_ext,$(target),generic_install))'; \
	  echo 'cache.$(target).generic_delegate = $$(call memorize,cache.$(target).generic_delegate,$$(call generic_resolve_ext,$(target),generic_delegate))'; \
	  echo 'cache.$(target).generic_buildtesting = $$(call memorize,cache.$(target).generic_buildtesting,$$(or $$(call $(target).generic_buildtesting,$(target)),$$(BUILD_TESTING)))'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) rules'; \
	  echo '$(call generic_props_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_prepare_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_assemble_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_saveartifacts_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_lint_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_check_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_memcheck_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_package_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_install_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_clean_rule,$(target))'; \
	  echo; \
	  echo '$(call generic_delegate_makefile_rule,$(target))'; \
	  echo; \
	  echo '$$(cache.$(target).adapter)/$(target): build/$(target)'; \
	  echo; \
	  echo '-include $$(OUTDIR)/$(target)/DoBuildFiles/generic_delegaterules.mk'; \
	  echo; \
	  echo '$(call generic_delegate_pattern_rule,$$(cache.$(target).adapter),$(target))'; \
	  echo; \
	  echo '$(\#)$(\#) END of generic $(target) configuration'; \
	  echo; \
	) \
	  echo 'endif'; \
	  echo; \
	} > $@

endif

