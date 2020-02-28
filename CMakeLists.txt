cmake_minimum_required (VERSION 3.10)
project(webfuse VERSION 0.2.0 DESCRIPTION "Websocket filesystem based on libfuse")

option(WITHOUT_TESTS    "disable unit tests"   OFF)
option(WITHOUT_ADAPTER  "disable adapter library" OFF)
option(WITHOUT_PROVIDER "disable provider library" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(coverage)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LWS REQUIRED libwebsockets)
pkg_check_modules(JANSSON REQUIRED jansson)

add_definitions(-D_FILE_OFFSET_BITS=64)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(C_WARNINGS -Wall -Wextra)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
	"include"
	${LWS_INCLUDE_DIRS} 
	${JANSSON_INCLUDE_DIRS}
)

add_compile_options(
	${C_WARNINGS}
	${LWS_CFLAGS_OTHER}
	${JANSSON_CFLAGS_OTHER}
	"-pthread"
)

include(jsonrpc)
include(webfuse_core)
include(webfuse_adapter)
include(webfuse_provider)
include(unit_tests)

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_DESCRIPTION "Websocket filesystem based on libfuse")
#set(CPACK_COMPONENTS_ALL libraries)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Falk Werner")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_DEBIAN_LIBRARIES_FILE_NAME "webfuse_${PROJECT_VERSION}_${CMAKE_SYSTEM_NAME}.deb")
set(CPACK_DEBIAN_LIBRARIES_PACKAGE_NAME "webfuse")

set(CPACK_DEBIAN_HEADERS_FILE_NAME "webfuse-dev_${PROJECT_VERSION}_${CMAKE_SYSTEM_NAME}.deb")
set(CPACK_DEBIAN_HEADERS_PACKAGE_NAME "webfuse-dev")
set(CPACK_DEBIAN_HEADERS_PACKAGE_DEPENDS "webfuse")

include(CPack)