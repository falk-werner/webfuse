cmake_minimum_required(VERSION 3.10)
project(webfuse VERSION 2.0.0)

set (CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED IMPORTED_TARGET fuse3)
pkg_check_modules(LWS REQUIRED IMPORTED_TARGET libwebsockets)

add_library(webfuse_static STATIC
    src/webfuse/webfuse.cpp
    src/webfuse/fuse.cpp
    src/webfuse/filesystem.cpp
    src/webfuse/filesystem/status.cpp
    src/webfuse/filesystem/accessmode.cpp
    src/webfuse/filesystem/openflags.cpp
    src/webfuse/filesystem/userid.cpp
    src/webfuse/filesystem/groupid.cpp
    src/webfuse/filesystem/filemode.cpp
    src/webfuse/filesystem/filetime.cpp
    src/webfuse/filesystem/fileattributes.cpp
    src/webfuse/filesystem/filesystem_statistics.cpp
    src/webfuse/filesystem/empty_filesystem.cpp
    src/webfuse/ws/config.cpp
    src/webfuse/ws/server.cpp
)

target_include_directories(webfuse_static PUBLIC src)
target_link_libraries(webfuse_static PUBLIC PkgConfig::FUSE PkgConfig::LWS)

add_executable(webfuse
    src/main.cpp)

target_link_libraries(webfuse PRIVATE webfuse_static)

if(NOT(WITHOUT_TEST))

    pkg_check_modules(GTEST REQUIRED gtest_main)
    pkg_check_modules(GMOCK REQUIRED gmock)

    add_executable(alltests
        test-src/webfuse/test_app.cpp
        test-src/webfuse/filesystem/test_status.cpp
        test-src/webfuse/filesystem/test_accessmode.cpp
        test-src/webfuse/filesystem/test_openflags.cpp
        test-src/webfuse/filesystem/test_filemode.cpp
        test-src/webfuse/filesystem/test_userid.cpp
        test-src/webfuse/filesystem/test_groupid.cpp
        test-src/webfuse/filesystem/test_fileattributes.cpp
    )

    target_include_directories(alltests PRIVATE ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
    target_compile_options(alltests PRIVATE 
        ${GTEST_CFLAGS} ${GTEST_CFLAGS_OTHER} 
        ${GMOCK_CFLAGS} ${GMOCK_CFLAGS_OTHER}
    )
    target_link_libraries(alltests PRIVATE webfuse_static ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})

    enable_testing()
    add_test(NAME alltests COMMAND alltests)

    find_program(VALGRIND valgrind REQUIRED)
    if(VALGRIND)
        add_custom_target(memcheck COMMAND valgrind --leak-check=full --error-exitcode=1 ./alltests)
    endif()

endif()