project('webfuse', 'c', 'cpp', version: '0.3.0', license: 'LGPL-3.0+')

libwebsockets_dep = dependency('libwebsockets', version: '>=4.0.1') 
jansson_dep = dependency('jansson', version: '>=2.11', fallback: ['jansson', 'jansson_dep'])

pkg_config = import('pkgconfig')

inc_dir = include_directories('include')
private_inc_dir = include_directories('include', 'lib')

webfuse_core = static_library('webfuse_core',
    'lib/webfuse/core/slist.c',
	'lib/webfuse/core/message.c',
	'lib/webfuse/core/message_queue.c',
	'lib/webfuse/core/status.c',
	'lib/webfuse/core/string.c',
	'lib/webfuse/core/base64.c',
	'lib/webfuse/core/lws_log.c',
	'lib/webfuse/core/json_util.c',
    'lib/webfuse/core/timer/manager.c',
    'lib/webfuse/core/timer/timepoint.c',
    'lib/webfuse/core/timer/timer.c',
	'lib/webfuse/core/jsonrpc/proxy.c',
	'lib/webfuse/core/jsonrpc/proxy_variadic.c',
	'lib/webfuse/core/jsonrpc/server.c',
	'lib/webfuse/core/jsonrpc/method.c',
	'lib/webfuse/core/jsonrpc/request.c',
	'lib/webfuse/core/jsonrpc/response.c',
	'lib/webfuse/core/jsonrpc/error.c',
    c_args: ['-fvisibility=hidden'],
    include_directories: private_inc_dir,    
    dependencies: [jansson_dep, libwebsockets_dep])

webfuse_core_dep = declare_dependency(
    include_directories: inc_dir,
    link_with: webfuse_core)

webfuse_provider_static = static_library('webfuse_provider',
	'lib/webfuse/provider/api.c',
	'lib/webfuse/provider/impl/url.c',
	'lib/webfuse/provider/impl/client.c',
	'lib/webfuse/provider/impl/client_config.c',
	'lib/webfuse/provider/impl/client_protocol.c',
	'lib/webfuse/provider/impl/provider.c',
	'lib/webfuse/provider/impl/request.c',
	'lib/webfuse/provider/impl/dirbuffer.c',
	'lib/webfuse/provider/impl/credentials.c',
	'lib/webfuse/provider/impl/operation/lookup.c',
	'lib/webfuse/provider/impl/operation/getattr.c',
	'lib/webfuse/provider/impl/operation/readdir.c',
	'lib/webfuse/provider/impl/operation/open.c',
	'lib/webfuse/provider/impl/operation/close.c',
	'lib/webfuse/provider/impl/operation/read.c',
    c_args: ['-fvisibility=hidden'],
    include_directories: private_inc_dir,
    dependencies: [webfuse_core_dep])

webfuse_provider_static_dep = declare_dependency(
    include_directories: inc_dir,
    link_with: webfuse_provider_static)

webfuse_provider = shared_library('webfuse_provider',
    'lib/webfuse/provider/api.c',
    version: meson.project_version(),
    c_args: ['-fvisibility=hidden', '-DWFP_API=WFP_EXPORT'],
    include_directories: private_inc_dir,
    dependencies: [webfuse_provider_static_dep])

pkg_config.generate(
    libraries: [webfuse_provider, jansson_dep, libwebsockets_dep],
    subdirs: '.',
    version: meson.project_version(),
    name: 'libwebfuse_provider',
    filebase: 'webfuse_provider',
    description: 'Provider library for websockets filesystem')