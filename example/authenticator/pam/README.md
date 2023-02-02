# webfuse PAM authenticator

This directory contains an example of a webfuse authenticator using PAM.

The authenticator uses `username` and `password` for authentication.
Since webfuse only provides a token, username and password are encoded as follows:

    TOKEN := base64 ( USERNAME ":" PASSWORD )

Example:

    USERNAME := "user"
    PASSWORD := "secret"
    TOKEN    := base64 ( "user:secret" ) = "XNlcjpzZWNyZXQ="

The utilities `webfuse_pam_token_encode` and `webfuse_pam_token_decode` can be used
to encode and decode tokens.

## Build

    cmake -b build
    cmake build

## Dependencies

- libpam
- libb64

## Notes

- in order to make the authenticator work, read access to /etc/shadow is needed
