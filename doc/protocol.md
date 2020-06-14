# Webfuse Protocol

## Requests, responses and notifications

There are three types of messages, used for communication between webfuse daemon and filesystem provider. All message types are encoded in [JSON](https://www.json.org/) and strongly inspired by [JSON-RPC](https://www.jsonrpc.org/).

### Request

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

### Response

A response is used to answer a prior request. There are two kinds of responses:

#### Successful Results

    {
       "result": <result>,
       "id": <id>
    }

| Item        | Data type | Description             |
| ----------- |:---------:| ----------------------- |
| result      | any       | request specific result |
| id          | integer   | id, same as request     |

#### Error notifications

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

#### Error codes

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

### Notification

Notfications are used to inform a receiver about something. Unlike requests, notifications are not answered. Therefore, an id is not supplied.

    {
      "method": <method_name>,
      "params": <params>
    }

| Item        | Data type | Description                       |
| ----------- |:---------:| --------------------------------- |
| method_name | string    | name of the method to invoke      |
| params      | array     | method specific parameters        |

## Requests (Adapter -> Provider)

### lookup

Retrieve information about a filesystem entry by name.

    webfuse daemon: {"method": "lookup", "params": [<filesystem>, <parent>, <name>], "id": <id>}
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
| filesystem  | string          | name of the filesystem                      |
| parent      | integer         | inode of parent directory (1 = root)        |
| name        | string          | name of the filesystem object to look up    |
| inode       | integer         | inode of the filesystem object              |
| mode        | integer         | unix file mode                              |
| type        | "file" or "dir" | type of filesystem object                   |
| size        | integer         | required for files; file size in bytes      |
| atime       | integer         | optional; unix time of last access          |
| mtime       | integer         | optional; unix time of last modification    |
| ctime       | intefer         | optional; unix time of last metadata change |

### getattr

Get file attributes.

    webfuse daemon: {"method": "getattr", "params": [<filesystem>, <inode>], "id": <id>}
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
| filesystem  | string          | name of the filesystem                      |
| inode       | integer         | inode of the filesystem object              |
| mode        | integer         | unix file mode                              |
| type        | "file" or "dir" | type of filesystem object                   |
| size        | integer         | required for files; file size in bytes      |
| atime       | integer         | optional; unix time of last access          |
| mtime       | integer         | optional; unix time of last modification    |
| ctime       | intefer         | optional; unix time of last metadata change |

### readdir

Read directory contents.  
Result is an array of name-inode pairs for each entry. The generic entries
"." and ".." should also be provided.

    webfuse daemon: {"method": "readdir", "params": [<filesystem>, <dir_inode>], "id": <id>}
    fs provider: {"result": [
        {"name": <name>, "inode": <inode>},
        ...
        ], "id": <id>}

| Item        | Data type       | Description                    |
| ----------- | --------------- | ------------------------------ |
| filesystem  | string          | name of the filesystem         |
| dir_inode   | integer         | inode of the directory to read |
| name        | integer         | name of the entry              |
| inode       | integer         | inode of the entry             |

### open

Open a file.

    webfuse daemon: {"method": "readdir", "params": [<filesystem>, <inode>, <flags>], "id": <id>}
    fs provider: {"result": {"handle": <handle>}, "id": <id>}

| Item        | Data type | Description                   |
| ----------- | ----------| ----------------------------- |
| filesystem  | string    | name of the filesystem        |
| inode       | integer   | inode of the file             |
| flags       | integer   | access mode flags (see below) |
| handle      | integer   | handle of the file            |

#### Flags

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

### close

Informs filesystem provider, that a file is closed.  
Since `close` is a notification, it cannot fail.

    webfuse daemon: {"method": "close", "params": [<filesystem>, <inode>, <handle>, <flags>], "id": <id>}

| Item        | Data type | Description                  |
| ----------- | ----------| ---------------------------- |
| filesystem  | string    | name of the filesystem       |
| inode       | integer   | inode of the file            |
| handle      | integer   | handle of the file           |
| flags       | integer   | access mode flags (see open) |

### read

Read from an open file.

    webfuse daemon: {"method": "close", "params": [<filesystem>, <inode>, <handle>, <offset>, <length>], "id": <id>}
    fs provider: {"result": {
        "data": <data>,
        "format": <format>,
        "count": <count>
        }, "id": <id>}

| Item        | Data type | Description                   |
| ----------- | ----------| ----------------------------- |
| filesystem  | string    | name of the filesystem        |
| inode       | integer   | inode of the file             |
| handle      | integer   | handle of the file            |
| offset      | integer   | Offet to start read operation |
| length      | integer   | Max. number of bytes to read  |
| data        | integer   | handle of the file            |
| format      | string    | Encoding of data (see below)  |
| count       | integer   | Actual number of bytes read   |

#### Format

| Format     | Description                                              |
| ---------- | -------------------------------------------------------- |
| "identiy"  | Use data as is; note that JSON strings are UTF-8 encoded |
| "base64"   | data is base64 encoded                                   |

## Requests (Client -> Server)

_Note:_ The following requests are initiated by the client and
responded by the server. Depending on the use case, a provider
can be a client (e.g. a webbrowser which want's to inject a
filesystem to an IoT device) or a server (e.g. a cloud based
service, an adapter connects to and requests a filesystem).

### add_filesystem

Adds a filesystem.

    client: {"method": "add_filesystem", "params": [<name>], "id": <id>}
    server: {"result": {"id": <name>}, "id": <id>}

| Item        | Data type | Description                     |
| ----------- | ----------| ------------------------------- |
| name        | string    | name and id of filesystem       |

### authtenticate

Authenticate the provider.  
If authentication is enabled, a client must be authenticated by the server before filesystems can be added.

    client: {"method": "authenticate", "params": [<type>, <credentials>], "id": <id>}
    server: {"result": {}, "id": <id>}

| Item        | Data type | Description                     |
| ----------- | ----------| ------------------------------- |
| type        | string    | authentication type (see below) |
| credentials | object    | credentials to authenticate     |

#### authentication types

-   **username**: authenticate via username and password  
    `{"username": <username>, "password": <password>}`
