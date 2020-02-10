if(NOT WITHOUT_TESTS AND NOT WITHOUT_ADAPTER AND NOT WITHOUT_PROVIDER)

include (CTest)

pkg_check_modules(GTEST gtest_main)
include(GoogleTest)
pkg_check_modules(GMOCK gmock)

add_executable(alltests
	test/msleep.cc
	test/die_if.cc
	test/mock_authenticator.cc
	test/mock_request.cc
	test/core/test_container_of.cc
	test/core/test_string.cc
	test/core/test_slist.cc
	test/core/test_path.cc
	test/core/test_base64.cc
	test/core/test_status.cc
	test/core/test_message.cc
	test/core/test_message_queue.cc
	test/adapter/test_response_parser.cc
	test/adapter/test_server.cc
	test/adapter/test_timepoint.cc
	test/adapter/test_timer.cc
	test/adapter/test_credentials.cc
	test/adapter/test_authenticator.cc
	test/adapter/test_authenticators.cc
	test/adapter/test_fuse_req.cc
	test/adapter/jsonrpc/test_util.cc
	test/adapter/jsonrpc/test_is_request.cc
	test/adapter/jsonrpc/test_request.cc
	test/adapter/jsonrpc/test_is_response.cc
	test/adapter/jsonrpc/test_response.cc
	test/adapter/jsonrpc/test_server.cc
	test/adapter/jsonrpc/test_proxy.cc
	test/provider/test_url.cc
	test/provider/test_static_filesystem.cc
	test/integration/test_integration.cc
	test/integration/server.cc
	test/integration/provider.cc
)

target_include_directories(alltests PRIVATE
	${FUSE3_INCLUDE_DIRS} 
	${UUID_INCLUDE_DIRS}
)

target_compile_options(alltests PUBLIC
	${FUSE3_CFLAGS_OTHER}
	${UUID_CFLAGS_OTHER}
)

target_link_libraries(alltests PUBLIC 
	webfuse-adapter-static
	webfuse-provider-static
	webfuse-core
	${FUSE3_LIBRARIES}
	${LWS_LIBRARIES}
	${JANSSON_LIBRARIES}
	${UUID_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	${GMOCK_LIBRARIES}
	${GTEST_LIBRARIES}
)

target_include_directories(alltests PUBLIC test lib ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
target_compile_options(alltests PUBLIC ${GMOCK_CFLAGS} ${GTEST_CFLAGS})

enable_testing()
gtest_discover_tests(alltests TEST_PREFIX alltests:)

add_custom_target(coverage
	./alltests
	COMMAND mkdir -p coverage
	COMMAND lcov --capture --directory . --output-file coverage/lcov.info
	COMMAND lcov --remove coverage/lcov.info '/usr/*' --output-file coverage/lcov.info
	COMMAND lcov --remove coverage/lcov.info '*/test/*' --output-file coverage/lcov.info
)
add_dependencies(coverage alltests)

add_custom_target(coverage-report
	COMMAND genhtml coverage/lcov.info --output-directory coverage/report
)
add_dependencies(coverage-report coverage)

endif(NOT WITHOUT_TESTS AND NOT WITHOUT_ADAPTER AND NOT WITHOUT_PROVIDER)
