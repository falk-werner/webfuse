cmake_minimum_required (VERSION 3.10)
project(webfuse VERSION 0.2.0 DESCRIPTION "Websocket filesystem based on libfuse")

option(WITHOUT_TESTS   "disable unit tests" OFF)
option(WITHOUT_EXAMPLE "disable example"    OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE3 REQUIRED fuse3)
pkg_check_modules(LWS REQUIRED libwebsockets)
pkg_check_modules(JANSSON REQUIRED jansson)
pkg_check_modules(UUID REQUIRED uuid)

add_definitions(-D_FILE_OFFSET_BITS=64)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(C_WARNINGS -Wall -Wextra)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

add_definitions(
	-pg
	--coverage
	-fprofile-arcs
	-ftest-coverage
)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	link_libraries(gcov)
else()
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs")

endif()

include_directories(
	"include"
	${FUSE3_INCLUDE_DIRS} 
	${LWS_INCLUDE_DIRS} 
	${JANSSON_INCLUDE_DIRS}
	${UUID_INCLUDE_DIRS}
)

set(EXTRA_LIBS 
	${EXTRA_LIBS}
	${FUSE3_LIBRARIES}
	${LWS_LIBRARIES}
	${JANSSON_LIBRARIES}
	${UUID_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

add_compile_options(
	${C_WARNINGS}
	${FUSE3_CFLAGS_OTHER}
	${LWS_CFLAGS_OTHER}
	${JANSSON_CFLAGS_OTHER}
	${UUID_CFLAGS_OTHER}
	"-pthread"
)

# libwebfuse-core

add_library(webfuse-core STATIC 
	lib/webfuse/core/slist.c
	lib/webfuse/core/message.c
	lib/webfuse/core/message_queue.c
	lib/webfuse/core/status.c
	lib/webfuse/core/string.c
	lib/webfuse/core/path.c
)

set_target_properties(webfuse-core PROPERTIES OUTPUT_NAME webfuse-core)
target_include_directories(webfuse-core PUBLIC lib)
set_target_properties(webfuse-core PROPERTIES C_VISIBILITY_PRESET hidden)

install(DIRECTORY include/webfuse/core DESTINATION include/webfuse)


# libwebfuse-adapter

add_library(webfuse-adapter-static STATIC 
	lib/webfuse/adapter/api.c
	lib/webfuse/adapter/impl/filesystem.c
	lib/webfuse/adapter/impl/server.c
	lib/webfuse/adapter/impl/server_config.c
	lib/webfuse/adapter/impl/server_protocol.c
	lib/webfuse/adapter/impl/session.c
	lib/webfuse/adapter/impl/session_manager.c
	lib/webfuse/adapter/impl/authenticator.c
	lib/webfuse/adapter/impl/authenticators.c
	lib/webfuse/adapter/impl/credentials.c
	lib/webfuse/adapter/impl/operations.c
	lib/webfuse/adapter/impl/time/timepoint.c
	lib/webfuse/adapter/impl/time/timer.c
	lib/webfuse/adapter/impl/time/timeout_manager.c
	lib/webfuse/adapter/impl/operation/lookup.c
	lib/webfuse/adapter/impl/operation/getattr.c
	lib/webfuse/adapter/impl/operation/readdir.c
	lib/webfuse/adapter/impl/operation/open.c
	lib/webfuse/adapter/impl/operation/close.c
	lib/webfuse/adapter/impl/operation/read.c
	lib/webfuse/adapter/impl/jsonrpc/proxy.c
	lib/webfuse/adapter/impl/jsonrpc/server.c
	lib/webfuse/adapter/impl/jsonrpc/method.c
	lib/webfuse/adapter/impl/jsonrpc/request.c
	lib/webfuse/adapter/impl/jsonrpc/response.c
	lib/webfuse/adapter/impl/jsonrpc/util.c
)

set_target_properties(webfuse-adapter-static PROPERTIES OUTPUT_NAME webfuse-adapter)
set_target_properties(webfuse-adapter-static PROPERTIES C_VISIBILITY_PRESET hidden)
target_include_directories(webfuse-adapter-static PUBLIC lib)

add_library(webfuse-adapter SHARED 
	lib/webfuse/adapter/api.c
)

set_target_properties(webfuse-adapter PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(webfuse-adapter PROPERTIES SOVERSION 0)
set_target_properties(webfuse-adapter PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(webfuse-adapter PROPERTIES COMPILE_DEFINITIONS "WF_API=WF_EXPORT")

target_link_libraries(webfuse-adapter PRIVATE webfuse-adapter-static webfuse-core)

file(WRITE "${PROJECT_BINARY_DIR}/libwebfuse-adapter.pc"
"prefix=\"${CMAKE_INSTALL_PREFIX}\"

exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib${LIB_SUFFIX}
includedir=\${prefix}/include
Name: libwebfuse
Description: Websockets filesystem server library
Version: ${PROJECT_VERSION}

Libs: -L\${libdir} -lwebfuse-adapter -l${FUSE3_LIBRARIES} -l${LWS_LIBRARIES} -l${JANSSON_LIBRARIES}
Cflags: -I\${includedir}"
)

install(TARGETS webfuse-adapter DESTINATION lib${LIB_SUFFIX})
install(FILES include/webfuse_adapter.h DESTINATION include)
install(DIRECTORY include/webfuse/adapter DESTINATION include/webfuse)
install(FILES "${PROJECT_BINARY_DIR}/libwebfuse-adapter.pc" DESTINATION lib${LIB_SUFFIX}/pkgconfig)

#libwebfuse-provider

add_library(webfuse-provider-static STATIC 
	lib/webfuse/provider/api.c
	lib/webfuse/provider/impl/url.c
	lib/webfuse/provider/impl/client.c
	lib/webfuse/provider/impl/client_config.c
	lib/webfuse/provider/impl/client_protocol.c
	lib/webfuse/provider/impl/provider.c
	lib/webfuse/provider/impl/request.c
	lib/webfuse/provider/impl/dirbuffer.c
	lib/webfuse/provider/impl/operation/lookup.c
	lib/webfuse/provider/impl/operation/getattr.c
	lib/webfuse/provider/impl/operation/readdir.c
	lib/webfuse/provider/impl/operation/open.c
	lib/webfuse/provider/impl/operation/close.c
	lib/webfuse/provider/impl/operation/read.c
	lib/webfuse/provider/impl/static_filesystem.c
)

set_target_properties(webfuse-provider-static PROPERTIES OUTPUT_NAME webfuse-provider)
set_target_properties(webfuse-provider-static PROPERTIES C_VISIBILITY_PRESET hidden)
target_include_directories(webfuse-provider-static PUBLIC lib)

add_library(webfuse-provider SHARED 
	lib/webfuse/provider/api.c
)

set_target_properties(webfuse-provider PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(webfuse-provider PROPERTIES SOVERSION 0)
set_target_properties(webfuse-provider PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(webfuse-provider PROPERTIES COMPILE_DEFINITIONS "WFP_API=WFP_EXPORT")

target_include_directories(webfuse-provider PUBLIC lib)
target_link_libraries(webfuse-provider PRIVATE webfuse-provider-static webfuse-core)

file(WRITE "${PROJECT_BINARY_DIR}/libwebfuse-provider.pc"
"prefix=\"${CMAKE_INSTALL_PREFIX}\"
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib${LIB_SUFFIX}
includedir=\${prefix}/include
Name: libwebfuse-provider
Description: Provider library for websockets filesystem
Version: ${PROJECT_VERSION}

Libs: -L\${libdir} -lwebfuse-provider  -l${LWS_LIBRARIES} -l${JANSSON_LIBRARIES}
Cflags: -I\${includedir}"
)

install(TARGETS webfuse-provider DESTINATION lib${LIB_SUFFIX})
install(FILES include/webfuse_provider.h DESTINATION include)
install(DIRECTORY include/webfuse/provider DESTINATION include/webfuse)
install(FILES "${PROJECT_BINARY_DIR}/libwebfuse-provider.pc" DESTINATION lib${LIB_SUFFIX}/pkgconfig)


# examples

pkg_check_modules(OPENSSL REQUIRED openssl)

if(NOT WITHOUT_EXAMPLE)

# libuserdb

add_library(userdb STATIC 
	example/lib/userdb/src/userdb.c
)

target_include_directories(userdb PUBLIC
	example/lib/userdb/include
	${OPENSSL_INCLUDE_DIRS}
	${JANSSON_INCLUDE_DIRS}
)

target_compile_options(userdb PUBLIC ${OPENSSL_CFLAGS_OTHER})

# daemon

add_executable(webfused
	example/daemon/main.c
)

target_link_libraries(webfused PUBLIC webfuse-adapter userdb ${OPENSSL_LIBRARIES} ${EXTRA_LIBS})
target_compile_options(webfused PUBLIC ${OPENSSL_CFLAGS_OTHER})

# provider

add_executable(webfuse-provider-app
	example/provider/main.c
)

set_target_properties(webfuse-provider-app PROPERTIES OUTPUT_NAME webfuse-provider)

target_link_libraries(webfuse-provider-app PUBLIC webfuse-provider ${EXTRA_LIBS})
target_include_directories(webfuse-provider-app PUBLIC ${EXTRA_INCLUDE_DIRS})

# static-filesystem-provider

add_executable(static-filesystem-provider
	example/provider/static_filesystem.c
)

target_link_libraries(static-filesystem-provider PUBLIC webfuse-provider ${EXTRA_LIBS})
target_include_directories(static-filesystem-provider PUBLIC ${EXTRA_INCLUDE_DIRS})
target_compile_options(static-filesystem-provider PUBLIC ${EXTRA_CFLAGS})

# webfuse-passwd

add_executable(webfuse-passwd
	example/passwd/main.c
)

target_link_libraries(webfuse-passwd PUBLIC 
	userdb
	${OPENSSL_LIBRARIES}
	${JANSSON_LIBRARIES}
)

target_include_directories(webfuse-passwd PUBLIC 
	example/passwd
	example/lib/userdb/include
	${OPENSSL_INCLUDE_DIRS}
	${JANSSON_INCLUDE_DIRS}
)

target_compile_options(webfuse-passwd PUBLIC ${OPENSSL_CFLAGS_OTHER})


endif(NOT WITHOUT_EXAMPLE)

# tests

if(NOT WITHOUT_TESTS)

include (CTest)

pkg_check_modules(GTEST gtest_main)
include(GoogleTest)
pkg_check_modules(GMOCK gmock)

add_executable(alltests
	test/msleep.cc
	test/mock_authenticator.cc
	test/mock_request.cc
	test/core/test_container_of.cc
	test/core/test_string.cc
	test/core/test_slist.cc
	test/core/test_path.cc
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
	test/test_url.cc
	test/test_static_filesystem.cc
)

target_link_libraries(alltests PUBLIC webfuse-adapter-static webfuse-provider-static webfuse-core ${EXTRA_LIBS} ${GMOCK_LIBRARIES} ${GTEST_LIBRARIES})
target_include_directories(alltests PUBLIC test lib ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
target_compile_options(alltests PUBLIC ${GMOCK_CFLAGS} ${GTEST_CFLAGS})

enable_testing()
gtest_discover_tests(alltests TEST_PREFIX alltests:)

add_custom_target(coverage
	./alltests
	COMMAND mkdir -p coverage
	COMMAND lcov --capture --directory . --output-file coverage/lcov.info
	COMMAND lcov --remove coverage/lcov.info '/usr/*' --output-file coverage/lcov.info
)
add_dependencies(coverage alltests)

add_custom_target(coverage-report
	COMMAND genhtml coverage/lcov.info --output-directory coverage/report
)
add_dependencies(coverage-report coverage)

endif(NOT WITHOUT_TESTS)
