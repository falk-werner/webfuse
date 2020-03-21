if(NOT WITHOUT_TESTS AND NOT WITHOUT_ADAPTER AND NOT WITHOUT_PROVIDER)

include (CTest)

pkg_check_modules(GTEST gtest_main)
include(GoogleTest)
pkg_check_modules(GMOCK gmock)

add_executable(alltests
	lib/wf/jsonrpc/test/wf/jsonrpc/test_is_request.cc
	lib/wf/jsonrpc/test/wf/jsonrpc/test_request.cc
	lib/wf/jsonrpc/test/wf/jsonrpc/test_is_response.cc
	lib/wf/jsonrpc/test/wf/jsonrpc/test_response.cc
	lib/wf/jsonrpc/test/wf/jsonrpc/test_server.cc
	lib/wf/jsonrpc/test/wf/jsonrpc/test_proxy.cc
	lib/wf/jsonrpc/test/wf/jsonrpc/test_response_parser.cc
	lib/wf/timer/test/wf/timer/test_timepoint.cc
	lib/wf/timer/test/wf/timer/test_timer.cc
	test/webfuse/utils/tempdir.cc
	test/webfuse/utils/file_utils.cc
	test/webfuse/utils/die_if.cc
	test/webfuse/utils/timeout_watcher.cc
	test/webfuse/utils/path.c
	test/webfuse/utils/static_filesystem.c
	test/webfuse/utils/ws_server.cc
	test/webfuse/mocks/mock_authenticator.cc
	test/webfuse/mocks/mock_request.cc
	test/webfuse/mocks/mock_provider_client.cc
	test/webfuse//tests/core/test_util.cc
	test/webfuse/tests/core/test_container_of.cc
	test/webfuse/tests/core/test_string.cc
	test/webfuse/tests/core/test_slist.cc
	test/webfuse/tests/core/test_base64.cc
	test/webfuse/tests/core/test_status.cc
	test/webfuse/tests/core/test_message.cc
	test/webfuse/tests/core/test_message_queue.cc
	test/webfuse/tests/adapter/test_server.cc
	test/webfuse/tests/adapter/test_server_config.cc
	test/webfuse/tests/adapter/test_credentials.cc
	test/webfuse/tests/adapter/test_authenticator.cc
	test/webfuse/tests/adapter/test_authenticators.cc
	test/webfuse/tests/adapter/test_mountpoint.cc
	test/webfuse/tests/adapter/test_fuse_req.cc
	test/webfuse/tests/provider/test_url.cc
	test/webfuse/tests/provider/test_client_protocol.cc
	test/webfuse/tests/integration/test_integration.cc
	test/webfuse/tests/integration/server.cc
	test/webfuse/tests/integration/provider.cc
)

target_include_directories(alltests PRIVATE
	lib/wf/jsonrpc/include
	lib/wf/jsonrpc/src
	lib/wf/timer/include
	lib/wf/timer/src
	${FUSE3_INCLUDE_DIRS} 
)

target_compile_options(alltests PUBLIC
	${FUSE3_CFLAGS_OTHER}
)

target_link_libraries(alltests PUBLIC 
	webfuse-adapter-static
	webfuse-provider-static
	webfuse-core
	wf_jsonrpc
	wf_timer
	${FUSE3_LIBRARIES}
	${LWS_LIBRARIES}
	${JANSSON_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	${GMOCK_LIBRARIES}
	${GTEST_LIBRARIES}
)

target_include_directories(alltests PUBLIC test lib ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
target_compile_options(alltests PUBLIC ${GMOCK_CFLAGS} ${GTEST_CFLAGS})

enable_testing()
gtest_discover_tests(alltests TEST_PREFIX alltests:)

add_custom_target(coverage
	mkdir -p coverage
	COMMAND lcov --initial --capture --directory . --output-file coverage/lcov_base.info --rc lcov_branch_coverage=1
	COMMAND ./alltests
	COMMAND lcov --capture --directory . --output-file coverage/lcov.info --rc lcov_branch_coverage=1
	COMMAND lcov --remove coverage/lcov.info '/usr/*' --output-file coverage/lcov.info --rc lcov_branch_coverage=1
	COMMAND lcov --remove coverage/lcov.info '*/test/*' --output-file coverage/lcov.info --rc lcov_branch_coverage=1
)
add_dependencies(coverage alltests)

add_custom_target(coverage-report
	COMMAND genhtml --branch-coverage --highlight --legend --prefix "${CMAKE_SOURCE_DIR}" coverage/lcov.info --output-directory coverage/report
)
add_dependencies(coverage-report coverage)

endif(NOT WITHOUT_TESTS AND NOT WITHOUT_ADAPTER AND NOT WITHOUT_PROVIDER)
