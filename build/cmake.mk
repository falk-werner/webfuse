CURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST)))

#######################################################################################################################
# Overridable CMake defaults

filter_targets ?= $2

CMAKE_BUILD_TYPE ?= $(BUILDTYPE)
CMAKE_MEMCHECK_FILTER ?= $(MEMCHECK_FILTER)
CMAKE_PROJECTDIR ?= $(CONTAINER_PROJECTDIR)
CMAKE_SCRIPTDIR ?= $(CONTAINER_SCRIPTDIR)

ifndef _INCLUDE_DEFAULTS
include $(patsubst %/,%,$(dir $(CURRENT_MAKEFILE)))/defaults.mk
endif

#######################################################################################################################
# CMake macros

cmake_configure_rule = \
  $$(OUTDIR)/$1/$$(CMAKE_BUILD_TYPE)/CMakeCache.txt: $$(PROJECTDIR)/CMakeLists.txt $$(OUTDIR)/docker/$1 | $$(OUTDIR)/$1/$$(CMAKE_BUILD_TYPE)/gdbserver; \
    $$(SILENT)$$(call cmake_configure,$1)
cmake_configure = \
     $(call run,$1,sh -c 'cmake $(CMAKEFLAGS) $(CMAKE_PROJECTDIR) && $(CMAKE_SCRIPTDIR)/cmake_discover_cc_settings.sh $(notdir $@) "$(realpath $(dir $@))"') \
  && touch $(addprefix $(dir $@)/,include_dirs.txt) $@

cmake_discover_cc_settings_rule = \
  $$(OUTDIR)/$1/$$(CMAKE_BUILD_TYPE)/include_dirs.txt: $$(OUTDIR)/$1/$$(CMAKE_BUILD_TYPE)/CMakeCache.txt; \
    $$(SILENT)$$(call cmake_discover_cc_settings,$1)
cmake_discover_cc_settings = \
  $(call run,$1,$(CMAKE_SCRIPTDIR)/cmake_discover_cc_settings.sh $(notdir $<) '$(realpath $(dir $<))')

ninja_build_rule = \
  build-$1: $$(OUTDIR)/$1/$$(CMAKE_BUILD_TYPE)/CMakeCache.txt; \
    $$(SILENT)$$(call ninja_build,$1)
ninja_build = $(call run,$1,ninja $(_PARALLELMFLAGS) $(NINJAFLAGS) $(GOALS))

ctest_memcheck_rule = \
  memcheck-$1: build-$1; \
    $$(SILENT)$$(call ctest_memcheck,$1)
ctest_memcheck = $(call run,$1,ctest -T memcheck $(CTESTFLAGS))

#######################################################################################################################
# CMake rule target configuration

$(BUILDSILENT)NINJAFLAGS += -v

DOCKER_RUNFLAGS += --env NINJA_STATUS

CMAKEFLAGS += '-GNinja'
CMAKEFLAGS += '-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)'

CMAKE_TARGET := $(filter $(BUILDTARGET),$(CMAKE_TARGETS))
$(CMAKE_TARGET)CMAKE_ACTIVE_TARGETS += $(CMAKE_TARGETS)
CMAKE_ACTIVE_TARGETS += $(CMAKE_TARGET)
CMAKE_TARGETS := $(call filter_targets,$(FILTER),$(CMAKE_ACTIVE_TARGETS))

CMAKE_BUILD_TARGETS += $(addprefix build-,$(CMAKE_TARGETS))
CMAKE_CLEAN_TARGETS += $(addprefix clean-,$(CMAKE_TARGETS))
CMAKE_CHECK_TARGETS += $(addprefix check-,$(CMAKE_TARGETS))
CMAKE_MEMCHECK_TARGETS += $(addprefix memcheck-,$(call filter_targets,$(CMAKE_MEMCHECK_FILTER),$(CMAKE_TARGETS)))
CMAKE_RUN_TARGETS += $(addprefix run-,$(firstword $(CMAKE_TARGETS)))
CMAKE_DISCOVER_CC_TARGETS += $(addprefix discover-cc-,$(firstword $(CMAKE_TARGETS)))
CMAKE_OUTDIRS += $(addsuffix /$(CMAKE_BUILD_TYPE),$(addprefix $(OUTDIR)/,$(CMAKE_TARGETS)))
CMAKE_RULE_TARGETS += $(addsuffix /cmakerules.mk,$(addprefix $(OUTDIR)/,$(CMAKE_TARGETS)))

BUILD_TARGETS += $(CMAKE_BUILD_TARGETS)
CHECK_TARGETS += $(CMAKE_CHECK_TARGETS)
MEMCHECK_TARGETS += $(CMAKE_MEMCHECK_TARGETS)
CLEAN_TARGETS += $(CMAKE_CLEAN_TARGETS)
RUN_TARGETS += $(CMAKE_RUN_TARGETS)
DISCOVER_CC_TARGETS += $(CMAKE_DISCOVER_CC_TARGETS)
RULE_TARGETS += $(CMAKE_RULE_TARGETS)
TARGETS += $(CMAKE_TARGETS)
OUTDIRS += $(CMAKE_OUTDIRS)

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += touch
MAKEFILE_DEPS += echo

#######################################################################################################################
# CMake rules

$(CMAKE_RULE_TARGETS):
	$(SILENT) \
	{ \
	  echo '$(call image_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call cmake_configure_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call ninja_build_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call check_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call ctest_memcheck_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call run_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call clean_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call cmake_discover_cc_settings_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call discover_cc_rule,$(TARGET))'; \
	  echo; \
	  echo '$(call wrapper_rule,$(TARGET))'; \
	} > $@

