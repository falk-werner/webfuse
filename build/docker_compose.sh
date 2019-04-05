#!/bin/sh

set -e

DOCKER="${DOCKER:-docker}"
DOCKER_HOST="${DOCKER_HOST:-/var/run/docker.sock}"
IMAGE="${IMAGE:-${REGISTRY_PREFIX}docker/compose:1.24.0}"
NETWORK="${NETWORK:-host}"
USERID="${USERID:-$(id -u)}"
SCRIPT_ROOT="${SCRIPT_ROOT:-"$(dirname "$(readlink -f "$0")")"}"
ENTRYPOINT="${ENTRYPOINT:-docker-compose}"
HOST_CONTAINER="${HOST_CONTAINER:-"$("$SCRIPT_ROOT/get_container_id.sh")"}" || true
HOST_ENVFILTER="${HOST_ENVFILTER:-^DOCKER_\|^COMPOSE_}"

RUNFLAGS='--rm'

# setup options for connection to docker host
if [ -S "$DOCKER_HOST" ]; then
  DOCKER_SOCK_GROUP="$(stat -c '%g' "$DOCKER_HOST")"
  RUNFLAGS="$RUNFLAGS -e DOCKER_SOCK_GROUP=$DOCKER_SOCK_GROUP --group-add $DOCKER_SOCK_GROUP"
  RUNFLAGS="$RUNFLAGS -v $DOCKER_HOST:$DOCKER_HOST -e DOCKER_HOST"
else
  RUNFLAGS="$RUNFLAGS -e DOCKER_HOST -e DOCKER_TLS_VERIFY -e DOCKER_CERT_PATH"
fi

if [ -n "$HOST_CONTAINER" ]; then
  RUNFLAGS="$RUNFLAGS --volumes-from $HOST_CONTAINER"
fi

RUNFLAGS="$RUNFLAGS --network $NETWORK"

if [ -n "$CONTAINER_CGROUP_PARENT" ]; then
  RUNFLAGS="$RUNFLAGS --cgroup-parent $CONTAINER_CGROUP_PARENT"
fi

RUNFLAGS="$RUNFLAGS --workdir $PWD"

# if STDIN piped or redirected
if [ -p /dev/stdin ] || { [ ! -t 0 ] && [ ! -p /dev/stdin ]; }; then
  RUNFLAGS="$RUNFLAGS --interactive"
elif [ -t 1 ]; then
  RUNFLAGS="$RUNFLAGS --tty"
fi
  
if [ -t 0 ] && ! "$SCRIPT_ROOT/is_running_in_bg.sh" $$; then
  RUNFLAGS="$RUNFLAGS --interactive"
fi

RUNFLAGS="$RUNFLAGS --user $USERID:$USERID"

ENVVARS="$(printenv | grep -e "$HOST_ENVFILTER" | sed -n -e 's/\([^=]*\)=.*/-e \1/p')" || true
RUNFLAGS="$RUNFLAGS $ENVVARS"

# shellcheck disable=SC2086
set -- $RUNFLAGS --entrypoint "$ENTRYPOINT" "$IMAGE" "$@"

exec "$DOCKER" run "$@"
