project('webfuse', 'c', 'cpp', version: '0.4.0', license: 'LGPL-3.0+')

without_tests = get_option('without_tests')

libwebsockets_dep = dependency('libwebsockets', version: '>=4.0.13', required: false)
if not libwebsockets_dep.found()
	cmake = import('cmake')
	libwebsockets = cmake.subproject('libwebsockets')
	libwebsockets_dep = libwebsockets.dependency('websockets_shared')
endif

jansson_dep = dependency('jansson', version: '>=2.11', fallback: ['jansson', 'jansson_dep'])

pkg_config = import('pkgconfig')

inc_dir = include_directories('include')
private_inc_dir = include_directories('include', 'lib')

install_subdir('include/webfuse/core', install_dir: 'include/webfuse')


libfuse_dep = dependency('fuse3', version: '>=3.8.0', fallback: ['fuse3', 'libfuse_dep'])

webfuse_static = static_library('webfuse',
    'lib/webfuse/core/slist.c',
	'lib/webfuse/core/message.c',
	'lib/webfuse/core/message_queue.c',
	'lib/webfuse/core/status.c',
	'lib/webfuse/core/base64.c',
	'lib/webfuse/core/lws_log.c',
	'lib/webfuse/core/json_util.c',
	'lib/webfuse/core/url.c',
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
	'lib/webfuse/adapter/api.c',
	'lib/webfuse/adapter/impl/filesystem.c',
	'lib/webfuse/adapter/impl/server.c',
	'lib/webfuse/adapter/impl/server_config.c',
	'lib/webfuse/adapter/impl/server_protocol.c',
	'lib/webfuse/adapter/impl/session.c',
	'lib/webfuse/adapter/impl/session_manager.c',
	'lib/webfuse/adapter/impl/authenticator.c',
	'lib/webfuse/adapter/impl/authenticators.c',
	'lib/webfuse/adapter/impl/credentials.c',
	'lib/webfuse/adapter/impl/mountpoint.c',
	'lib/webfuse/adapter/impl/mountpoint_factory.c',
	'lib/webfuse/adapter/impl/operation/context.c',
	'lib/webfuse/adapter/impl/operation/lookup.c',
	'lib/webfuse/adapter/impl/operation/getattr.c',
	'lib/webfuse/adapter/impl/operation/readdir.c',
	'lib/webfuse/adapter/impl/operation/open.c',
	'lib/webfuse/adapter/impl/operation/close.c',
	'lib/webfuse/adapter/impl/operation/read.c',
	'lib/webfuse/adapter/impl/client.c',
	'lib/webfuse/adapter/impl/client_protocol.c',
	'lib/webfuse/adapter/impl/client_tlsconfig.c',
    c_args: ['-fvisibility=hidden'],
    include_directories: private_inc_dir,
    dependencies: [libfuse_dep, libwebsockets_dep, jansson_dep])

webfuse_static_dep = declare_dependency(
	include_directories: inc_dir,
	link_with: [webfuse_static],
	dependencies: [libfuse_dep, libwebsockets_dep, jansson_dep])

webfuse = shared_library('webfuse',
    'lib/webfuse/adapter/api.c',
    version: meson.project_version(),
    c_args: ['-fvisibility=hidden', '-DWF_API=WF_EXPORT'],
    include_directories: private_inc_dir,
    dependencies: [webfuse_static_dep, libfuse_dep, libwebsockets_dep, jansson_dep],
	install: true)

webfuse_dep = declare_dependency(
	include_directories: inc_dir,
	link_with: [webfuse],
	dependencies: [libfuse_dep, libwebsockets_dep, jansson_dep])

install_headers('include/webfuse_adapter.h', subdir: 'webfuse')
install_subdir('include/webfuse/adapter', install_dir: 'include/webfuse')

pkg_config.generate(
    libraries: [webfuse],
	requires: ['fuse3', 'libwebsockets', 'jansson'],
    subdirs: '.',
    version: meson.project_version(),
    name: 'libwebfuse',
    filebase: 'webfuse',
    description: 'Websockets filesystem adapter library')

# Unit Tests

if not without_tests

gtest_dep = dependency('gtest', version: '>=1.10.0', fallback: ['gtest', 'gtest_dep']) 
gmock_main_dep = dependency('gmock_main', version: '>=1.10.0', fallback: ['gtest', 'gmock_main_dep'])

fscheck = executable('fs_check',
	'test/webfuse/tests/integration/fs_check.c')

openssl = find_program('openssl')
test_server_certs = custom_target('test_server_certs',
	output: ['server-key.pem', 'server-cert.pem'],
	command: [openssl, 'req', '-x509', '-newkey', 'rsa:4096', '-keyout', 'server-key.pem', '-out', 'server-cert.pem', '-days', '365', '-nodes', '-batch', '-subj', '/CN=localhost'])
test_client_certs = custom_target('test_client_certs',
	output: ['client-key.pem', 'client-cert.pem'],
	command: [openssl, 'req', '-x509', '-newkey', 'rsa:4096', '-keyout', 'client-key.pem', '-out', 'client-cert.pem', '-days', '365', '-nodes', '-batch', '-subj', '/CN=localhost'])

test_certs_dep = declare_dependency(
	sources: [test_server_certs, test_client_certs])

alltests = executable('alltests',
	'test/webfuse/tests/core/jsonrpc/mock_timer_callback.cc',
	'test/webfuse/tests/core/jsonrpc/mock_timer.cc',
	'test/webfuse/tests/core/jsonrpc/test_is_request.cc',
	'test/webfuse/tests/core/jsonrpc/test_request.cc',
	'test/webfuse/tests/core/jsonrpc/test_is_response.cc',
	'test/webfuse/tests/core/jsonrpc/test_response.cc',
	'test/webfuse/tests/core/jsonrpc/test_server.cc',
	'test/webfuse/tests/core/jsonrpc/test_proxy.cc',
	'test/webfuse/tests/core/jsonrpc/test_response_parser.cc',
	'test/webfuse/tests/core/timer/test_timepoint.cc',
	'test/webfuse/tests/core/timer/test_timer.cc',
	'test/webfuse/utils/tempdir.cc',
	'test/webfuse/utils/file_utils.cc',
	'test/webfuse/utils/timeout_watcher.cc',
	'test/webfuse/utils/path.c',
	'test/webfuse/utils/ws_server.cc',
	'test/webfuse/utils/ws_server2.cc',
	'test/webfuse/utils/adapter_client.cc',
	'test/webfuse/utils/jansson_test_environment.cc',
	'test/webfuse/mocks/fake_invokation_context.cc',
	'test/webfuse/mocks/mock_authenticator.cc',
	'test/webfuse/mocks/mock_request.cc',
	'test/webfuse/mocks/mock_provider.cc',
	'test/webfuse/mocks/mock_fuse.cc',
	'test/webfuse/mocks/mock_operation_context.cc',
	'test/webfuse/mocks/mock_jsonrpc_proxy.cc',
	'test/webfuse/mocks/mock_adapter_client_callback.cc',
	'test/webfuse//tests/core/test_util.cc',
	'test/webfuse/tests/core/test_container_of.cc',
	'test/webfuse/tests/core/test_slist.cc',
	'test/webfuse/tests/core/test_base64.cc',
	'test/webfuse/tests/core/test_status.cc',
	'test/webfuse/tests/core/test_message.cc',
	'test/webfuse/tests/core/test_message_queue.cc',
	'test/webfuse/tests/core/test_url.cc',
	'test/webfuse/tests/adapter/test_server.cc',
	'test/webfuse/tests/adapter/test_server_config.cc',
	'test/webfuse/tests/adapter/test_credentials.cc',
	'test/webfuse/tests/adapter/test_authenticator.cc',
	'test/webfuse/tests/adapter/test_authenticators.cc',
	'test/webfuse/tests/adapter/test_mountpoint.cc',
	'test/webfuse/tests/adapter/test_fuse_req.cc',
	'test/webfuse/tests/adapter/operation/test_context.cc',
	'test/webfuse/tests/adapter/operation/test_open.cc',
	'test/webfuse/tests/adapter/operation/test_close.cc',
	'test/webfuse/tests/adapter/operation/test_read.cc',
	'test/webfuse/tests/adapter/operation/test_readdir.cc',
	'test/webfuse/tests/adapter/operation/test_getattr.cc',
	'test/webfuse/tests/adapter/operation/test_lookup.cc',
	'test/webfuse/tests/integration/file.cc',
	'test/webfuse/tests/adapter/test_client.cc',
	'test/webfuse/tests/adapter/test_client_tlsconfig.cc',
	link_args: [
		'-Wl,--wrap=wf_timer_manager_create',
		'-Wl,--wrap=wf_timer_manager_dispose',
		'-Wl,--wrap=wf_timer_manager_check',
		'-Wl,--wrap=wf_timer_create',
		'-Wl,--wrap=wf_timer_dispose',
		'-Wl,--wrap=wf_timer_start',
		'-Wl,--wrap=wf_timer_cancel',
		'-Wl,--wrap=wf_impl_operation_context_get_proxy',
		'-Wl,--wrap=wf_jsonrpc_proxy_vinvoke',
		'-Wl,--wrap=wf_jsonrpc_proxy_vnotify',
		'-Wl,--wrap=fuse_req_userdata',
		'-Wl,--wrap=fuse_reply_open',
		'-Wl,--wrap=fuse_reply_err',
		'-Wl,--wrap=fuse_reply_buf',
		'-Wl,--wrap=fuse_reply_attr',
		'-Wl,--wrap=fuse_reply_entry',
		'-Wl,--wrap=fuse_req_ctx'
	],
	include_directories: [private_inc_dir, 'test'],
	dependencies: [
		webfuse_static_dep,
		libwebsockets_dep,
		libfuse_dep,
		jansson_dep,
		gtest_dep,
		gmock_main_dep,
		test_certs_dep
	])

test('alltests', alltests)

endif