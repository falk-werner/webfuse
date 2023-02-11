cmake_minimum_required(VERSION 3.10)
project(webfuse VERSION 2.0.0)

option(WITHOUT_CLANG_TIDY "Disables clang tidy" OFF)

set (CMAKE_CXX_STANDARD 17)

find_library(PAM pam REQUIRED)
find_library(B64 b64 REQUIRED)

add_executable(webfuse_pam_authenticator src/main.cpp)
target_link_libraries(webfuse_pam_authenticator pam b64)
install(TARGETS webfuse_pam_authenticator DESTINATION bin)
install(FILES etc/pam.d/webfuse DESTINATION /etc/pam.d)

add_executable(webfuse_pam_token_encode src/token_encode.cpp)
target_link_libraries(webfuse_pam_token_encode b64)

add_executable(webfuse_pam_token_decode src/token_decode.cpp)
target_link_libraries(webfuse_pam_token_decode b64)

if(NOT(WITHOUT_CLANG_TIDY))
set_property(
    TARGET webfuse_pam_authenticator webfuse_pam_token_encode webfuse_pam_token_decode
    PROPERTY CXX_CLANG_TIDY clang-tidy -checks=readability-*,-readability-identifier-length -warnings-as-errors=*)
endif()
