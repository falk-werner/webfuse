#!/bin/sh

set -e

DOCKER="${DOCKER:-docker}"
DOCKER_HOST="${DOCKER_HOST:-/var/run/docker.sock}"
IMAGE="${IMAGE:-${REGISTRY_PREFIX}docker/compose:1.24.0}"
NETWORK="${NETWORK:-host}"
USERID="${USERID:-$(id -u)}"
SCRIPT_ROOT="${SCRIPT_ROOT:-"$(cd "$(dirname "$0")" && echo "$PWD")"}"
ENTRYPOINT="${ENTRYPOINT:-docker-compose}"
HOST_ENVFILTER="${HOST_ENVFILTER:-^DOCKER_\|^COMPOSE_}"

set -- --entrypoint "$ENTRYPOINT" "$IMAGE" "$@"
set -- --user "$USERID:$USERID" --network "$NETWORK" --workdir "$PWD" "$@"

ENVFLAGS="$(printenv | grep -e "$HOST_ENVFILTER" | sed -n -e 's/\([^=]*\)=.*/-e \1/p')" || true
#shellcheck disable=SC2086
set -- $ENVFLAGS "$@"

if [ -n "$CONTAINER_CGROUP_PARENT" ]; then
  set -- --cgroup-parent "$CONTAINER_CGROUP_PARENT" "$@"
fi

HOST_CONTAINER="${HOST_CONTAINER:-"$("$SCRIPT_ROOT/get_container_id.sh")"}" || true
if [ -n "$HOST_CONTAINER" ]; then
  set --  --volumes-from "$HOST_CONTAINER" "$@"
fi

# setup options for connection to docker host
if [ -S "$DOCKER_HOST" ]; then
  DOCKER_SOCK_GROUP="$(stat -c '%g' "$DOCKER_HOST")"
  set -- -e DOCKER_SOCK_GROUP="$DOCKER_SOCK_GROUP" --group-add "$DOCKER_SOCK_GROUP" "$@"
else
  set -- -e DOCKER_HOST -e DOCKER_TLS_VERIFY -e DOCKER_CERT_PATH "$@"
fi

if [ -t 0 ] && ! "$SCRIPT_ROOT/is_running_in_bg.sh" $$; then
  set -- --interactive "$@"
fi

# if STDIN piped or redirected
if [ -p /dev/stdin ] || { [ ! -t 0 ] && [ ! -p /dev/stdin ]; }; then
  set -- --interactive "$@"
elif [ -t 1 ]; then
  set -- --tty "$@"
fi

set -- --rm "$@"

exec "$DOCKER" run "$@"
