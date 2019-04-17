#!/bin/sh

set -e

running_in_background() {
  pid="$1"
  
  case "$("$PS" -o stat= -p "$pid" 2>/dev/null || echo '+')" in
  *+*) ;;
  *)   return 0 ;;
  esac
  
  return 1
}

PS="${PS:-ps}"

running_in_background "$@"
