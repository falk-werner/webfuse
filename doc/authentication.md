# Authentication

Webfuse supports two authentications mechanisms:

- token-based authentication using HTTP headers
- in-protocol authentication

To activate authentication, two command line option needs to be given:

- `--wf-authenticator PATH`  
  allows to specify an executable used for authentication
- `--wf-auth-header HEADER` _(optional)_
  allows to specify the HTTP header used for authentication

When `--wf-auth-header` is not specifiend or the header is not contained
in the HTTP request, the in-protocol solutions is used: Before any other
operation, the credentials are queried via `getcreds`request.

## Authenticator

An authenticator is an executable or script used for token-based
authentication. During HTTP handshake, webfuse will scan for the
configured HTTP header and invoke the authenticator.

    authenticator TOKEN

The provided `token` contains the contents of the HTTP header.

## Header restrictions

Note that not any HTTP header can be specified using `--wf-auth-header`
option. The following headers are supported:

- `X-Auth-Token`
- `Authorization`

In addition to that, any non-standard header can be specified.

Due to implementation limitation, most standard headers can not be used by now. Please let us know, when you intend to use a header that is not supported yet. Please create an issue in that case.
