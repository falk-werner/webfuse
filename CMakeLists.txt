cmake_minimum_required(VERSION 3.10)
project(webfuse VERSION 2.0.0)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED IMPORTED_TARGET fuse3)
pkg_check_modules(LWS REQUIRED IMPORTED_TARGET libwebsockets)

add_library(webfuse_static STATIC
    src/webfuse/webfuse.cpp)

target_include_directories(webfuse_static PUBLIC src)
target_link_libraries(webfuse_static PUBLIC PkgConfig::FUSE PkgConfig::LWS)

add_executable(webfuse
    src/main.cpp)

target_link_libraries(webfuse PRIVATE webfuse_static)

if(NOT(WITHOUT_TEST))

    pkg_check_modules(GTEST REQUIRED IMPORTED_TARGET gtest_main)

    add_executable(alltests
        test/webfuse/test_app.cpp
    )

    target_link_libraries(alltests PRIVATE webfuse_static PkgConfig::GTEST)

    enable_testing()
    add_test(NAME alltests COMMAND alltests)

endif()
