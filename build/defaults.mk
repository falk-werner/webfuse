CURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST)))
_INCLUDE_DEFAULTS := T

#######################################################################################################################
# Overridable common defaults

# NOTE: default assumes first loaded makefile is located in root directory of project
MAKEFILE ?= $(firstword $(MAKEFILE_LIST))
MAKEFILE := $(MAKEFILE)

PROJECTDIR ?= $(patsubst %/,%,$(dir $(MAKEFILE)))
SCRIPTDIR ?= $(patsubst %/,%,$(dir $(CURRENT_MAKEFILE)))
OUTDIR ?= $(PROJECTDIR)/.build

BUILDTYPE ?= Debug

BUILDVERBOSE ?= 
BUILDSILENT := $(if $(BUILDVERBOSE),,1)

VERSION ?= $(shell cat $(PROJECTDIR)/VERSION)
VERSION := $(VERSION)

USERID ?= $(shell id -u)
USERID := $(USERID)

HOST_MARCH ?= $(shell dpkg --print-architecture 2>/dev/null)
HOST_CONTAINER ?= $(shell $(SCRIPTDIR)/get_container_id.sh)
HOST_CONTAINER := $(HOST_CONTAINER)

$(HOST_CONTAINER)PORTABLE_WORSPACE ?= 
CONTAINER_PROJECTDIR ?= /workspace/src
CONTAINER_SCRIPTDIR ?= /workspace/bin
CONTAINER_OUTDIR ?= /workspace/out
$(PORTABLE_WORSPACE)CONTAINER_PROJECTDIR = $(abspath $(PROJECTDIR))
$(PORTABLE_WORSPACE)CONTAINER_SCRIPTDIR = $(abspath $(SCRIPTDIR))
$(PORTABLE_WORSPACE)CONTAINER_OUTDIR = $(abspath $(OUTDIR))

SOURCE_DATE_EPOCH ?= $(shell $(SCRIPTDIR)/get_source_date_epoch.sh)
SOURCE_DATE_EPOCH := $(SOURCE_DATE_EPOCH)
export SOURCE_DATE_EPOCH

BUILDTIME ?= $(shell date -u -d '@$(SOURCE_DATE_EPOCH)' --rfc-3339 ns 2>/dev/null | sed -e 's/ /T/')
BUILDTIME := $(BUILDTIME)
export BUILDTIME

VERBOSE ?= 
PARALLELMFLAGS ?= 
NPROC ?= $(shell echo '$(PARALLELMFLAGS)' | sed -n -e 's@.*-j.*\([0-9]\+\)@\1@p')

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += cat
MAKEFILE_DEPS += id
MAKEFILE_DEPS += echo
MAKEFILE_DEPS += sed

