cmake_minimum_required(VERSION 3.10)
project(webfuse VERSION 2.0.0)

set (CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED IMPORTED_TARGET fuse3)
pkg_check_modules(LWS REQUIRED IMPORTED_TARGET libwebsockets)

add_library(webfuse_static STATIC
    src/webfuse/webfuse.cpp
    src/webfuse/provider.cpp
    src/webfuse/fuse.cpp
    src/webfuse/request_type.cpp
    src/webfuse/response_type.cpp
    src/webfuse/filesystem.cpp
    src/webfuse/filesystem/status.cpp
    src/webfuse/filesystem/accessmode.cpp
    src/webfuse/filesystem/openflags.cpp
    src/webfuse/filesystem/filemode.cpp
    src/webfuse/filesystem/filesystem_statistics.cpp
    src/webfuse/filesystem/empty_filesystem.cpp
    src/webfuse/ws/config.cpp
    src/webfuse/ws/server.cpp
    src/webfuse/ws/client.cpp
    src/webfuse/ws/messagewriter.cpp
    src/webfuse/ws/messagereader.cpp
)

target_include_directories(webfuse_static PUBLIC src)
target_link_libraries(webfuse_static PUBLIC PkgConfig::FUSE PkgConfig::LWS)

add_executable(webfuse
    src/main.cpp)

target_link_libraries(webfuse PRIVATE webfuse_static)

if(NOT(WITHOUT_PROVIDER))

add_executable(webfuse_provider
    src/provider_main.cpp)

target_link_libraries(webfuse_provider PRIVATE webfuse_static)

endif()

if(NOT(WITHOUT_TEST))

    pkg_check_modules(GTEST REQUIRED gtest_main)
    pkg_check_modules(GMOCK REQUIRED gmock)

    add_executable(unit_tests
        test-src/unit/webfuse/test_app.cpp
        test-src/unit/webfuse/test_request_type.cpp
        test-src/unit/webfuse/test_response_type.cpp
        test-src/unit/webfuse/filesystem/test_status.cpp
        test-src/unit/webfuse/filesystem/test_accessmode.cpp
        test-src/unit/webfuse/filesystem/test_openflags.cpp
        test-src/unit/webfuse/filesystem/test_filemode.cpp
    )

    target_include_directories(unit_tests PRIVATE test-src/unit ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
    target_compile_options(unit_tests PRIVATE 
        ${GTEST_CFLAGS} ${GTEST_CFLAGS_OTHER} 
        ${GMOCK_CFLAGS} ${GMOCK_CFLAGS_OTHER}
    )
    target_link_libraries(unit_tests PRIVATE webfuse_static ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})

    add_executable(integration_tests
        test-src/integration/webfuse/test/tempdir.cpp
        test-src/integration/webfuse/test/fixture.cpp
        test-src/integration/webfuse/test/process.cpp
        test-src/integration/webfuse/test/daemon.cpp
        test-src/integration/test_access.cpp
    )

    target_include_directories(integration_tests PRIVATE test-src/integration ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
    target_compile_options(integration_tests PRIVATE 
        ${GTEST_CFLAGS} ${GTEST_CFLAGS_OTHER} 
        ${GMOCK_CFLAGS} ${GMOCK_CFLAGS_OTHER}
    )
    target_link_libraries(integration_tests PRIVATE webfuse_static ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})

    enable_testing()
    add_test(NAME unit_tests COMMAND unit_tests)
    add_test(NAME integration_tests COMMAND integration_tests)

    find_program(VALGRIND valgrind REQUIRED)
    if(VALGRIND)
        add_custom_target(memcheck COMMAND valgrind --leak-check=full --error-exitcode=1 ./unit_tests)
    endif()

endif()