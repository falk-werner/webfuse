[![build](https://github.com/falk-werner/webfuse/actions/workflows/build.yml/badge.svg)](https://github.com/falk-werner/webfuse/actions/workflows/build.yml)

# webfuse

webfuse combines libwebsockets and libfuse. It allows to attach a remote filesystem via websockets.

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
Therefore, webfuse defined two roles participating in a webfuse connection:

- webfuse service
- webfuse provider

### Webfuse Service

A `webfuse service` is both,
- a [websocket](https://en.wikipedia.org/wiki/WebSocket) service providing the `webfuse` protocol
- a [fuse](https://github.com/libfuse/libfuse) filesystem attached to a local mountpoint

The `webfuse service` awaits incoming connections from a `webfuse provider`. Once connected, it communicates all the filesystem requests originated by the `libfuse` to the connected `webfuse provider` using the `websocket`-based `webfuse protocol`.

By doing so, `webfuse` allows to inject a filesystem to a remote device.

### Webfuse Provider

A `webfuse provider` provides a filesystem to a remote device using the `websocket`-based `webfuse protocol`. Therefore, a `webfuse provider` implements a `websocket` client.


## Similar Projects

### Davfs2

[davfs2](http://savannah.nongnu.org/projects/davfs2) is a Linux file system driver that allows to mount a [WebDAV](https://wiki.archlinux.org/index.php/WebDAV) resource. WebDAV is an extension to HTTP/1.1 that allows remote collaborative authoring of Web resources.

Unlike webfuse, davfs2 mounts a remote filesystem locally, that is provided by a WebDAV server. In contrast, webfuse starts a server awaiting client connections to attach the remote file system.

## Further Documentation

-   [Build instructions](doc/build.md)
-   [Webfuse command line options](doc/webfuse.md)
-   [Webfuse provider command line options](doc/webfuse_provider.md)
-   [Webfuse Protocol](doc/protocol.md)
-   [Authentication](doc/authentication.md)

## webfuse legacy

`webfuse2` is a complete re-implementation of the original idea behind `webfuse`. In contrast to the original `webfuse` implementation, `webfuse2` provides also write access to the filesystem and allows to use all standard options of a fuse filesystem.

But `webfuse2` marks also some breaking changes:

- `webfuse2` uses a new, binary protocol which is not compatible to the JSON-based protocol of legacy `webfuse`
- `webfuse` does not provide an API nor a library to program against  
  _(if you are interested in an API or a library for webfuse2 feel free to create an issue)_

When you are interested in the original `webfuse` implementation take a look at this [branch](https://github.com/falk-werner/webfuse/tree/webfuse-legacy).
