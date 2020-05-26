cmake_minimum_required(VERSION 3.10)

project(cmake-gtest-example VERSION 0.0.1 DESCRIPTION "CMake gtest example")

option(THREADS_PREFER_PTHREAD_FLAG "If the use of the -pthread compiler and linker flag is prefered then the caller can set" ON)
option(POSITION_INDEPENDENT_CODE "This variable is used to initialize the POSITION_INDEPENDENT_CODE property on all the targets" ON)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_check_modules(GTEST gtest_main gtest)

include(CTest)
include(GoogleTest)

add_executable(alltests
  test_stringcompare.cpp
)

target_include_directories(alltests PUBLIC ${GTEST_INCLUDE_DIRS})
target_compile_options(alltests PUBLIC -Wall -Wextra ${GTEST_CFLAGS})
target_link_libraries(alltests PUBLIC Threads::Threads ${GTEST_LIBRARIES} gtest_main)

enable_testing()
gtest_discover_tests(alltests TEST_PREFIX alltests:)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
add_dependencies(check alltests)

add_custom_target(memcheck COMMAND ${CMAKE_CTEST_COMMAND} -T memcheck)
add_dependencies(memcheck alltests)
