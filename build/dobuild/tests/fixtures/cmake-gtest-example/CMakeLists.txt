cmake_minimum_required(VERSION 2.8)

project(cmake-gtest-example)

option(POSITION_INDEPENDENT_CODE "This variable is used to initialize the POSITION_INDEPENDENT_CODE property on all the targets" ON)

include(CTest)

add_executable(alltests
  test_stringcompare.cpp
)

target_include_directories(alltests PUBLIC ${GTEST_INCLUDE_DIRS})
target_compile_options(alltests PUBLIC -Wall -Wextra ${GTEST_CFLAGS})
target_link_libraries(alltests PUBLIC -pthread ${GTEST_LIBRARIES} gtest_main gtest)

enable_testing()
add_test(NAME alltests COMMAND alltests)

