#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

ifndef standardrules_include_guard
standardrules_include_guard := 1

.PHONY: all
all:

current_makefile := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
include $(patsubst %/,%,$(dir $(current_makefile)))/generic.mk

#######################################################################################################################
# Standard macros

# md5sum command
# $(call md5sum,file,checksum)
md5sum = { \
  if [ -n "$2" ]; then \
    sed -e 's!%MD5%!$2!g' -e 's!%FILE%!$(call escape,$1,!)!g' $(DOBUILDDIR)/assets/templates/md5sum.txt.template > '$1.md5'; \
    md5sum -c '$1.md5'; \
  else \
    echo 'warning:$1: no md5 skipping verification' 1>&2; \
  fi; \
}

# curl command
# $(call curl,file,url)
$(SKIP_CURL)curl = { \
     $(call echo_if_silent_cmd,curl -fSL $(CURLFLAGS) -o '$1' '$2') \
  && curl -fSL $(CURLFLAGS) -o '$1' '$2' \
  && $(call md5sum,$1,$3); \
}
curl ?= echo 'warning:$1: download skipped (SKIP_CURL=$(SKIP_CURL))'

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += rm
MAKEFILE_DEPS += mkdir
MAKEFILE_DEPS += cat
$(SKIP_MD5SUM)MAKEFILE_DEPS += md5sum
$(SKIP_CURL)MAKEFILE_DEPS += curl

#######################################################################################################################
# Standard rule target configuration

OUTDIRS := $(sort $(OUTDIRS))

standardrules_unsatisfied_deps = $(call filter_not_found,$(sort $(MAKEFILE_DEPS)))
standardrules_rule_targets_deps := $(filter-out $(RULE_TARGETS) $(addprefix $(PROJECTDIR),$(RULE_TARGETS)),$(MAKEFILE_LIST))

-include $(RULE_TARGETS)

JOBS = $(call memorize,JOBS,$(words $(or $(sort $(TARGETS)),_)))

# uses golden cut to manage relation between intern and external parallelization (az = phi^z)
# phi^-3 ~= 0,236, phi^-2 ~= 0,382, phi^-1 ~= 0,618
EXTERNPARALLEL = $(call memorize,EXTERNPARALLEL,$(call min,$(call bc,((($(JOBSLOTS) - 1) * 382) / 1000) + 1) $(JOBS)))
INTERNPARALLEL = $(call memorize,INTERNPARALLEL,$(call bc,($(JOBSLOTS) - 1) / $(call max,$(EXTERNPARALLEL) 1) + 1))

ifeq ($(filter 1,$(EXTERNPARALLEL)),)
  $(SKIP_EXTERNSYNC)GNUMAKEFLAGS += --output-sync
  MAKEFLAGS += $(addprefix -j,$(EXTERNPARALLEL))
endif

#######################################################################################################################
# Standard rule assertions

ASSERTIONS += $(call assert,$(call not,$(standardrules_unsatisfied_deps)),Required commands $(standardrules_unsatisfied_deps) \
not found; install appropriate packages e.g. docker-ce$(,) busybox and curl)

# evaluate only once, while make restarts because of generated makefiles
ifeq ($(MAKE_RESTARTS),)
  ASSERTIONS := $(ASSERTIONS)
  EXPECTATIONS := $(EXPECTATIONS)
endif

#######################################################################################################################
# Shell exports

export DOBUILD_HOSTCONTAINER := $(HOST_CONTAINER)
export DOBUILD_VERBOSE := $(VERBOSE)
export DOBUILD_BUILDVERBOSE := $(BUILDVERBOSE)
export DOBUILD_TESTVERBOSE := $(TESTVERBOSE)
export DOBUILD_NPROC := $(INTERNPARALLEL)
export SOURCE_DATE_EPOCH
export BUILDTIME
export DOCKER
export DOCKER_BUILDKIT
export BUILDKIT_PROGRESS

#######################################################################################################################
# Standard rules

.PHONY: all
all: $(BUILD_TARGETS);

.PHONY: check
check: $(CHECK_TARGETS);

.PHONY: memcheck
memcheck: $(MEMCHECK_TARGETS);

.PHONY: lint
lint: $(LINT_TARGETS);

.PHONY: run
run: $(firstword $(RUN_TARGETS));

.PHONY: clean
clean: $(CLEAN_TARGETS);

.PHONY: prepare
prepare: $(PREPARE_TARGETS);

.PHONY: dist
dist: $(DIST_TARGETS);

.PHONY: distclean
distclean: $(DISTCLEAN_TARGETS)
	$(SILENT)-$(if $(strip $(OUTDIRS)),rm -rf $(OUTDIRS))

.PHONY: install
install: $(INSTALL_TARGETS);

.PHONY: print-targets
print-targets:
	$(SILENT)MAKE=$(MAKE) $(DOBUILDDIR)/bin/parse_make_targets.sh $(MFLAGS) $(addprefix -f ,$(MAKEFILE_LIST)) $(MAKEOVERRIDES)

.PHONY: debug-print-%
debug-print-%:
	@printf '%s\n' '$*:' 1>&2; \
	 printf '%s\n' $($*)

$(RULE_TARGETS): $(standardrules_rule_targets_deps) | $(OUTDIRS)

$(OUTDIRS):
	$(SILENT)mkdir -p $@

endif
