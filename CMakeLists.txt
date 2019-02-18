cmake_minimum_required (VERSION 3.1)
project(fuse-wsfs VERSION 0.1.0 DESCRIPTION "Websocket filesystem based on libfuse")

option(WITHOUT_TESTS   "disable unit tests" OFF)
option(WITHOUT_EXAMPLE "disable example"    OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE3 REQUIRED fuse3)
pkg_check_modules(LWS REQUIRED libwebsockets)
pkg_check_modules(JANSSON REQUIRED jansson)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(C_WARNINGS -Wall -Wextra)

set(EXTRA_INCLUDE_DIRS
	"include"
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
	${C_WARNINGS}
	${FUSE3_CFLAGS_OTHER}
	${LWS_CFLAGS_OTHER}
	${JANSSON_CFLAGS_OTHER}
	"-pthread"
)


# libwsfs

set(WSFS_SOURCES
	lib/wsfs/status.c
	lib/wsfs/filesystem.c
	lib/wsfs/server.c
	lib/wsfs/message.c
	lib/wsfs/message_queue.c
	lib/wsfs/time/timepoint.c
	lib/wsfs/time/timer.c
	lib/wsfs/time/timeout_manager.c
	lib/wsfs/operation/lookup.c
	lib/wsfs/operation/getattr.c
	lib/wsfs/operation/readdir.c
	lib/wsfs/operation/open.c
	lib/wsfs/operation/close.c
	lib/wsfs/operation/read.c
	lib/wsfs/server_config.c
	lib/wsfs/server_protocol.c
	lib/wsfs/jsonrpc/server.c
	lib/wsfs/jsonrpc/method.c
	lib/wsfs/jsonrpc/request.c
	lib/wsfs/jsonrpc/response.c
	lib/wsfs/jsonrpc/util.c
)

add_library(wsfs SHARED ${WSFS_SOURCES})

set_target_properties(wsfs PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(wsfs PROPERTIES SOVERSION 0)
set_target_properties(wsfs PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(wsfs PROPERTIES COMPILE_DEFINITIONS "WSFS_API=WSFS_EXPORT")

target_include_directories(wsfs PUBLIC lib ${EXTRA_INCLUDE_DIRS})
target_compile_options(wsfs PUBLIC  ${EXTRA_CFLAGS})

file(WRITE "${PROJECT_BINARY_DIR}/libwsfs.pc"
"prefix=\"${CMAKE_INSTALL_PREFIX}\"
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib${LIB_SUFFIX}
includedir=\${prefix}/include
Name: libwsfs
Description: Websockets filesystem server library
Version: ${PROJECT_VERSION}

Libs: -L\${libdir} -lwsfs -l${FUSE3_LIBRARIES} -l${LWS_LIBRARIES} -l${JANSSON_LIBRARIES}
Cflags: -I\${includedir}"
)

install(TARGETS wsfs DESTINATION lib${LIB_SUFFIX})
install(FILES include/wsfs.h DESTINATION include)
install(DIRECTORY include/wsfs DESTINATION include)
install(FILES "${PROJECT_BINARY_DIR}/libwsfs.pc" DESTINATION lib${LIB_SUFFIX}/pkgconfig)

#libwsfs-provider

set(WSFS_PROVIDER_SOURCES
	lib/wsfsp/client.c
	lib/wsfsp/client_protocol.c
	lib/wsfsp/provider.c
	lib/wsfsp/provider_default.c
)

add_library(wsfs-provider SHARED ${WSFS_PROVIDER_SOURCES})

set_target_properties(wsfs-provider PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(wsfs-provider PROPERTIES SOVERSION 0)
set_target_properties(wsfs-provider PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(wsfs-provider PROPERTIES COMPILE_DEFINITIONS "WSFSP_API=WSFSP_EXPORT")

target_include_directories(wsfs-provider PUBLIC lib ${EXTRA_INCLUDE_DIRS})
target_compile_options(wsfs-provider PUBLIC  ${EXTRA_CFLAGS})

file(WRITE "${PROJECT_BINARY_DIR}/libwsfs-provider.pc"
"prefix=\"${CMAKE_INSTALL_PREFIX}\"
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib${LIB_SUFFIX}
includedir=\${prefix}/include
Name: libwsfs-provider
Description: Provider library for websockets filesystem
Version: ${PROJECT_VERSION}

Libs: -L\${libdir} -lwsfs-provider  -l${LWS_LIBRARIES} -l${JANSSON_LIBRARIES}
Cflags: -I\${includedir}"
)

install(TARGETS wsfs-provider DESTINATION lib${LIB_SUFFIX})
install(FILES include/wsfs_provider.h DESTINATION include)
install(DIRECTORY include/wsfsp DESTINATION include)
install(FILES "${PROJECT_BINARY_DIR}/libwsfs-provider.pc" DESTINATION lib${LIB_SUFFIX}/pkgconfig)


# examples

if(NOT WITHOUT_EXAMPLE)

# daemon

add_executable(wsfsd
	example/daemon/main.c
)

target_link_libraries(wsfsd PUBLIC wsfs ${EXTRA_LIBS})
target_include_directories(wsfsd PUBLIC ${EXTRA_INCLUDE_DIRS})
target_compile_options(wsfsd PUBLIC ${EXTRA_CFLAGS})

# provider

add_executable(wsfs-provider-app
	example/provider/main.c
)

set_target_properties(wsfs-provider-app PROPERTIES OUTPUT_NAME wsfs-provider)

target_link_libraries(wsfs-provider-app PUBLIC wsfs-provider ${EXTRA_LIBS})
target_include_directories(wsfs-provider-app PUBLIC ${EXTRA_INCLUDE_DIRS})
target_compile_options(wsfs-provider-app PUBLIC ${EXTRA_CFLAGS})

endif(NOT WITHOUT_EXAMPLE)

# tests

if(NOT WITHOUT_TESTS)

pkg_check_modules(GTEST gtest_main)

add_library(wsfs-static STATIC ${WSFS_SOURCES})

set_target_properties(wsfs-static PROPERTIES OUTPUT_NAME wsfs)

target_include_directories(wsfs-static PUBLIC lib ${EXTRA_INCLUDE_DIRS})
target_compile_options(wsfs-static PUBLIC ${EXTRA_CFLAGS})


add_executable(alltests
	test/msleep.cc
	test/test_response_parser.cc
	test/test_server.cc
	test/test_timepoint.cc
	test/test_timer.cc
)

target_link_libraries(alltests PUBLIC wsfs-static ${EXTRA_LIBS} ${GTEST_LIBRARIES})
target_include_directories(alltests PUBLIC lib ${EXTRA_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})
target_compile_options(alltests PUBLIC ${EXTRA_CFLAGS} ${GTEST_CFLAGS})

enable_testing()
add_test(alltests alltests)

endif(NOT WITHOUT_TESTS)