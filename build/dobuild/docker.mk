#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

ifndef _DOBUILD_INCLUDE_GUARD_DOCKER
_DOBUILD_INCLUDE_GUARD_DOCKER := 1

current_makefile := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

ifndef defaults_include_guard
  include $(patsubst %/,%,$(dir $(current_makefile)))/defaults.mk
endif

#######################################################################################################################
# Overridable docker defaults

DOCKER ?= docker
DOBUILD_DOCKERFILE ?= $(PROJECTDIR)/%MARCH%-%DISTRIB_ID%-%ID%.dockerfile
DOBUILD_RUNCMD ?= 

#######################################################################################################################
# Overridable docker macros, hooks to customize target default values

# hook called to retrieve the target dockerfile name,
# may return an empty value in which case the default dockerfile is used
# $(call dobuild_dockerfile,target-name)
dobuild_dockerfile ?= $(DOCKERFILE)

# hook called to retrieve the target image build context,
# may return an empty value in which case OUTDIR is used
# $(call dobuild_image_context,target-name)
dobuild_image_context ?= 

# hook called to retrieve the target image archive file name, 
# may return an empty value
# $(call dobuild_image_archivefile,target-name)
dobuild_image_archivefile ?= $(call image_default_archivefile,$1)

# hook called to retrieve the target image prerequisites, 
# may return an empty list
# $(call dobuild_image_prerequisites,target-name)
dobuild_image_prerequisites ?= 

# hook called to retrieve the target image names,
# may return an empty list in which case the name is derived from the build target name
# $(call dobuild_image_names,target-name)
dobuild_image_names ?=

# hook called to retrieve the target container user,
# may return an empty value in which case CONTAINER_USER is used
# $(call dobuild_container_user,target-name)
dobuild_container_user ?= 

# hook called to retrieve the target container group, 
# may return an empty value in which case CONTAINER_GROUP is used
# $(call dobuild_container_group,target-name)
dobuild_container_group ?= 

# hook called to retrieve the target container run command, 
# may return an empty value in which case CONTAINER_CMD is used
# $(call dobuild_container_cmd,target-name)
dobuild_container_cmd ?= 

# hook called to retrieve the target image build arguments,
# may return an empty list
# $(call dobuild_image_buildargs,target-name)
dobuild_image_buildargs ?= \
  $(addprefix HOSTMARCH=,%HOSTMARCH%) \
  $(addprefix MARCH=,%MARCH%) \
  $(addprefix DISTRIB_ID=,%DISTRIB_ID%) \
  $(addprefix DISTRIB_VERSION=,%DISTRIB_VERSION%) \
  $(addprefix SYS=,%SYS%) \
  $(addprefix ABI=,%ABI%) \
  $(addprefix ID=,%ID%) \
  $(addprefix VARIANT=,%VARIANT%)

# hook called to retrieve the target docker build flags,
# may return an empty list
# $(call dobuild_docker_buildflags,target-name)
dobuild_docker_buildflags ?= 

# hook called to retrieve the target docker run flags,
# may return an empty list
# $(call dobuild_docker_runflags,target-name)
dobuild_docker_runflags ?=

#######################################################################################################################
# Docker macros

# retrieves the version parts (major, minor) of a docker version string
# $(call docker_version_parts,version-string)
docker_version_parts = $(shell echo '$1' | sed -ne 's![^0-9]*[0]*\([^.]\+\).[0]*\([^.]\+\).*!\1 \2!p')

# tests if docker version is greater or equal to required version
# $(call docker_version_ge,required-version-string)
docker_version_ge = \
  $(call ge_version, \
    $(call docker_version_parts,$(docker_version)),\
    $(call docker_version_parts,$1)\
  )

# retrieves the target default docker identifier file
# $(call docker_default_idfile,target-name...)
docker_default_idfile = $(addsuffix /DoBuildFiles/docker.idfile,$(addprefix $(OUTDIR)/,$1))

# retrieves the target default dockerfile name
# $(call image_default_dockerfile,target-name...)
image_default_dockerfile = $(addsuffix /Dockerfile,$(addprefix $(OUTDIR)/,$1))

# retrieves the target default image archive file name
# $(call image_default_archivefile,target-name...)
image_default_archivefile = $(addsuffix /image-root$(addprefix -,$(VERSION)).tar,$(addprefix $(OUTDIR)/,$1))

# retrieves the target default image prerequisites
# $(call image_default_prerequisites,target-name)
image_default_prerequisites = 

# retrieves the target default image build flags
# $(call image_default_buildflags,target-name)
image_default_buildflags = $(addprefix --build-arg ,$(strip $(cache.$1.image_buildargs)))

# retrieves the target default environment path variable value
# $(call env_default_path,target-name)
env_default_path = $(call join_s,\
    $(abspath \
      $(cache.$1.container_extdir) \
      $(addprefix $(container_dobuilddir)/assets/adapters/,$(cache.$1.adapter)) \
    ) \
    $(call image_env_path,$1),\
    : \
  )

# retrieves the target default extension directory value
# $(call container_default_exdir,target-name)
container_default_extdir = $(if $(docker_portable_workspace),$(container_extdir),$(cache.$1.extdir))

# retrieves the target default run flags
# $(call docker_default_runflags,target-name)
docker_default_runflags = \
  $(if $(and $(docker_portable_workspace),$(realpath $(cache.$1.extdir))),--volume '$(realpath $(cache.$1.extdir)):$(abspath $(cache.$1.container_extdir)):cached')

# retrieves the target image path env variable value
# $(call image_env_path,target-name)
image_env_path = $(shell $(DOCKER) inspect -f '{{range $$i, $$var := .Config.Env}}{{println $$var}}{{end}}' '$(call image_name,$1)' | sed -n -e 's!^PATH=\(.*\)!\1!p')

# formats a list of target properties as string
# $(call target_properties_format,property...)
image_target_properties_format = \
  $(call join_s, \
    $(call target_property_host_arch,$1) \
    $(call target_property_arch_sub,$1) \
    $(call target_property_vendor_or_distrib_id,$1) \
    $(call target_property_distrib_version,$1) \
    $(call target_property_sys,$1) \
    $(call target_property_abi,$1) \
    $(PROJECTNAME) \
    $(call target_property_id,$1) \
    $(call replace_all,$(call target_property_variant,$1),release,), \
  /)

# retrieves the target default image qualified name (registry/repository:tag-version)
# $(call image_default_name,target-name[,tag-version][,registry-prefix])
image_default_name = \
  $(call join_s, \
    $3 \
    $(call image_target_properties_format,$(call target_properties,$1)) \
    $(addprefix :,$2) \
  )

# retrieves the target image archive file name
# $(call image_archivefile,target-name...)
image_archivefile = $(foreach target,$1,$(cache.$(target).image_archivefile))

# retrieves the target image build target name
# $(call image_buildtarget,target-name...)
image_buildtarget = $(call image_idfile,$(filter $1,$(DOCKER_TARGETS)))

# retrieves the target image identifier file
# $(call image_idfile,target-name...)
image_idfile = $(addsuffix -image.idfile,$(addprefix $(OUTDIR)/,$1))

# retrieves the unique local identifier from id file
# $(call image_id,id-file...)
image_id = $(shell cat $1 2>/dev/null)

# retrieves the image hash value from id file
# $(call image_hash,id-file...)
image_hash = $(patsubst sha256:%,%,$(call image_id,$1))

# retrieves the target image name
# $(call image_name,target-name...)
image_name = $(foreach target,$1,$(cache.$(target).image_name))

# creates an image build command
# $(call image_build_cmd,dockerfile,tag...,context[,docker-build-flag...][,output-prefix])
image_build_cmd = $(call echo_cmd,$(DOCKER),build --rm $4 --file '$1' $(addprefix --tag ,$2) '$3',$(addprefix $5 ,docker))

# creates an image build rule command for given target configuration, assuming the first prerequisite is the dockerfile
# $(call image_build_rule_cmd,target-name,idfile)
image_build_rule_cmd = { \
    mkdir -p $(dir $2); \
    $(call image_build_cmd,$<, \
      $(cache.$1.image_tags),$(cache.$1.image_context),$(docker_buildflags) $(cache.$1.docker_buildflags) --iidfile '$2',\
      $(call color,green,TARGET=$1)); \
  }

# creates an image build rule
# $(call image_build_rule,target-name)
image_build_rule = \
  build/image/$1 $$(cache.$1.docker_idfile): $$(cache.$1.dockerfile) $$(cache.$1.image_prerequisites) $$(EXTRACT_TARGETS) $$(MAKEFILE_LIST); \
    $$(SILENT)$$(call image_build_rule_cmd,$1,$$(cache.$1.docker_idfile))

# creates an image tag rule command for given target configuration
# $(call image_tag_rule_cmd,target-name,oci-idfile,docker-idfile)
# NOTE: workaround for moby/moby issue #39796
# referencing images with local sha256:id does not work when referenced as base image using BuildKit,
# therefore we tag the image additionally with its local unique identifier
image_tag_rule_cmd = { \
    mkdir -p $(dir $2); \
    name='$(call image_default_name,$1,$(call image_hash,$3),)'; \
       $(call echo_cmd,$(DOCKER),tag '$(call image_id,$3)' "$$name",docker) \
    && echo "$$name" > '$2'; \
  }

# creates an image tag rule
# $(call image_tag_rule,target-name)
image_tag_rule = \
  $$(cache.$1.image_idfile): $$(cache.$1.docker_idfile); \
    $$(SILENT)$$(call image_tag_rule_cmd,$1,$$(cache.$1.image_idfile),$$(cache.$1.docker_idfile))

# creates an image save command
# $(call image_save_cmd,image-name,output-file[,output-prefix])
image_save_cmd = $(call echo_cmd,$(DOCKER),save '$1' $(addprefix --output ,$2),$(addprefix $3 ,docker))

# creates an image save rule command for given target configuration
# $(call image_save_rule_cmd,target-name,archivefile)
image_save_rule_cmd = $(call image_save_cmd,$(call image_name,$1),$2,$(call color,green,TARGET=$1))

# creates an image save rule
# $(call image_save_rule,target-name)
image_save_rule = \
  package/image/$1 $$(cache.$1.image_archivefile): $$(cache.$1.image_idfile); \
    $$(SILENT)$$(call image_save_rule_cmd,$1,$$(cache.$1.image_archivefile))

# creates an image load command
# $(call image_load_cmd,image-name,input-file)
image_load_cmd = $(call echo_if_silent_cmd,docker image load $2) \
  && $(DOCKER) tag "$$($(DOCKER) load --quiet --input '$2' | sed -n -e 's!^Loaded image ID:\s\+\(.*\)!\1!p')" $1

# creates an image load rule command for given target configuration, assuming the first prerequisite is the image 
# archive file to load
# $(call image_load_rule_cmd,image-name)
image_load_rule_cmd = \
  $(call image_load_cmd,$1,$<)

# creates an image remove command
# $(call image_rm_cmd,image-name...[,output-prefix])
image_rm_cmd = $(if $(strip $1),$(call echo_cmd,$(DOCKER),rmi -f $1,$(addprefix $2 ,docker)),true)

# creates an image clean command
# $(call image_clean_cmd,target-name[,output-prefix])
image_clean_cmd = { \
    $(call image_rm_cmd,$(cache.$1.image_tags),$2) 2>/dev/null; \
    rm -f '$(cache.$1.docker_idfile)'; \
  }

# creates an image clean rule command
# $(call image_clean_rule_cmd,target-name)
image_clean_rule_cmd = $(call image_clean_cmd,$1,$(call color,green,TARGET=$1))

# creates an image clean rule
# $(call image_clean_rule,target-name)
image_clean_rule = \
  clean/image/$1: ; \
    $$(SILENT)-$$(call image_clean_rule_cmd,$1)

# creates an image dist clean rule command
# $(call image_distclean_rule_cmd,target-name)
image_distclean_rule_cmd = { \
    $(call echo_if_silent_cmd,$(call color,green,TARGET=$1) distclean image); \
    $(call image_clean_cmd,$1,); \
    $(call image_rm_cmd,$(call image_name,$1),); \
    rm -f '$(cache.$1.image_idfile)'; \
  }

# creates an image dist clean rule
# $(call image_distclean_rule,target-name)
image_distclean_rule = \
  distclean/image/$1: ; \
    $$(SILENT)-$$(call image_distclean_rule_cmd,$1)

# retrieves the target container default working directory
# $(call container_default_workdir,target-name...)
container_default_workdir = $(addprefix $(container_outdir)/,$1)

# creates a command which is executed in a container
# $(call container_run_cmd,[target-name],image-name,workdir,command...[,docker-run-flag...][,output-command])
container_run_cmd = \
     $(call echo_if_silent_cmd,$(call color,green,$(addprefix TARGET=,$1)) container_run $(or $6,$4)) \
  && $(DOBUILDDIR)/bin/container_run $5 --workdir '$3' '$2' $4

# creates a container run rule
# $(call container_run_rule,target-name)
container_run_rule = \
  run/container/$1: $$(cache.$1.image_idfile); \
    $$(SILENT)-$$(call run_cmd,$1,$$(cache.$1.container_cmd))

# creates a container run rule command
# $(call run_cmd,target-name,command[,args...][,output-command])
run_cmd = $(call container_run_cmd,$1,$(call image_name,$1),$(cache.$1.container_workdir),$2 $3,\
  $(docker_runflags) -e 'PATH=$(cache.$1.env_path)' \
  --user '$(cache.$1.container_user):$(cache.$1.container_group)' $(cache.$1.docker_runflags),$(or $4,$2) $3)

#######################################################################################################################
# Docker rule target configuration

DOCKERFILE = $(call memorize,DOCKERFILE,$(DOBUILD_DOCKERFILE))

CONTAINER_CMD = $(call memorize,CONTAINER_CMD,$(DOBUILD_RUNCMD))
CONTAINER_USER = $(USERID)
CONTAINER_GROUP = $(USERID)

docker_version = $(call memorize,docker_version,$(shell $(DOCKER) version --format '{{.Client.Version}}'))
docker_version_req = 18.09

container_dobuilddir = /mnt/dobuild
container_projectdir = $(container_dobuilddir)/workspace/src
container_outdir = $(container_dobuilddir)/workspace/out
container_extdir = $(container_dobuilddir)/workspace/extension
container_destdir = $(if $(DESTDIR),$(container_dobuilddir)/workspace/stage)

docker_portable_workspace = $(call not,$(HOST_CONTAINER))

ifeq ($(docker_portable_workspace),)
  container_projectdir = $(abspath $(PROJECTDIR))
  container_dobuilddir = $(abspath $(DOBUILDDIR))
  container_outdir = $(abspath $(OUTDIR))
  container_destdir = $(abspath $(DESTDIR))
endif

container_cpus = $(call min,$(INTERNPARALLEL) $(nproc))
container_cpuperiod = 100000
container_quota = $(call bc,($(container_cpus)*$(container_cpuperiod)))
container_nproc = $(INTERNPARALLEL)

docker_runflags += $(DOCKER_RUNFLAGS)
docker_runflags += $(addprefix --cpus ,$(container_cpus))
docker_runflags += --env SOURCE_DATE_EPOCH
docker_runflags += --env BUILDTIME
docker_runflags += --env DOBUILD_VERBOSE
docker_runflags += --env DOBUILD_BUILDVERBOSE
docker_runflags += --env DOBUILD_TESTVERBOSE
docker_runflags += --env DOBUILD_NPROC$(addprefix $(=),$(container_nproc))

ifeq ($(HOST_CONTAINER),)
  docker_runflags += --volume '$(realpath $(PROJECTDIR)):$(container_projectdir):cached'
  docker_runflags += --volume '$(realpath $(DOBUILDDIR)):$(container_dobuilddir):cached'
  docker_runflags += --volume '$(realpath $(OUTDIR)):$(container_outdir):delegated'
  ifneq ($(container_destdir),)
    ASSERTIONS += $(call assert,$(realpath $(DESTDIR)),Staging directory DESTDIR='$(DESTDIR)' doesn't exist)
    docker_runflags += --volume '$(realpath $(DESTDIR)):$(container_destdir):delegated'
  endif
endif

image_buildargs += $(IMAGE_BUILDARGS)
image_buildargs += 'PARALLELMFLAGS=$(addprefix -j,$(container_nproc))'
image_buildargs += 'REGISTRY_PREFIX=$(REGISTRY_PREFIX)'

docker_buildflags += $(DOCKER_BUILDFLAGS)
docker_buildflags += $(addprefix --cpu-period ,$(container_cpuperiod))
docker_buildflags += $(addprefix --cpu-quota ,$(container_quota))
docker_buildflags += $(addprefix --build-arg ,$(image_buildargs))

docker_defaulttarget = $(if $(SKIP_DEFAULTTARGET),,$(DEFAULTTARGET))
docker_targets = $(filter $(docker_defaulttarget),$(DOCKER_TARGETS))
docker_selected_targets = $(if $(SKIP_DEFAULTTARGET),$(DOCKER_TARGETS),$(docker_targets))
docker_active_targets = $(call memorize,docker_active_targets,$(call target_filter,$(FILTER),$(docker_selected_targets),$(EXCLUDEFILTER)))

docker_build_targets = $(call image_idfile,$(docker_active_targets))
docker_run_targets = $(addprefix run/container/,$(docker_active_targets))
docker_clean_targets = $(addprefix clean/image/,$(docker_active_targets))
docker_dist_targets = 
docker_distclean_targets = $(addprefix distclean/image/,$(docker_active_targets))
docker_rule_targets = $(addsuffix /dockerrules.mk,$(OUTDIR))
docker_outdirs = $(addprefix $(OUTDIR)/,$(docker_active_targets))

project_targets += $(DOCKER_TARGETS)

BUILD_TARGETS += $(docker_build_targets)
DIST_TARGETS += $(docker_dist_targets)
DISTCLEAN_TARGETS += $(docker_distclean_targets)
CLEAN_TARGETS += $(docker_clean_targets)
RUN_TARGETS += $(docker_run_targets)
RULE_TARGETS += $(docker_rule_targets)
TARGETS += $(docker_active_targets)
OUTDIRS += $(docker_outdirs)

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += $(DOCKER)
MAKEFILE_DEPS += cat
MAKEFILE_DEPS += chmod
MAKEFILE_DEPS += rm
MAKEFILE_DEPS += mkdir

#######################################################################################################################
# Docker rules

$(docker_rule_targets):
	$(SILENT) \
	{ \
	  echo '$(\#) generated file - do not edit!!!'; \
	  echo; \
	  ID='$(call id,$@)'; \
	  echo "ifndef $${ID}_include_guard"; \
	  echo "$${ID}_include_guard := 1"; \
	  echo; \
	$(foreach target,$(DOCKER_TARGETS),\
	  echo '$(\#)$(\#) BEGIN of docker $(target) configuration'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) defaults'; \
	  echo '$(target) ?= $$(call memorize,$(target),$(call target_properties_parse,$(target)))'; \
	  echo '$(target).env_path = $$(call env_default_path,$$1)'; \
	  echo '$(target).adapter ?= '; \
	  echo '$(target).extdir ?= '; \
	  echo '$(target).dockerfile ?= $$(call dobuild_dockerfile,$$1)'; \
	  echo '$(target).image_context ?= $$(call dobuild_image_context,$$1)'; \
	  echo '$(target).image_archivefile ?= $$(call dobuild_image_archivefile,$$1)'; \
	  echo '$(target).image_prerequisites ?= $$(call dobuild_image_prerequisites,$$1)'; \
	  echo '$(target).image_tags ?= $$(call dobuild_image_names,$$1)'; \
	  echo '$(target).image_buildargs ?= $$(call dobuild_image_buildargs,$$1)'; \
	  echo '$(target).image_name = $$(call image_id,$$(cache.$(target).image_idfile))'; \
	  echo '$(target).container_workdir = $$(call container_default_workdir,$$1)'; \
	  echo '$(target).container_extdir = $$(call container_default_extdir,$$1)'; \
	  echo '$(target).container_user ?= $$(call dobuild_container_user,$$1)'; \
	  echo '$(target).container_group ?= $$(call dobuild_container_group,$$1)'; \
	  echo '$(target).container_cmd ?= $$(call dobuild_container_cmd,$$1)'; \
	  echo '$(target).docker_buildflags ?= $$(call dobuild_docker_buildflags,$$1)'; \
	  echo '$(target).docker_runflags ?= $$(call dobuild_docker_runflags,$$1)'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) cached values'; \
	  echo 'cache.$(target).image_idfile = $$(call memorize,cache.$(target).image_idfile,$$(call image_idfile,$(target)))'; \
	  echo 'cache.$(target).docker_idfile = $$(call memorize,cache.$(target).docker_idfile,$$(call docker_default_idfile,$(target)))'; \
	  echo 'cache.$(target).env_path = $$(call memorize,cache.$(target).env_path,$$(call $(target).env_path,$(target)))'; \
	  echo 'cache.$(target).adapter = $$(call memorize,cache.$(target).adapter,$$(call target_get_and_subst,$(target),adapter,))'; \
	  echo 'cache.$(target).extdir = $$(call memorize,cache.$(target).extdir,$$(call target_get_and_subst,$(target),extdir,$$(EXTDIR)))'; \
	  echo 'cache.$(target).dockerfile = $$(call memorize,cache.$(target).dockerfile,$$(call target_get_and_subst,$(target),dockerfile,$$(call image_default_dockerfile,$(target))))'; \
	  echo 'cache.$(target).image_context = $$(call memorize,cache.$(target).image_context,$$(call target_get_and_subst,$(target),image_context,$$(OUTDIR)))'; \
	  echo 'cache.$(target).image_archivefile = $$(call memorize,cache.$(target).image_archivefile,$$(call target_get_and_subst,$(target),image_archivefile,))'; \
	  echo 'cache.$(target).image_prerequisites = $$(call memorize,cache.$(target).image_prerequisites,$$(call target_get_and_subst,$(target),image_prerequisites,) $$(call image_default_prerequisites,$(target)))'; \
	  echo 'cache.$(target).image_tags = $$(call memorize,cache.$(target).image_tags,$$(call target_get_and_subst,$(target),image_tags,$$(call image_default_name,$(target),$$(VERSION),$$(REGISTRY_PREFIX))))'; \
	  echo 'cache.$(target).image_buildargs = $$(call memorize,cache.$(target).image_buildargs,$$(call target_get_and_subst,$(target),image_buildargs,))'; \
	  echo 'cache.$(target).image_name = $$(call memorize,cache.$(target).image_name,$$(call $(target).image_name,$(target)))'; \
	  echo 'cache.$(target).container_workdir = $$(call memorize,cache.$(target).container_workdir,$$(call $(target).container_workdir,$(target)))'; \
	  echo 'cache.$(target).container_extdir = $$(call memorize,cache.$(target).container_extdir,$$(call $(target).container_extdir,$(target)))'; \
	  echo 'cache.$(target).container_user = $$(call memorize,cache.$(target).container_user,$$(or $$(call $(target).container_user,$(target)),$$(CONTAINER_USER)))'; \
	  echo 'cache.$(target).container_group = $$(call memorize,cache.$(target).container_group,$$(or $$(call $(target).container_group,$(target)),$$(CONTAINER_GROUP)))'; \
	  echo 'cache.$(target).container_cmd = $$(call memorize,cache.$(target).container_cmd,$$(or $$(call $(target).container_cmd,$(target)),$$(CONTAINER_CMD)))'; \
	  echo 'cache.$(target).docker_buildflags = $$(call memorize,cache.$(target).docker_buildflags,$$(call target_get_and_subst,$(target),docker_buildflags,) $$(call image_default_buildflags,$(target)))'; \
	  echo 'cache.$(target).docker_runflags = $$(call memorize,cache.$(target).docker_runflags,$$(call $(target).docker_runflags,$(target)) $$(call docker_default_runflags,$(target)))'; \
	  echo; \
	  echo '$(\#)$(\#)$(\#) rules'; \
	  echo '$(call image_build_rule,$(target))'; \
	  echo; \
	  echo '$(call image_tag_rule,$(target))'; \
	  echo; \
	  echo '$(call image_save_rule,$(target))'; \
	  echo; \
	  echo '$(call image_clean_rule,$(target))'; \
	  echo; \
	  echo '$(call image_distclean_rule,$(target))'; \
	  echo; \
	  echo '$(call container_run_rule,$(target))'; \
	  echo; \
	  echo '$(\#)$(\#) END of docker $(target) configuration'; \
	  echo; \
	) \
	  echo 'endif'; \
	  echo; \
	} > $@

#######################################################################################################################
# Docker assertions

ASSERTIONS += $(call assert,$(DOCKER),Value of variable DOCKER should not be empty)

EXPECTATIONS += $(call expect,$(call docker_version_ge,$(docker_version_req)),Using old docker version=$(docker_version)$(,) \
consider upgrading to a newer version >= $(docker_version_req))

endif

