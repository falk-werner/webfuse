#!/bin/sh -e

# <host-arch>+<target-arch><sub>-<vendor>-<sys>-<abi>+<id>
#
# arch = x86_64, i386, arm, thumb, mips, etc.
# sub = for ex. on ARM: v5, v6m, v7a, v7m, etc.
# vendor = pc, apple, nvidia, ibm etc.
# sys = none, linux, win32, darwin, cuda, etc.
# abi = eabi, gnu, android, macho, elf, etc.

sh_escape() {
  sed \
    -e "s/\\([;\\*&\\|\\'\\\"\\\$\\(\\)]\\)/\\\\\\1/g" \
    -e "s/\\([^$ESCCHAR]\\)$ESCCHAR\\(\\s\\)/\\1$ESCCHAR\\\\\\2/g" \
    -e "s/$ESCCHAR/\\\\$ESCCHAR/"
}

split() {
  delimiter="$1"
  shift
  printf -- '%s\n' "$@" | sed -e "s/\\([^$ESCCHAR]\\)$delimiter/\\1\\n/g"
}

unescape() {
  printf -- '%s\n' "$@" | sed -e "s/[$ESCCHAR]\\(.\\)\\?/\\1/g"
}

join() {
  paste -s -d"$1" -
}

tail() {
  skip 1 "$@"
}

skip() {
  toskip="$1"
  shift
  
  if [ "$toskip" -gt "$#" ]; then
    toskip="$#"
  fi
  
  while [ "$toskip" -gt '0' ]
  do
   shift
   toskip="$((toskip - 1))"
  done

  printf -- '%s\n' "$@"
}

host_march() {
  uname -m
}

if [  "$#" -eq 0 ]; then
  {
    printf 'Usage:\n%s <target-name>\n\n' "$0"
    printf 'Format of target-name:\n'
    printf '[<host-arch>+][<target-triple>+]<id>\n'
    printf 'or\n'
    printf '[<host-arch>] [<target-triple>] <id>\n\n'
    printf 'Format of target-triple:\n'
    printf '<target-arch><sub>[-<vendor>][-<sys>][-<abi>]\n'
    printf 'or\n'
    printf '<target-arch><sub>[-<distrib>[@distrib-version]][-<sys>][-<abi>]\n\n'
    printf 'Special characters: %s,+,-,@\n' "$ESCCHAR"
    printf 'Escape character: %s\n\n' "$ESCCHAR"
    printf 'Examples:\n'
    printf '%s myapp\n' "$0"
    printf '%s x86_64+myapp\n' "$0"
    printf '%s x86_64 myapp\n' "$0"
    printf '%s x86_64-alpine@3.9-linux-gnueabihf+myapp\n' "$0"
    printf '%s x86_64-alpine@3.9-linux-gnueabihf myapp\n' "$0"
    printf '%s x86_64+arm32v7-alpine@3.9-linux-gnueabihf+myapp\n' "$0"
    printf '%s x86_64 arm32v7-alpine@3.9-linux-gnueabihf myapp\n' "$0"
    printf '%s x86_64+arm32v7-alpine@3.9-linux-gnueabihf+myapp+id\n' "$0"
    printf '%s x86_64 arm32v7-alpine@3.9-linux-gnueabihf myapp id\n\n' "$0"
  } >&2
  return 1
fi

ESCCHAR='~'
TARGET="$(echo "$@" | join '+')"

# TODO: remove ugly hack
# necessary while posix shell can not handle arrays except the argument array
# maybe re-implement using a more suitable language!

#shellcheck disable=SC2046
eval set -- $(split '+' "$@" | sh_escape)

if [  "$#" -gt 2 ]; then
  HOST_ARCH="$1"
  shift
fi

if [  "$#" -eq 1 ]; then
  set -- "$(host_march)" "$@"
fi

ID_VARIANT="$(tail "$@" | join '+')"
TARGET_TRIPLE="$1"

#shellcheck disable=SC2046
eval set -- $(split '@' "$ID_VARIANT" | sh_escape)

ID="$1"
if [ -z "$ID" ]; then
  printf 'error: failed to parse "%s" aka "%s", unsupported target name\n' "$*" "$TARGET" >&2
  return 2
fi

VARIANT="$(tail "$@" | join '@')"
VARIANT="${VARIANT:-release}"

#shellcheck disable=SC2046
eval set -- $(split '-' "$TARGET_TRIPLE" | sh_escape)

TARGET_ARCH_SUB="$1"
HOST_ARCH="${HOST_ARCH:-$TARGET_ARCH_SUB}"
VENDOR="${2:-unknown}"
TARGET_SYS="${3:-linux}"

TARGET_ABI="$(skip 3 "$@" | join '-')"
TARGET_ABI="${TARGET_ABI:-gnu}"

#shellcheck disable=SC2046
eval set -- $(split '@' "$VENDOR" | sh_escape)

VENDOR_OR_DISTRIB_ID="$1"

DISTRIB_VERSION="$(tail "$@" | join '@')"
DISTRIB_VERSION="${DISTRIB_VERSION:-latest}"

set -- \
  "$HOST_ARCH" \
  "$TARGET_ARCH_SUB" \
  "$VENDOR_OR_DISTRIB_ID" \
  "$TARGET_SYS" \
  "$TARGET_ABI" \
  "$ID" \
  "$DISTRIB_VERSION" \
  "$VARIANT"

unescape "$@" | cat
