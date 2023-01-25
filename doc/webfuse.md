# webfuse command line options

In order to inject a remote filesystem, webfuse mounts a local
filesystem via fuse and exposes it's API via websockets.

## Usage

 webfuse [options] &lt;mountpoint&gt;

## Webfuse specific options

| Option       | Argument | Default   | Description |
| ------------ | -------- | --------- | ----------- |
| --wf-port    | port     | 8081      | Specify the port of the websocket server |
| --wf-vhost   | vhost    | localhost | Specify the name of the websocket server's virtual host |
| --wf-cert    | path     | -         | Optional. Specify the file path of the server's public certificate |
| --wf-key     | path     | -         | Optional. Specify the file path of the server's private key |
| --wf- authenticator | path | -      | Optional. Specify the file path of the authenticator executable |
| --wf-auth-header | name | -         | Optional. Specify the name of the HTTP header used for authentication |

## Fuse options

| Option                | Descripion |
| --------------------- | ---------- |
| -h, --help            | print help |
| -V   --version        | print version |
| -d   -o debug         | enable debug output (implies -f) |
| -f                    | foreground operation |
| -s                    | disable multi-threaded operation |
| -o clone_fd           | use separate fuse device fd for each thread |
|                       | (may improve performance) |
| -o max_idle_threads   | the maximum number of idle worker threads |
|                       | allowed (default: 10) |
| -o kernel_cache       | cache files in kernel |
| -o [no]auto_cache     | enable caching based on modification times (off) |
| -o umask=M            | set file permissions (octal) |
| -o uid=N              | set file owner |
| -o gid=N              | set file group |
| -o entry_timeout=T    | cache timeout for names (1.0s) |
| -o negative_timeout=T | cache timeout for deleted names (0.0s) |
| -o attr_timeout=T     | cache timeout for attributes (1.0s) |
| -o ac_attr_timeout=T  | auto cache timeout for attributes (attr_timeout) |
| -o noforget           | never forget cached inodes |
| -o remember=T         | remember cached inodes for T seconds (0s) |
| -o modules=M1[:M2...] | names of modules to push onto filesystem stack |
| -o allow_other        | allow access by all users |
| -o allow_root         | allow access by root |
| -o auto_unmount       | auto unmount on process termination |

### Options for subdir module

| Option                | Descripion |
| --------------------- | ---------- |
| -o subdir=DIR         | prepend this directory to all paths (mandatory) |
| -o [no]rellinks	    | transform absolute symlinks to relative |

### Options for iconv module

| Option                | Descripion |
| --------------------- | ---------- |
| -o from_code=CHARSET  | original encoding of file names (default: UTF-8) |
| -o to_code=CHARSET    | new encoding of the file names (default: UTF-8) |

## Examples

- run webfuse in foreground on default port:  
  `webfuse -f /path/to/mointpoint`
- run webfuse in forground on port 8080:  
  `webfuse -f --wf-port 8080 /path/to/mountpoint`
- run webfuse using TLS:  
  `webfuse -f --wf-cert /path/to/cert --wf-key /path/to/key /path/to/mountpoint`
- run webfuse using authentication via `X-Auth-Token` header:  
  `webfuse -f --wf-authenticator /path/to/authenticator \`  
  `  --wf-auth-header X-Auth-Token /path/to/mountpoint`
