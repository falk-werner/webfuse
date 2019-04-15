CURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST))

#######################################################################################################################
# Overridable docker defaults

DOCKER ?= docker
DOCKER_BUILDKIT ?= 

CONTAINER_CGROUP_PARENT ?= 
CONTAINER_USER ?= $(USERID)
CONTAINER_GROUP ?= $(USERID)

ifndef _INCLUDE_DEFAULTS
include $(patsubst %/,%,$(dir $(CURRENT_MAKEFILE)))/defaults.mk
endif

#######################################################################################################################
# Docker macros

image_run_volumes += $(addprefix --volumes-from ,$2)

image_name = $(REGISTRY_PREFIX)$(subst -,/,$1)/$(PROJECTNAME)$(addprefix :,$(VERSION))

image_run = $(DOCKER) run --rm --interactive $(DOCKER_RUNFLAGS) \
  $(call image_run_volumes,$1,$(HOST_CONTAINER)) \
  $(addprefix --cgroup-parent ,$(CONTAINER_CGROUP_PARENT)) \
  --workdir '$(CONTAINER_OUTDIR)/$1/$(BUILDTYPE)' \
  $3 \
  $(call image_name,$1) \
  $2

image_rule = \
  $$(OUTDIR)/docker/$1: $$(OUTDIR)/docker/$1.dockerfile $$(EXTRACT_TARGETS) $$(MAKEFILE_LIST); \
    $$(SILENT)$$(call image,$1)
image = \
     $(call echo_if_silent,TARGET=$1 docker build --file $< --tag $(call image_name,$1) $(OUTDIR)) \
  && $(DOCKER) build --rm $(DOCKER_BUILDFLAGS) --iidfile $@ --file $< --tag $(call image_name,$1) $(OUTDIR)

run_rule = \
  run-$1: $$(OUTDIR)/docker/$1; \
    $$(SILENT)$$(call run,$1,bash,--tty) || true
run = $(call echo_if_silent,TARGET=$1 $(addprefix BUILDTYPE=,$(BUILDTYPE)) $2) && $(call image_run,$1,$2,$3)

wrapper_rule = \
  $$(OUTDIR)/$1/$$(BUILDTYPE)/gdbserver: $$(SCRIPTDIR)/resources/run_image.sh.template $$(OUTDIR)/docker/$1; \
    $$(SILENT)$$(call wrapper,$1)
wrapper = \
     $(call echo_if_silent,generating $@) \
  && sed \
       -e 's@%PROJECTDIR%@$(abspath $(PROJECTDIR))@g' \
       -e 's@%SCRIPTDIR%@$(abspath $(SCRIPTDIR))@g' \
       -e 's@%DOCKER%@$(DOCKER)@g' \
       -e 's@%RUNFLAGS%@$(DOCKER_RUNFLAGS) $(call image_run_volumes,$1)@g' \
       -e 's@%IMAGE%@$(call image_name,$1)@g' \
       -e 's@%RUNCMD%@$(notdir $@)@g' \
       $< > $@ \
  && chmod +x $@

#######################################################################################################################
# Docker rule target configuration

DOCKER_RUNFLAGS += --env SOURCE_DATE_EPOCH
DOCKER_RUNFLAGS += --env BUILDTIME
DOCKER_RUNFLAGS += --user '$(CONTAINER_USER):$(CONTAINER_GROUP)'

DOCKER_BUILDARGS += 'USERID=$(USERID)'
DOCKER_BUILDARGS += 'PARALLELMFLAGS=$(_PARALLELMFLAGS)'
DOCKER_BUILDARGS += 'PROJECTDIR=$(CONTAINER_PROJECTDIR)'
DOCKER_BUILDARGS += 'SCRIPTDIR=$(CONTAINER_SCRIPTDIR)'
DOCKER_BUILDARGS += 'OUTDIR=$(CONTAINER_OUTDIR)'
DOCKER_BUILDARGS += 'REGISTRY_PREFIX=$(REGISTRY_PREFIX)'

DOCKER_BUILDFLAGS += $(addprefix --build-arg ,$(DOCKER_BUILDARGS))

OUTDIRS += $(OUTDIR)/docker

$(HOST_CONTAINER)image_run_volumes += --volume '$(realpath $(PROJECTDIR)):$(CONTAINER_PROJECTDIR):cached'
$(HOST_CONTAINER)image_run_volumes += --volume '$(realpath $(SCRIPTDIR)):$(CONTAINER_SCRIPTDIR):cached'
$(HOST_CONTAINER)image_run_volumes += --volume '$(realpath $(OUTDIR)/$1):$(CONTAINER_OUTDIR)/$1:delegated'

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += $(DOCKER)
MAKEFILE_DEPS += sed
MAKEFILE_DEPS += chmod

