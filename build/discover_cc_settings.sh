#!/bin/sh

set -e

export LANG=C
export LC_ALL=C

cleanup() {
  rm -rf "$INCLUDE_DIRS_TMPFILE"
}

trap cleanup TERM INT EXIT

DEST="${DEST:-$PWD}"

CMAKE_CACHE_FILE="${1:-CMakeCache.txt}"
CMAKE_CXX_COMPILER="$(sed -n -e 's/CMAKE_CXX_COMPILER:FILEPATH=\(.*\)$/\1/p' "$CMAKE_CACHE_FILE")"
CMAKE_C_COMPILER="$(sed -n -e 's/CMAKE_C_COMPILER:FILEPATH=\(.*\)$/\1/p' "$CMAKE_CACHE_FILE")"

INCLUDE_DIRS_PREFIX="${2:-$DEST}"
INCLUDE_DIRS_TMPFILE="$(mktemp -p "$DEST" include_dirs_XXXXXXXXXX.txt)"
INCLUDE_DIRS_FILE="${INCLUDE_DIRS_FILE:-$DEST/include_dirs.txt}"

C_BUILTIN_FILE="${C_BUILTIN_FILE:-$DEST/builtins.h}"
CXX_BUILTIN_FILE="${CXX_BUILTIN_FILE:-$DEST/builtins.hpp}"

if [ -z "${RSYNC+x}" ]; then
  RSYNC=rsync
fi

{
  {
    echo | "$CMAKE_CXX_COMPILER" -xc++ -E -Wp,-v - 2>&1;
    echo | "$CMAKE_C_COMPILER" -xc -E -Wp,-v - 2>&1;
  } \
  | sed -n -e 's/\s\+\(\/.*$\).*/\1/p';
  
  sed -n -e 's/.*_INCLUDE_DIRS:INTERNAL=\(.*\)$/\1/p' "$CMAKE_CACHE_FILE";
} \
| sed 's/;/\n/g' |  sed '/^$/d' | sort -u 2>/dev/null 1> "$INCLUDE_DIRS_TMPFILE"

{
  echo '#include <...> search starts here:'
  # shellcheck disable=SC1117
  sed -n -e "s@\(.*\)@$INCLUDE_DIRS_PREFIX\1@p" "$INCLUDE_DIRS_TMPFILE"
  echo 'End of search list.'
} > "$INCLUDE_DIRS_FILE"

if [ -n "$RSYNC" ]; then
  "$RSYNC" -arR --files-from="$INCLUDE_DIRS_TMPFILE" / "$DEST"
  echo "Include folders copied to $DEST" >&2
fi

"$CMAKE_C_COMPILER" -xc -dM -E - < /dev/null > "$C_BUILTIN_FILE"
echo "C Compiler built-ins written to $C_BUILTIN_FILE" >&2
"$CMAKE_CXX_COMPILER" -xc++ -dM -E - < /dev/null > "$CXX_BUILTIN_FILE"
echo "C++ Compiler built-ins written to $CXX_BUILTIN_FILE" >&2
