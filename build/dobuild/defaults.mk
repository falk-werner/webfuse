#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of dobuild.
# Copyright (c) 2019 Contributors as noted in the AUTHORS file.
#
# SPDX-License-Identifier: MPL-2.0

ifndef defaults_include_guard
defaults_include_guard := 1

current_makefile := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

#######################################################################################################################
# Overridable common defaults

# NOTE: default assumes first loaded makefile is located in root directory of project
MAKEFILE ?= $(firstword $(MAKEFILE_LIST))
MAKEFILE := $(MAKEFILE)

DOBUILD_TOPDIR ?= $(PROJECTDIR)
DOBUILD_PROJECTDIR ?= $(patsubst %/,%,$(dir $(MAKEFILE)))
DOBUILD_PROJECTNAME ?= $(notdir $(shell cd $(PROJECTDIR) && pwd))
DOBUILD_PROJECTVERSIONFILE ?= $(PROJECTDIR)/VERSION
DOBUILD_PROJECTVERSION ?= $(shell cat '$(VERSIONFILE)' 2>/dev/null)
DOBUILD_OUTDIR ?= $(BUILDDIR)/.build
DOBUILD_BUILDDIR ?= $(PROJECTDIR)
DOBUILD_BUILDVERBOSE ?=
DOBUILD_TESTVERBOSE ?=
DOBUILD_BUILDTESTING ?= 1
DOBUILD_USERID ?= $(shell id -u)
DOBUILD_HOSTCONTAINER ?= $(shell $(DOBUILDDIR)/bin/get_container_id.sh)
DOBUILD_JOBSLOTS ?= $(call jobslots,$(make_cmdline))
DOBUILD_SKIPMD5SUM ?= $(call filter_not_found,md5sum)
DOBUILD_SKIPCURL ?= $(call filter_not_found,curl)
DOBUILD_SKIPDEFAULTTARGET ?= $(or $(HOSTMARCH),$(MARCH),$(DISTRIB_ID),$(DISTRIB_VERSION),$(SYS),$(ABI),$(ID),$(VARIANT),$(call not,$(DEFAULTTARGET)))
DOBUILD_SKIPEXTERNSYNC ?= 
DOBUILD_FILTER ?= $(call target_properties_combine, \
    $(or $(HOSTMARCH),%), \
    $(or $(MARCH),%), \
    $(or $(DISTRIB_ID),%), \
    $(or $(SYS),%), \
    $(or $(ABI),%), \
    $(or $(ID),%), \
    $(or $(DISTRIB_VERSION),%), \
    $(or $(VARIANT),%) \
  )
DOBUILD_EXCLUDEFILTER ?= $(if $(strip \
      $(or $(EXCLUDE_HOSTMARCH),$(EXCLUDE_MARCH),$(EXCLUDE_DISTRIB_ID),$(EXCLUDE_SYS),$(EXCLUDE_ABI),$(EXCLUDE_ID),$(EXCLUDE_DISTRIB_VERSION),$(EXCLUDE_VARIANT)) \
    ), \
    $(call target_properties_combine, \
      $(or $(EXCLUDE_HOSTMARCH),%), \
      $(or $(EXCLUDE_MARCH),%), \
      $(or $(EXCLUDE_DISTRIB_ID),%), \
      $(or $(EXCLUDE_SYS),%), \
      $(or $(EXCLUDE_ABI),%), \
      $(or $(EXCLUDE_ID),%), \
      $(or $(EXCLUDE_DISTRIB_VERSION),%), \
      $(or $(EXCLUDE_VARIANT),%)\
    ) \
  )
DOBUILD_FILTERMEMCHECK ?= $(call target_properties_format_args,$(or $(machine),%),$(or $(machine),%),%,%,%,%,%,%)
DOBUILD_CGROUPPARENT ?=
DOBUILD_COLOR ?= $(shell { command -v tput >/dev/null 2>&1 && test "$$(tput $(addprefix -T,$(TERM)) colors)" -ge 8 2>/dev/null; } && echo '1')
DOBUILD_EXTDIR ?= $(PROJECTDIR)/dobuild-extensions

BUILDKIT_PROGRESS ?= plain
DOCKER_BUILDKIT ?= 1
DESTDIR ?=
TERM ?= linux
REGISTRY_PREFIX ?=
VERBOSE ?=

COLOR = $(call memorize,COLOR,$(DOBUILD_COLOR))

PREPARE_TARGETS = $(EXTRACT_TARGETS)
RULE_TARGETS = 
BUILD_TARGETS = 
CHECK_TARGETS = 
MEMCHECK_TARGETS = 
LINT_TARGETS = 
RUN_TARGETS = 
CLEAN_TARGETS = 
EXTRACT_TARGETS = 
DIST_TARGETS = 
DISTCLEAN_TARGETS = 
INSTALL_TARGETS = 
TARGETS = 
OUTDIRS = 
BUILDTARGET = $(warning: deprecated use DEFAULTTARGET instead)

DOCKER_RUNFLAGS = 
DOCKER_BUILDFLAGS = 
IMAGE_BUILDARGS = 

CURLFLAGS = 

MAKEFILE_DEPS =

#######################################################################################################################
# Common macros

# caches a value lazily - at it's first evaluation
# $(call memorize,VAR,value...)
memorize = $(eval $1 := $$2)$2

# asserts value is not empty, evaluates to value or reports an error message on failure and aborts build
# $(call assert,value,message)
assert = $(or $1,$(error assertion failed: $2))

# asserts value is not empty, evaluates to value or reports a warning message on failure and continues build
# $(call expect,value,message)
expect = $(or $1,$(warning expectation failed: $2))

# asserts value is of scalar type, evaluates to value or reports an error message on failure
# $(call assert_scalar,value...,message)
assert_scalar = $(if $(call eq_s,1,$(words $1)),$1,$(error assertion failed: $2))

# filters out all commands found on host, resulting in a list of unavailable commands
# $(call filter_not_found,command...)
filter_not_found = $(filter $1,$(foreach cmd,$1,$(shell command -v $(cmd) >/dev/null 2>&1 || echo $(cmd))))

# escapes a character in string
# $(call escape,string,char)
escape = $(subst $2,$(\\)$2,$1)

# filters all elements matching any of the given regular expressions
# $(call filter_by_regex,regex...,element...)
filter_by_regex = $(shell printf '%s\n' $2 | sed -n$(foreach regex,$1, -e 's!$(call escape,$(regex),!)!\0!p'))

# parses the target properties [host-arch arch_sub vendor_or_distrib_id sys abi id distrib_version variant] for given name
# $(call target_properties_parse,target-name)
target_properties_parse = $(shell $(DOBUILDDIR)/bin/parse_target_properties.sh $1)

# retrieves the target properties
# $(call target_properties,target-name)
target_properties = $(or $($1),$(call target_properties_parse,$1))

# formats a list of target properties as string
# $(call target_properties_format,property...)
target_properties_format = $(call target_properties_format_args, \
    $(call target_property_host_arch,$1), \
    $(call target_property_arch_sub,$1), \
    $(call target_property_vendor_or_distrib_id,$1), \
    $(call target_property_sys,$1), \
    $(call target_property_abi,$1), \
    $(call target_property_id,$1), \
    $(call target_property_distrib_version,$1), \
    $(call target_property_variant,$1) \
  )

# formats target name by its properties as separated arguments - each may be empty
# $(call target_properties_format_args,host_arch,arch_sub,distrib,sys,abi,id,distrib_version,variant)
target_properties_format_args = $(call join_s,$1 $(call join_s,$2 $(call join_s,$3 $7,@) $4 $5,-) $(call join_s,$6 $8,@),+)

# tests each target property matches associated pattern
# $(call target_properties_matches,pattern...,property...)
target_properties_matches = $(call eq,$1,$2,filter)

# retrieves the target property host-arch from the list of properties
# $(call target_property_host_arch,property...)
target_property_host_arch = $(word 1,$1)

# retrieves the target property arch_sub from the list of properties
# $(call target_property_arch_sub,property...)
target_property_arch_sub = $(word 2,$1)

# retrieves the target property vendor_or_distrib_id from the list of properties
# $(call target_property_vendor_or_distrib_id,property...)
target_property_vendor_or_distrib_id = $(word 3,$1)

# retrieves the target property sys from the list of properties
# $(call target_property_sys,property...)
target_property_sys = $(word 4,$1)

# retrieves the target property abi from the list of properties
# $(call target_property_abi,property...)
target_property_abi = $(word 5,$1)

# retrieves the target property id from the list of properties
# $(call target_property_id,property...)
target_property_id = $(word 6,$1)

# retrieves the target property distrib_version from the list of properties
# $(call target_property_distrib_version,property...)
target_property_distrib_version = $(word 7,$1)

# retrieves the target property variant from the list of properties
# $(call target_property_variant,property...)
target_property_variant = $(word 8,$1)

# retrieves the target host-arch
# $(call target_host_arch,target-name)
target_host_arch = $(call target_property_host_arch,$(call target_properties,$1))

# retrieves the target arch_sub
# $(call target_arch_sub,target-name)
target_arch_sub = $(call target_property_arch_sub,$(call target_properties,$1))

# retrieves the target vendor_or_distrib_id
# $(call target_vendor_or_distrib_id,target-name)
target_vendor_or_distrib_id = $(call target_property_vendor_or_distrib_id,$(call target_properties,$1))

# retrieves the target sys
# $(call target_sys,target-name)
target_sys = $(call target_property_sys,$(call target_properties,$1))

# retrieves the target abi
# $(call target_abi,target-name)
target_abi = $(call target_property_abi,$(call target_properties,$1))

# retrieves the target id
# $(call target_id,target-name)
target_id = $(call target_property_id,$(call target_properties,$1))

# retrieves the target id
# $(call target_variant,target-name)
target_variant = $(call target_property_variant,$(call target_properties,$1))

# retrieves the target distribution version
# $(call target_distrib_version,target-name)
target_distrib_version = $(call target_property_distrib_version,$(call target_properties,$1))

# retrieves the target distribution id and version
# $(call target_distrib,target-name)
target_distrib = $(addsuffix $(addprefix @,$(call target_distrib_version,$1)),$(call target_vendor_or_distrib_id,$1))

# retrieves the target distribution id
# $(call target_distrib_id,target-name)
target_distrib_id = $(call target_vendor_or_distrib_id,$1)

# tests target properties matches given pattern
# $(call target_matches,target-name,pattern...)
target_matches = $(call target_properties_matches,$2,$(call target_properties,$1))

# filters all targets which properties matches given patterns
# $(call target_filter_by_properties,pattern...,target-name...)
target_filter_by_properties = $(call filter_p,$2,target_matches,$1)

# filters all targets which properties matches given include-pattern and not the given exclude-pattern strings
# $(call target_filter,inlcude-pattern-string...,target-name...[,exclude-pattern-string...])
target_filter = \
  $(filter-out \
    $(foreach pattern,$3,$(call target_filter_by_properties,$(call target_properties_parse,$(pattern)),$2)),\
    $(foreach pattern,$1,$(call target_filter_by_properties,$(call target_properties_parse,$(pattern)),$2))\
  )

# replaces template values in string list with target properties
# $(call target_subst,target-name[,values...,])
target_subst = $(strip \
    $(subst %TARGET%,$1, \
      $(subst %OUTDIR%,$(OUTDIR), \
        $(subst %HOSTMARCH%,$(call target_host_arch,$1), \
          $(subst %MARCH%,$(call target_arch_sub,$1), \
            $(subst %DISTRIB%,$(call target_distrib,$1), \
              $(subst %DISTRIB_ID%,$(call target_distrib_id,$1), \
                $(subst %DISTRIB_VERSION%,$(call target_distrib_version,$1), \
                  $(subst %SYS%,$(call target_sys,$1), \
                    $(subst %ABI%,$(call target_abi,$1), \
                      $(subst %ID%,$(call target_id,$1), \
                        $(subst %VARIANT%,$(call target_variant,$1),$2) \
                      ) \
                    ) \
                  ) \
                ) \
              ) \
            ) \
          ) \
        ) \
      ) \
    ) \
  )

# replaces template values of target field with target properties
# $(call target_subst_cmd,target-name,field[,default-value])
target_get_and_subst = $(call target_subst,$1,$(or $(call $1.$2,$1),$3))

# replaces template values of target field with target properties and resolves result against given search 
# paths, returning a list of existing resolved paths
# $(call target_subst_and_resolve,target-name,field,path...)
target_subst_and_resolve = $(wildcard $(addsuffix /$(call target_get_and_subst,$1,$2,),$3))

# generates target names by combining elements of list properties
# $(call target_properties_combine,host_arch1...,arch_sub1..,distrib1...,sys1...,abi1...,id1...,distrib_version1...,variant1...) 
target_properties_combine = $(call combine,target_properties_format_args,$1,$2,$3,$4,$5,$6,$7,$8)

# tests scalar values on equality, returns the value or empty if non-equal
# $(call eq_s,value1,value2)
eq_s = $(and $(findstring $1,$2),$(findstring $2,$1))

# tests scalar values on inequality, returns non empty value if values are unequal
# $(call ne_s,value1,value2)
ne_s = $(call not,$(call eq_s,$1,$2))

# negates value, returns empty for non-empty value and vice versa 
# $(call not,value)
not = $(if $(strip $1),,1)

# compares number values, returns negative value if num1 is less than num2, 
# a positive value if num1 is greather than 2 and 0 if both are equal
# $(call compare,num1,num2)
compare = $(call bc,$1-$2)

# tests if num1 is less than num2
# $(call lt,num1,num2)
lt = $(if $(findstring -,$(call compare,$1,$2)),1)

# tests if num1 is greater than num2
# $(call gt,num1,num2)
gt = $(call not,$(or $(call lt,$1,$2),$(call eq_s,$1,$2)))

# tests if num1 is greater than or equal to num2
# $(call ge,num1,num2)
ge = $(call not,$(call lt,$1,$2))

# tests if version parts (major, minor, patch) are greater or equal to required version parts
# $(call ge_version,[major minor patch],[required-major required-minor required-patch])
ge_version = $(strip \
    $(or \
      $(call gt,$(or $(firstword $1),0),$(or $(firstword $2),0)), \
      $(if $(call eq_s,$(or $(firstword $1),0),$(or $(firstword $2),0)), \
        $(or \
          $(call gt,$(or $(word 2,$1),0),$(or $(word 2,$2),0)) \
          $(if $(call eq_s,$(or $(word 2,$1),0),$(or $(word 2,$2),0)), \
            $(call ge,$(or $(word 3,$1),0),$(or $(word 3,$2),0)) \
          ) \
        ) \
      ) \
    ) \
  )

# sorts numbers
# $(call sort_n,value...)
sort_n = $(shell LANG=C LC_ALL=C printf '%s\n' $1 | sort -n)

# caluclates minimum of numeric values
# $(call min,values...)
min = $(firstword $(call sort_n,$1))

# caluclates maximum of numeric values
# $(call max,values...)
max = $(call lastword,$(call sort_n,$1))

# caluclates the value of an arithmetic expression
# $(call bc,expr...)
bc = $(shell echo "$$(($1))")

# returns the tail of a list, removing its first element
# $(call tail,element...)
tail = $(wordlist 2,$(words $1),$1)

# returns the last element of a list (implemented for backward compatiblity with gnu make 3.8)
# $(call lastword,element...)
ifneq ($(lastword 0 1),1)
  lastword = $(if $1,$(word $(words $1),$1),)
endif

# removes whitespace characters between positional argument and its value
# $(call normalize_args,arg...,cmdline)
normalize_args = $(if $1,$(call normalize_args,$(call tail,$1),$(subst $(firstword $1)$( ),$(firstword $1),$2)),$2)

# parses the number of available job slots (-jx | --jobs X)
# $(call jobslots,mflags...)
jobslots = $(call lastword,$(shell printf '%s\n' $(call normalize_args,-j --jobs,$1) | sed -n -e 's@^\(--jobs=\?\|-j\)\([0-9]\+\)@\2@p'))

# creates a colored message
# $(call color,color-name,value...)
color = '$(tc_$1)'$2'$(tc_reset)'
$(COLOR)color = $2

# creates a command that prints a message when VERBOSE is disabled
# $(call echo_if_silent_cmd,value...)
echo_if_silent_cmd = VERBOSE=1
$(VERBOSE)echo_if_silent_cmd = { printf '$(DEFAULT_FMT) ' $1 && printf '\n'; }

# creates a command which is printed before it is executed
# $(call echo_cmd,command[,args...][,output-command])
echo_cmd = $(call echo_if_silent_cmd,$(or $3,$1) $2) && $1 $2

# creates a command which changes its working directory before it is executed
# $(call chdir_cmd,target-name,workdir,command[,args...][,output-command])
chdir_cmd = \
     $(call echo_if_silent_cmd,$(call color,green,$(addprefix TARGET=,$1) WORKDIR='$2') $(or $5,$3) $4) \
  && (cd '$2' && PATH='$(cache.$1.env_path)' $3 $4)

# overridable helper to create a command which is executed as part of a target rule
# $(call run_cmd,target-name,command[,args...][,output-command])
run_cmd = $(call chdir_cmd,$1,$(OUTDIR)/$1,$2,$3,$4)

# retrieves the host path environment variable value with optional target specific extensions
# $(call env_host_path[,target-name])
env_host_path = $(call join_s,\
    $(realpath \
      $(cache.$1.extdir) \
      $(addprefix $(DOBUILDDIR)/assets/adapters/,$(cache.$1.adapter)) \
    ) \
    $(PATH),\
    : \
  )

# overridable helper to retrieve the target default path environment variable value
# $(call env_default_path,target-name)
env_default_path = $(call env_host_path,$1)

# retrieves the last modification time of each files in seconds since epoch
# $(call mtime,file...)
mtime = $(if $1,$(shell stat -c '%Y' $1))

# joins a list to a string
# $(call join_s,element...[,separator])
join_s = $(subst $( ),$(strip $2),$(strip $1))

# splits a string to a list
# $(call split_s,element...[,separator])
split_s = $(subst $(strip $2),$( ),$(strip $1))

# identity function
# $(call identity,element...)
identity = $1

# filters all elements matching given predicate
# $(call filter_p,element...,predicate[,predicate-context])
filter_p = $(foreach elm,$1,$(if $(strip $(call $2,$(elm),$3)),$(elm),))

# tests inequality of list 1 and list 2
# $(call ne,elem1...,elem2...[,predicate])
ne = $(call not,$(call eq,$1,$2,$3))

# tests equality of list 1 and list 2
# $(call eq,elem1...,elem2...[,predicate])
eq = $(strip \
    $(if \
      $(call eq_s,$(words $1),$(words $2)), \
      $(call eq_each,$1,$2,$(or $3,eq_s)) \
    ) \
  )

# tests equality of elements in list 1 and list 2
# $(call eq_each,list1_elem...,list2_elem...,predicate)
eq_each = $(strip \
    $(if $(strip $1),\
      $(if \
        $(call $3,$(firstword $1),$(firstword $2)), \
        $(call eq_each,$(call tail,$1),$(call tail,$2),$3), \
      ), \
      1 \
    ) \
  )

# encodes string to a valid make identifier
# $(call id,string...)
id = $(subst $( ),~,$(subst $(=),_,$(call base64_encode,$1)))

# encodes string to base64
# $(call base64_encode,string...)
base64_encode = $(shell echo $1 | base64)

# decodes string from base64
# $(call base64_decode,string...)
base64_decode = $(shell echo $1 | base64 -d)

# replaces all elements which equals oldvalue with newvalue
# $(call replace_all,element...,oldvalue,newvalue)
replace_all = $(foreach elm,$1,$(if $(call eq_s,$(elm),$2),$3,$(elm)))

# internal helper to generate the cartesian product of list1 to list8 using mapping function fn
# $(call cartesian_product_helper,fn,emptylist_placeholder,elem1...,elem2..,elem3...,elem4...,elem5...,elem6...,elem7...,elem8...) 
cartesian_product_helper = $(strip \
    $(foreach f1,$3,\
      $(foreach f2,$4,\
        $(foreach f3,$5,\
          $(foreach f4,$6,\
            $(foreach f5,$7,\
              $(foreach f6,$8,\
                $(foreach f7,$9,\
                  $(foreach f8,$(10),\
                    $(call $1,\
                      $(call replace_all,$(f1),$2,),\
                      $(call replace_all,$(f2),$2,),\
                      $(call replace_all,$(f3),$2,),\
                      $(call replace_all,$(f4),$2,),\
                      $(call replace_all,$(f5),$2,),\
                      $(call replace_all,$(f6),$2,),\
                      $(call replace_all,$(f7),$2,),\
                      $(call replace_all,$(f8),$2,)\
                    ) \
                  ) \
                ) \
              ) \
            ) \
          ) \
        ) \
      ) \
    ) \
  )

# generates the cartesian product of list1 to list8 using mapping function fn, mapping empty list to place holder value
# $(call cartesian_product,fn,emptylist_placeholder,elem1...,elem2..,elem3...,elem4...,elem5...,elem6...,elem7...,elem8...) 
cartesian_product = $(call cartesian_product_helper,$1,$2,\
    $(or $(strip $3),$2),\
    $(or $(strip $4),$2),\
    $(or $(strip $5),$2),\
    $(or $(strip $6),$2),\
    $(or $(strip $7),$2),\
    $(or $(strip $8),$2),\
    $(or $(strip $9),$2),\
    $(or $(strip $(10)),$2)\
  )

# combines elements of list1 to list8 using mapping function fn
# $(call combine,fn,elem1...,elem2..,elem3...,elem4...,elem5...,elem6...,elem7...,elem8...) 
combine = $(call cartesian_product,$1,~EmptyList~,$2,$3,$4,$5,$6,$7,$8,$9)

# retrieves the version parts (major, minor) of a make version string
# $(call make_version_parts,version-string)
make_version_parts = $(shell echo '$1' | sed -ne 's![0]*\([^.]\+\).[0]*\([^.]\+\).*!\1 \2!p')

# tests if make version is greater or equal to required version
# $(call make_version_ge,required-version-string)
make_version_ge = \
  $(call ge_version, \
    $(call make_version_parts,$(make_version)),\
    $(call make_version_parts,$1)\
  )

#######################################################################################################################
# Common defaults

TOPDIR = $(call memorize,TOPDIR,$(DOBUILD_TOPDIR))
PROJECTDIR = $(call memorize,PROJECTDIR,$(DOBUILD_PROJECTDIR))
PROJECTNAME = $(call memorize,PROJECTNAME,$(DOBUILD_PROJECTNAME))
DOBUILDDIR := $(patsubst %/,%,$(dir $(current_makefile)))
OUTDIR = $(call memorize,OUTDIR,$(DOBUILD_OUTDIR))
BUILDDIR = $(call memorize,BUILDDIR,$(DOBUILD_BUILDDIR))
EXTDIR = $(call memorize,EXTDIR,$(DOBUILD_EXTDIR))

BUILDVERBOSE = $(call memorize,BUILDVERBOSE,$(DOBUILD_BUILDVERBOSE))
TESTVERBOSE = $(call memorize,TESTVERBOSE,$(DOBUILD_TESTVERBOSE))
BUILDSILENT = $(if $(BUILDVERBOSE),,1)
TESTSILENT = $(if $(TESTVERBOSE),,1)
BUILD_TESTING = $(call memorize,BUILD_TESTING,$(DOBUILD_BUILDTESTING))

VERSIONFILE = $(call memorize,VERSIONFILE,$(DOBUILD_PROJECTVERSIONFILE))
VERSION = $(call memorize,VERSION,$(DOBUILD_PROJECTVERSION))

USERID = $(call memorize,USERID,$(DOBUILD_USERID))

HOST_CONTAINER = $(call memorize,HOST_CONTAINER,$(DOBUILD_HOSTCONTAINER))

SOURCE_DATE_EPOCH ?= $(call memorize,SOURCE_DATE_EPOCH,$(shell $(DOBUILDDIR)/bin/get_source_date_epoch $(TOPDIR)))
BUILDTIME = $(call memorize,BUILDTIME,$(shell date -u -d '@$(SOURCE_DATE_EPOCH)' --rfc-3339 ns 2>/dev/null))

JOBSLOTS_DEFAULT = $(if $(findstring --jobserver-,$(make_cmdline)),2,$(nproc))
JOBSLOTS = $(call memorize,JOBSLOTS,$(or $(DOBUILD_JOBSLOTS),$(JOBSLOTS_DEFAULT)))

SKIP_MD5SUM = $(call memorize,SKIP_MD5SUM,$(DOBUILD_SKIPMD5SUM))
SKIP_CURL = $(call memorize,SKIP_CURL,$(DOBUILD_SKIPCURL))
SKIP_DEFAULTTARGET = $(call memorize,SKIP_DEFAULTTARGET,$(DOBUILD_SKIPDEFAULTTARGET))
SKIP_EXTERNSYNC = $(call memorize,SKIP_EXTERNSYNC,$(DOBUILD_SKIPEXTERNSYNC))

DEFAULTTARGET = $(BUILDTARGET)
HOSTMARCH = 
MARCH = 
VENDOR = 
DISTRIB_ID = $(VENDOR)
SYS = 
ABI = 
DISTRIB_VERSION = 
ID = 
VARIANT = 

EXCLUDE_HOSTMARCH = 
EXCLUDE_MARCH = 
EXCLUDE_VENDOR = 
EXCLUDE_DISTRIB_ID = $(EXCLUDE_VENDOR)
EXCLUDE_SYS = 
EXCLUDE_ABI = 
EXCLUDE_DISTRIB_VERSION = 
EXCLUDE_ID = 
EXCLUDE_VARIANT = 

FILTER = $(call memorize,FILTER,$(DOBUILD_FILTER))
EXCLUDEFILTER = $(call memorize,EXCLUDEFILTER,$(DOBUILD_EXCLUDEFILTER))
MEMCHECKFILTER = $(call memorize,MEMCHECKFILTER,$(DOBUILD_FILTERMEMCHECK))

$(VERBOSE)SILENT := @

make_pid = $(call memorize,make_pid,$(shell echo "$$PPID"))
make_cmdline = $(call memorize,make_cmdline,$(shell set -- $$(ps T 2>/dev/null | sed -n -e 's!^\s*$(make_pid)\s\+.*\($(call escape,$(MAKE),!)\s\+.*\)!\1!p') && echo "$$@"))

nproc = $(call memorize,nproc,$(shell nproc || echo '1'))
machine = $(call memorize,machine,$(shell uname -m 2>/dev/null))

make_version = $(MAKE_VERSION)
make_version_req = 3.81

# defines $, $; $% $= $: $(\#) $(\\) $($$) $( ) variables.
# NOTE: $(,) has to be used within macros to avoid conflict with argument spliting operator ,
, := ,
; := ;
% := %
esc_slash := \\
$(esc_slash) := \\
esc_hash := \\\#
$(esc_hash) := \#
esc_equal := =
$(esc_equal) := =
esc_colon := :
$(esc_colon) := :
esc_dollar := $$
$(esc_dollar) := $$
esc_space := 
esc_space += 
$(esc_space) := 
$(esc_space) +=
define n


endef

DEFAULT_FMT := %s
ifneq ($(COLOR),)
  DEFAULT_FMT := %b
  tc_red := \033[0;31m
  tc_green := \033[0;32m
  tc_brown := \033[0;33m
  tc_blue := \033[0;34m
  tc_purple := \033[0;35m
  tc_cyan := \033[0;36m
  tc_light_gray := \033[0;37m
  tc_dark_gray := \033[1;30m
  tc_light_green := \033[1;32m
  tc_yellow := \033[1;33m
  tc_light_blue := \033[1;34m
  tc_light_purple := \033[1;35m
  tc_light_cyan := \033[1;36m
  tc_reset := \033[0m
endif

docker_runflags = 
docker_buildflags = 
image_buildargs = 
project_targets = 

#######################################################################################################################
# Makefile dependencies

MAKEFILE_DEPS += cat
MAKEFILE_DEPS += id
MAKEFILE_DEPS += echo
MAKEFILE_DEPS += sed
MAKEFILE_DEPS += pwd
MAKEFILE_DEPS += cd
MAKEFILE_DEPS += sort
MAKEFILE_DEPS += printf
MAKEFILE_DEPS += test
MAKEFILE_DEPS += base64
MAKEFILE_DEPS += stat

#######################################################################################################################
# Basic assertions

ASSERTIONS += $(call assert_scalar,$(PROJECTDIR),Project directory PROJECTDIR='$(PROJECTDIR)' should not contain whitespaces)
ASSERTIONS += $(call assert_scalar,$(OUTDIR),Output directory OUTDIR='$(OUTDIR)' should not contain whitespaces)
ASSERTIONS += $(call assert_scalar,$(DOBUILDDIR),Script directory DOBUILDDIR='$(DOBUILDDIR)' should not contain whitespaces)
ASSERTIONS += $(call assert,$(call not,$(filter $(abspath $(PROJECTDIR)),$(abspath $(sort $(OUTDIR) $(OUTDIRS))))),Project \
location PROJECTDIR='$(abspath $(PROJECTDIR))' should not point to one of the output locations:$n \
$(addsuffix $n,$(abspath $(sort $(OUTDIR) $(OUTDIRS)))))
ASSERTIONS += $(call assert,$(SOURCE_DATE_EPOCH),Value of variable SOURCE_DATE_EPOCH should not be empty)
ASSERTIONS += $(call assert,$(or $(SKIP_DEFAULTTARGET),$(filter $(DEFAULTTARGET),$(project_targets))),Default \
target TARGET='$(DEFAULTTARGET)' is not contained in the list of project targets:$n \
$(addsuffix $n,$(sort $(project_targets))))
ASSERTIONS += $(call assert,$(or $(call not,$(project_targets)),$(strip $(TARGETS))),No target matches$(,) with include \
FILTER=[$(call join_s,$(FILTER),$(,))] and exclude filter EXCLUDEFILTER=[$(call join_s,$(EXCLUDEFILTER),$(,))]$(,) \
one of the project targets:$n $(addsuffix $n,$(sort $(project_targets))))

EXPECTATIONS += $(call expect,$(call make_version_ge,$(make_version_req)),Using old make version=$(make_version)$(,) \
consider upgrading to a newer version >= $(make_version_req))

endif
