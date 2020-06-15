# Webfuse API introduction

This introduction provides a general overview to webfuse API.
Please refer to the [build instructions](build.md) to generate API reference documentation.

## Contents

-   [Authentication](#Authentication (Adapter Server))
-   [Adapter Client](#Adapter Client)

## Authentication (Adapter Server)

By default, webfuse daemon will redirect each filesystem call to the first connected provider without any authentication.
This might be good for testing purposes or when an external authentication mechanism is used. In some use cases, explicit authentication is needed. Therefore, authentication can be enabled within webfuse daemon.

When authentication is enabled, filesystem calls are only redirected to a connected provider, after `authenticate`
has succeeded.

![authenticate](authenticate.png)

### Enable authentication

Authentication is enabled, if one or more authenticators are registered via `wf_server_config`.

    static bool authenticate(struct wf_credentials * creds, void * user_data)
    {
        char const * username = wf_credentials_get(creds, "username");
        char const * password = wf_credentials_get(creds, "password");

       return ((NULL != username) && (0 == strcmp(username, "bob")) &&
               (NULL != password) && (0 == strcmp(password, "???")));
    }

    wf_server_config * config = wf_server_config_create();
    wf_server_config_add_authenticator(config, "username", &authenticate, NULL);

    wf_server * server = wf_server_create(config);
    //...

### Authenticator types and credentidals

Each authenticator is identified by a user defined string, called `type`. The type is provided by the `authenticate` request, so you can define different authenticators for different authentication types, e.g. username, certificate, token.

Actually, only one type is used: **username**  
**It is strongly recommended to prefix custom authenticator types with an underscore (`_`) to avoid name clashes.**

The `wf_credentials`struct represents a map to access credentials as key-value pairs, where both, key and value, are of type string.

#### username

The authenticator type **username** is used to authenticate via username and password. Valid credentials should contain two keys.

-   **username** refers to the name of the user
-   **password** refers to the password of the user

**Note** that no further encryption is done, so this authenticator type should not be used over unencrypted websocket connections.

## Adapter Client

Webfuse also supports a client version of an adapter. This might be useful
to connect to a cloud based provider server and request a filesystem.

The adapter client is driven by a callback function, which is triggered whenever
an event occurs, an adapter should take care of.

    static void client_callback(
       struct wf_client * client,
       int reason,
       void * arg)
    {
        switch (reason)
        {
            // ... handle events
            default:
                break;
        }
    }

    // ...
    void * user_data = ...
    struct wf_client * client = wf_client_create(&client_callback, user_data);

### Init and Cleanup

There are two events definied to handle init and cleanup of clients:

-   `WF_CLIENT_INIT`
-   `WF_CLIENT_CLEANUP`

These two are the outer-most events and can be used for custom initialization
and cleanup.

A thrid event, `WF_CLIENT_CREATED`, is triggered, when the client is fully created.
You might use this event to connect to a foreign provider.

### Connection Status

The connection status is relected by two events:

-   `WF_CLIENT_CONNECTED`
-   `WF_CLIENT_DISCONNECTED`

The disconnected event is also triggerd, when an attempt to connect fails.

### Transport Layer Security

During startup, the event `WF_CLIENT_GET_TLS_CONFIG` is triggered.
In this case, the `arg` parameter points to an instance of `struct wf_client_tlsconfig`.

To enable TLS, set this struct. If the callback is ignorted or the struct is not
set, TLS is not active.

    static void client_callback(
        struct wf_client * client,
        int reason,
        void * arg)
    {
        switch (reason)
        {
            // ...
            case WF_CLIENT_GET_TLS_CONFIG:
                {
                    struct wf_client_tlsconfig * tls = arg;
                    wf_client_tslconfig_set_keypath(tls, "/path/to/key.pem");
                    wf_client_tslconfig_set_certpath(tls, "/path/to/cert.pem");
                    wf_client_tslconfig_set_cafilepath(tls, "/path/to/ca_file.pem");
                }
                break;
            default:
                break;
        }
    }

### Authentication (Adapter Client)

During `wf_client_authenticate` the event `WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS`
is triggered to query credentials for authentication.

In this case, the `arg` paramter point to an instance of `struct wf_credentials`.

    static void client_callback(
        struct wf_client * client,
        int reason,
        void * arg)
    {
        switch (reason)
        {
            // ...
            case WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS:
                {
                    struct wf_credentials * creds = arg;
                    wf_credentials_set_type(creds, "username");
                    wf_credentials_add(creds, "username", "Bob");
                    wf_credentials_add(creds, "password", "secret");
                }
                break;
            default:
                break;
        }
    }
