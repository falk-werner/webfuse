
#######################################################################################################################
# Overridable defaults

ifndef _INCLUDE_DEFAULTS
include $(patsubst %/,%,$(dir $(CURRENT_MAKEFILE)))/defaults.mk
endif

#######################################################################################################################
# Standard macros

echo_if_silent = VERBOSE=1
$(VERBOSE)echo_if_silent = echo $1
$(VERBOSE)SILENT := @

filter_out_command ?= $(filter $1,$(foreach CMD,$1,$(shell command -v $(CMD) 2>&1 1>/dev/null || echo $(CMD))))

md5sum = { \
  if [ -n "$2" ]; then \
    sed -e 's@%MD5%@$2@g' -e 's@%FILE%@$1@g' $(SCRIPTDIR)/resources/md5sum.txt.template > $1.md5; \
    md5sum -c $1.md5; \
  else \
    echo 'warning:$1: no md5 skipping verification' 1>&2; \
  fi; \
}

$(SKIP_FETCH)curl = $(call echo_if_silent,curl -fSL $(CURLFLAGS) -o $1 $2) \
  && curl -fSL $(CURLFLAGS) -o $1 $2 \
  && $(call md5sum,$1,$3)

curl ?= echo 'warning:$1: download skipped (SKIP_FETCH=$(SKIP_FETCH))'

check_rule = \
  check-$1: build-$1;

clean_rule = \
  clean-$1: ; \
    $$(SILENT)-$$(call clean,$1)
clean = rm -rf $(OUTDIR)/$1

discover_cc_rule = \
  discover-cc-$1: $$(OUTDIR)/$1/$$(BUILDTYPE)/include_dirs.txt; \
    $$(SILENT)$$(call discover_cc,$1)
discover_cc = cat $<

#######################################################################################################################
# Standard rule target configuration

OUTDIRS += $(OUTDIR)

OUTDIRS := $(sort $(OUTDIRS))
TARGETS := $(sort $(TARGETS))

TASKS := $(words $(if $(TARGETS),$(TARGETS),_))

DISBALE_OSYNC ?= $(filter 1,$(TASKS))
$(DISBALE_OSYNC)GNUMAKEFLAGS += --output-sync

_NPROC = $(NPROC)
ifeq ($(strip $(_NPROC)),)
_NPROC = $(shell nproc)
endif
_PARALLELMFLAGS := $(addprefix -j,$(shell echo "$$(($(_NPROC)/$(TASKS)))"))

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += cp
MAKEFILE_DEPS += rm
MAKEFILE_DEPS += mkdir
MAKEFILE_DEPS += echo

MAKEFILE_DEPS := $(sort $(MAKEFILE_DEPS))

UNSATISFIED_MAKEFILE_DEPS := $(call filter_out_command,$(MAKEFILE_DEPS))

RULE_TARGETS_DEPS := $(filter-out $(RULE_TARGETS) $(addprefix $(PROJECTDIR),$(RULE_TARGETS)),$(MAKEFILE_LIST))

#######################################################################################################################
# Standard rules

ifneq ($(MAKECMDGOALS),distclean)
-include $(RULE_TARGETS)
endif

.PHONY: all build-%
all: $(BUILD_TARGETS);

.PHONY: check check-%
check: $(CHECK_TARGETS);

.PHONY: memcheck memcheck-%
memcheck: $(MEMCHECK_TARGETS);

.PHONY: lint lint-%
lint: $(LINT_TARGETS);

.PHONY: run run-%
run: $(RUN_TARGETS);

.PHONY: clean clean-%
clean: $(CLEAN_TARGETS);

.PHONY: get-deps get-deps-%
get-deps: $(EXTRACT_TARGETS);

.PHONY: discover-cc discover-cc-%
discover-cc: $(DISCOVER_CC_TARGETS);

.PHONY: distclean
distclean:
	$(SILENT)-rm -rf $(OUTDIRS)

.PHONY: debug-print-%
debug-print-%:
	@printf '%s\n' '$*:' $($*)

$(RULE_TARGETS): $(RULE_TARGETS_DEPS) | $(UNSATISFIED_MAKEFILE_DEPS) $(OUTDIRS)

$(OUTDIRS):
	$(SILENT)mkdir -p $@

$(UNSATISFIED_MAKEFILE_DEPS):
	$(error 'required commands $(UNSATISFIED_MAKEFILE_DEPS) not found; install appropriate packages e.g. docker-ce, busybox and curl')
