cmake_minimum_required (VERSION 3.0)
project(fuse-wsfs VERSION 0.1.0 DESCRIPTION "Websocket filesystem based on libfuse")

option(WITH_TESTS "enable unit tests" OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE3 REQUIRED fuse3)
pkg_check_modules(LWS REQUIRED libwebsockets)
pkg_check_modules(JANSSON REQUIRED jansson)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -Wextra")

set(EXTRA_INCLUDE_DIRS
	"inc"
	${FUSE3_INCLUDE_DIRS} 
	${LWS_INCLUDE_DIRS} 
	${JANSSON_INCLUDE_DIRS}
)

set(EXTRA_LIBS 
	${EXTRA_LIBS}
	${FUSE3_LIBRARIES}
	${LWS_LIBRARIES}
	${JANSSON_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

set(EXTRA_CFLAGS
	${FUSE3_CFLAGS_OTHER}
	${LWS_CFLAGS_OTHER}
	${JANSSON_CFLAGS_OTHER}
	"-pthread"
)


# libfuse-wsfs

set(FUSE_WSFS_SOURCES
	src/wsfs/status.c
	src/wsfs/filesystem.c
	src/wsfs/server.c
	src/wsfs/message.c
	src/wsfs/message_queue.c
	src/wsfs/time/timepoint.c
	src/wsfs/time/timer.c
	src/wsfs/time/timeout_manager.c
	src/wsfs/operation/lookup.c
	src/wsfs/operation/getattr.c
	src/wsfs/operation/readdir.c
	src/wsfs/operation/open.c
	src/wsfs/operation/close.c
	src/wsfs/operation/read.c
	src/wsfs/server_config.c
	src/wsfs/server_protocol.c
	src/wsfs/jsonrpc/server.c
	src/wsfs/jsonrpc/method.c
	src/wsfs/jsonrpc/request.c
	src/wsfs/jsonrpc/response.c
	src/wsfs/jsonrpc/util.c
)

add_library(fuse-wsfs SHARED ${FUSE_WSFS_SOURCES})

set_target_properties(fuse-wsfs PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(fuse-wsfs PROPERTIES SOVERSION 0)
set_target_properties(fuse-wsfs PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(fuse-wsfs PROPERTIES COMPILE_DEFINITIONS "WSFS_API=WSFS_EXPORT")

target_include_directories(fuse-wsfs PUBLIC src ${EXTRA_INCLUDE_DIRS})
target_compile_options(fuse-wsfs PUBLIC  ${EXTRA_CFLAGS})

add_library(fuse-wsfs-static STATIC ${FUSE_WSFS_SOURCES})

set_target_properties(fuse-wsfs-static PROPERTIES OUTPUT_NAME fuse-wsfs)

target_include_directories(fuse-wsfs-static PUBLIC src ${EXTRA_INCLUDE_DIRS})
target_compile_options(fuse-wsfs-static PUBLIC ${EXTRA_CFLAGS})


# app

add_executable(wsfs
	src/app/main.c
)

target_link_libraries(wsfs PUBLIC fuse-wsfs ${EXTRA_LIBS})
target_include_directories(wsfs PUBLIC ${EXTRA_INCLUDE_DIRS})
target_compile_options(wsfs PUBLIC ${EXTRA_CFLAGS})

# tests

if(WITH_TESTS)

pkg_check_modules(GTEST gtest_main)

add_executable(alltests
	test-src/test_response_parser.cc
	test-src/test_server.cc
	test-src/test_timepoint.cc
	test-src/test_timer.cc
)

target_link_libraries(alltests PUBLIC fuse-wsfs-static ${EXTRA_LIBS} ${GTEST_LIBRARIES})
target_include_directories(alltests PUBLIC src ${EXTRA_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
target_compile_options(alltests PUBLIC ${EXTRA_CFLAGS} ${GTEST_CFLAGS})

enable_testing()
add_test(alltests alltests)

endif(WITH_TESTS)