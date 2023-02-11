#!/usr/bin/bash

read AUTH_TOKEN

if [[ "$AUTH_TOKEN" == "simple_token" ]]
then
    echo "$(date): webfuse: auth granted: $AUTH_TOKEN" >> /tmp/webfuse_auth.log
else
    echo "$(date): webfuse: auth denied: $AUTH_TOKEN" >> /tmp/webfuse_auth.log
    exit 1
fi


