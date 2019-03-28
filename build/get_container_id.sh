#!/bin/sh

set -e

container_id() {
  if [ "$#" -le 0 ]; then
    # shellcheck disable=SC2119
    container_id_by_cgroup
  else
    "$DOCKER" inspect --format='{{.Id}}' "$@"
  fi
}

# shellcheck disable=SC2120
container_id_by_cgroup() {
  impl_container_id_by_cgroup "$@" || return "$?"
}

impl_container_id_by_cgroup() {
  file="${1:-/proc/self/cgroup}"

  while IFS= read -r cmd; do
    id="$(echo "$cmd" | sed -n -e 's/[^:]*:[^:]*:.*\/\([0-9a-fA-F]\+\)$/\1/p')"
    if container_id "$id" >/dev/null 2>&1; then
      echo "$id"
      return 0
    fi
  done < "$file"

  return 1
}

DOCKER="${DOCKER:-docker}"

if [ -z "${CONTAINER+x}" ] && [ -n "$DOCKER" ]; then
  CONTAINER="$(container_id "$@")" || true
fi

echo "$CONTAINER"
