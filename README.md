[![Build Status](https://travis-ci.org/falk-werner/webfuse.svg?branch=master)](https://travis-ci.org/falk-werner/webfuse)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/d6c20d37bb3a456a9c0ee224001081b2)](https://www.codacy.com/app/falk.werner/webfuse?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=falk-werner/webfuse&amp;utm_campaign=Badge_Grade)

# webfuse

webfuse combines libwebsockets and libfuse. It allows ot attach a remote filesystem via websockets.

## Contents

-   [Motivation](#Motivation)
-   [Concept](#Concept)
-   [Similar Projects](#Similar-Projects)
-   [API](#API)
-   [Build and run](#Build-and-run)
-   [Dependencies](#Dependencies)

## Motivation

Many embedded devices, such as smart home or [IoT](https://en.wikipedia.org/wiki/Internet_of_things) devices are very limited regarding to their (non-volatile) memory resources. Such devices are typically comprised of an embedded linux and a small web server, providing an interface for maintenance purposes.

Some use cases, such as firmware update, require to transfer (larger) files to the device. The firmware file is often stored multiple times on the device:

1.  cached by the web server, e.g. [lighttpd](https://redmine.lighttpd.net/boards/2/topics/3451)
2.  copied to locally, e.g. /tmp
3.  uncompressed, also to /tmp

Techniques like [SquashFS](https://en.wikipedia.org/wiki/SquashFS) help to avoid the third step, since the upgrade file can be mounted directly. [RAUC](https://rauc.io/) shows the use of SquashFS within an update facility.  
However at least one (unecessary) copy of the upload file is needed on the device.

To avoid Steps 1 and 2, it would be great to keep the update file entirely in web server, just like [NFS](https://en.wikipedia.org/wiki/Network_File_System) or [WebDAV](https://wiki.archlinux.org/index.php/WebDAV). Unfortunately, NFS is not based on any protocol, natively usable by a web application. WebDAV is based on HTTP, but it needs a server providing the update file.

webfuse solves this problem by using the [WebSocket](https://en.wikipedia.org/wiki/WebSocket) protocol. The emdedded device runs a service, known as webfuse adapter, awaiting incoming connections, e.g. from a web browser. The browser acts as a file system provider, providing the update file to the device.

## Concept

![concept](doc/concept.png)

With webfuse it is possible to implement remote filesystems based on websockets.
A reference implementation of such a daemon is provided within the examples. The picture above describes the workflow:

-   The websocket filesystem daemon (*webfuse daemon*) mounts a filesystem on startup.  
    It starts the websocket server and waits for incoming connections.

-   A remote filesystem provider connects to webfuse daemon via websocket protocol.  
    The example includes such a provider implemented in HTML and JavaScript.

-   Whenever the user makes filesystem requests, such as *ls*, the request is redirected via webfuse daemon to the connected filesystem provider

Currently all requests are initiated by webfuse daemon and responded by filesystem provider. This may change in future, e.g. when authentication is supported.

## Similar Projects

### Davfs2

[davfs2](http://savannah.nongnu.org/projects/davfs2) is a Linux file system driver that allows to mount a [WebDAV](https://wiki.archlinux.org/index.php/WebDAV) resource. WebDAV is an extension to HTTP/1.1 that allows remote collaborative authoring of Web resources.

Unlike webfuse, davfs2 mounts a remote filesystem locally, that is provided by a WebDAV server. In contrast, webfuse starts a server awaiting client connections to attach the remote file system.

## API

### Requests, responses and notifications

There are three types of messages, used for communication between webfuse daemon and filesystem provider. All message types are encoded in [JSON](https://www.json.org/) and strongly inspired by [JSON-RPC](https://www.jsonrpc.org/).

#### Request

A request is used by a sender to invoke a method on the receiver. The sender awaits a response from the receiver. Since requests and responses can be sendet or answered in any order, an id is provided in each request to identify it.

    {
      "method": <method_name>,
      "params": <params>,
      "id"    : <id>
    }

| Item        | Data type | Description                       |
| ----------- |:---------:| --------------------------------- |
| method_name | string    | name of the method to invoke      |
| params      | array     | method specific parameters        |
| id          | integer   | id, which is repeated in response |

#### Response

A response is used to answer a prior request. There are two kinds of responses:

##### Successful Results

    {
       "result": <result>,
       "id": <id>
    }

| Item        | Data type | Description             |
| ----------- |:---------:| ----------------------- |
| result      | any       | request specific result |
| id          | integer   | id, same as request     |

##### Error notifications

    {
       "error": {
         "code": <code>
       },
       "id": <id>
    }

| Item        | Data type | Description         |
| ----------- |:---------:| ------------------- |
| code        | integer   | error code          |
| id          | integer   | id, same as request |

##### Error codes

| Symbolic name      | Code      | Description            |
| ------------------ | ---------:| ---------------------- |
| GOOD               | 0         | no error               |
| BAD                | 1         | generic error          |
| BAD_NOTIMPLEMENTED | 2         | method not implemented |
| BAD_TIMEOUT        | 3         | timeout occured        |
| BAD_BUSY           | 4         | resource busy          |
| BAD_FORMAT         | 5         | invalid formt          |
| BAD_NOENTRY        | 101       | invalid entry          |
| BAD_ACCESS_DENIED  | 102       | access not allowed     |

#### Notification

Notfications are used to inform a receiver about something. Unlike requests, notifications are not answered. Therefore, an id is not supplied.

    {
      "method": <method_name>,
      "params": <params>
    }

| Item        | Data type | Description                       |
| ----------- |:---------:| --------------------------------- |
| method_name | string    | name of the method to invoke      |
| params      | array     | method specific parameters        |

### Requests (Adapter -> Provider)

#### lookup

Retrieve information about a filesystem entry by name.

    webfuse daemon: {"method": "lookup", "params": [<parent>, <name>], "id": <id>}
    fs provider: {"result": {
        "inode": <inode>,
        "mode" : <mode>,
        "type" : <type>,
        "size" : <size>,
        "atime": <atime>,
        "mtime": <mtime>,
        "ctime": <ctime>
        }, "id": <id>}

| Item        | Data type       | Description                                 |
| ----------- | --------------- | ------------------------------------------- |
| parent      | integer         | inode of parent directory (1 = root)        |
| name        | string          | name of the filesystem object to look up    |
| inode       | integer         | inode of the filesystem object              |
| mode        | integer         | unix file mode                              |
| type        | "file" or "dir" | type of filesystem object                   |
| size        | integer         | required for files; file size in bytes      |
| atime       | integer         | optional; unix time of last access          |
| mtime       | integer         | optional; unix time of last modification    |
| ctime       | intefer         | optional; unix time of last metadata change |

#### getattr

Get file attributes.

    webfuse daemon: {"method": "getattr", "params": [<inode>], "id": <id>}
    fs provider: {"result": {
        "mode" : <mode>,
        "type" : <type>,
        "size" : <size>,
        "atime": <atime>,
        "mtime": <mtime>,
        "ctime": <ctime>
        }, "id": <id>}

| Item        | Data type       | Description                                 |
| ----------- | --------------- | ------------------------------------------- |
| inode       | integer         | inode of the filesystem object              |
| mode        | integer         | unix file mode                              |
| type        | "file" or "dir" | type of filesystem object                   |
| size        | integer         | required for files; file size in bytes      |
| atime       | integer         | optional; unix time of last access          |
| mtime       | integer         | optional; unix time of last modification    |
| ctime       | intefer         | optional; unix time of last metadata change |

#### readdir

Read directory contents.  
Result is an array of name-inode pairs for each entry. The generic entries 
"." and ".." should also be provided.

    webfuse daemon: {"method": "readdir", "params": [<dir_inode>], "id": <id>}
    fs provider: {"result": [
        {"name": <name>, "inode": <inode>},
        ...
        ], "id": <id>}

| Item        | Data type       | Description                    |
| ----------- | --------------- | ------------------------------ |
| dir_inode   | integer         | inode of the directory to read |
| name        | integer         | name of the entry              |
| inode       | integer         | inode of the entry             |

#### open

Open a file.

    webfuse daemon: {"method": "readdir", "params": [<inode>, <flags>], "id": <id>}
    fs provider: {"result": {"handle": <handle>}, "id": <id>}

| Item        | Data type | Description                   |
| ----------- | ----------| ----------------------------- |
| inode       | integer   | inode of the file             |
| flags       | integer   | access mode flags (see below) |
| handle      | integer   | handle of the file            |

##### Flags

| Symbolic name | Code      | Description                 |
| --------------| ---------:| --------------------------- |
| O_ACCMODE     | 0x003     | access mode mask            |
| O_RDONLY      | 0x000     | open for reading only       |
| O_WRONLY      | 0x001     | open for writing only       |
| O_RDWR        | 0x002     | open for reading an writing |
| O_CREAT       | 0x040     | create (a new) file         |
| O_EXCL        | 0x080     | open file exclusivly        |
| O_TRUNC       | 0x200     | open file to truncate       |
| O_APPEND      | 0x400     | open file to append         |

#### close

Informs filesystem provider, that a file is closed.  
Since `close` is a notification, it cannot fail.

    webfuse daemon: {"method": "close", "params": [<inode>, <handle>, <flags>], "id": <id>}

| Item        | Data type | Description                  |
| ----------- | ----------| ---------------------------- |
| inode       | integer   | inode of the file            |
| handle      | integer   | handle of the file           |
| flags       | integer   | access mode flags (see open) |

#### read

Read from an open file.

    webfuse daemon: {"method": "close", "params": [<inode>, <handle>, <offset>, <length>], "id": <id>}
    fs provider: {"result": {
        "data": <data>,
        "format": <format>,
        "count": <count>
        }, "id": <id>}

| Item        | Data type | Description                   |
| ----------- | ----------| ----------------------------- |
| inode       | integer   | inode of the file             |
| handle      | integer   | handle of the file            |
| offset      | integer   | Offet to start read operation |
| length      | integer   | Max. number of bytes to read  |
| data        | integer   | handle of the file            |
| format      | string    | Encoding of data (see below)  |
| count       | integer   | Actual number of bytes read   |

##### Format

| Format     | Description                                              |
| ---------- | -------------------------------------------------------- |
| "identiy"  | Use data as is; note that JSON strings are UTF-8 encoded |
| "base64"   | data is base64 encoded                                   |

### Requests (Provider -> Adapter)

#### authtenticate

Authenticate the provider.  
If authentication is enabled, a provider must be authenticated by the adapter before the adapter will send any messages.

    fs provider: {"method": "authenticate", "params": [<type>, <credentials>], "id": <id>}
    webfuse daemon: {"result": {}, "id": <id>}

| Item        | Data type | Description                     |
| ----------- | ----------| ------------------------------- |
| type        | string    | authentication type (see below) |
| credentials | object    | credentials to authenticate     |

##### authentication types

-   **username**: authenticate via username and password  
    `{"username": <username>, "password": <password>}`

## Authentication

By default, webfuse daemon will redirect each filesystem call to the first connected provider without any authentication. 
This might be good for testing purposes or when an external authentication mechanism is used. In some use cases, explicit authentication is needed. Therefore, authentication can be enabled within webfuse daemon.

When authentication is enabled, filesystem calls are only redirected to a connected provider, after `authenticate`
has succeeded.

![authenticate](doc/authenticate.png)

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

## Build and run

To install dependencies, see below.

    cd webfuse
    mkdir .build
    cd .build
    cmake ..
    mkdir test
    ./webfused -m test --document_root=../exmaple/daemon/www --port=4711

### Build options

By default, unit tests and example application are enabled. You can disable them using the following cmake options:

-   **WITHOUT_TESTS**: disable tests  
    `cmake -DWITHOUT_TESTS=ON ..`

-   **WITHOUT_EXAMPLE**: disable example  
    `cmake -DWITHOUD_EXAMPLE=ON ..`

## Dependencies

-   [libfuse3](https://github.com/libfuse/libfuse/)
-   [libwebsockets](https://libwebsockets.org/)
-   [Jansson](https://jansson.readthedocs.io)
-   [GoogleTest](https://github.com/google/googletest) *(optional)*

### Installation from source

#### libfuse

    wget -O fuse-3.1.1.tar.gz https://github.com/libfuse/libfuse/archive/fuse-3.1.1.tar.gz
    tar -xf fuse-3.1.1.tar.gz
    cd libfuse-fuse-3.1.1
    ./makeconf.sh
    ./configure
    make
    sudo make install

#### libwebsockets

    wget -O libwebsockets-3.1.0.tar.gz https://github.com/warmcat/libwebsockets/archive/v3.1.0.tar.gz
    tar -xf libwebsockets-3.1.0.tar.gz
    cd libwebsockets-3.1.0
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install

#### Jansson

    wget -O libjansson-2.12.tar.gz https://github.com/akheron/jansson/archive/v2.12.tar.gz
    tar -xf libjansson-2.12.tar.gz
    cd jansson-2.12
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install

#### GoogleTest

Installation of GoogleTest is optional webfuse library, but required to compile tests.

    wget -O gtest-1.8.1.tar.gz https://github.com/google/googletest/archive/release-1.8.1.tar.gz
    tar -xf gtest-1.8.1.tar.gz
    cd googletest-release-1.8.1
    mkdir .build
    cd .build
    cmake ..
    make
    sudo make install
