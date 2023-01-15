# webfuse_provider command line options

Inject a remote filesystem via webfuse.

## Usage

    webfuse_provider -u <url> [-p <path>] [-a <cert_path>]

## Options

| Short Option | Long Option | Argument | Description |
| ------------ | ----------- | -------- | ----------- |
| -h           | --help      | -        | print usage and exit |
| -v           | --version   | -        | print version an exit |
| -p           | --path      | path     | path of local filesystem to inject (default: .) |
| -u           | --url       | url      | url of webfuse server |
| -a           | --ca-path   | path     | path of ca file |

## Examples

- inject current directory:  
  `webfuse_provider -u ws://localhost/`
- inject a given directory:  
  `webfuse_provider -u ws://localhost/ -p /path/to/directory`
- inject current directory to port 8080:  
  `webfuse_provider -u ws://localhost:8080/`
- inject current directory via TLS:  
  `webfuse_provider -u wss://localhost/`
- inject current diectory via TLS using a specific ca:  
  `webfuse_provider -u wss://localhost/ -a /path/to/server-cert.pem`
