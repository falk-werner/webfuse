#!/usr/bin/bash

AUTH_TOKEN="$1"

if [[ "$AUTH_TOKEN" == "simple_token" ]]
then
    echo "$(date): webfuse: auth granted: $AUTH_TOKEN" >> /tmp/webfuse_auth.log
else
    echo "$(date): webfuse: auth denied: $AUTH_TOKEN" >> /tmp/webfuse_auth.log
    exit 1
fi


