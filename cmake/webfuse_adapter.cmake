if(NOT WITHOUT_ADAPTER)

pkg_check_modules(FUSE3 REQUIRED fuse3)

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
	lib/webfuse/adapter/impl/mountpoint.c
	lib/webfuse/adapter/impl/mountpoint_factory.c
	lib/webfuse/adapter/impl/operation/lookup.c
	lib/webfuse/adapter/impl/operation/getattr.c
	lib/webfuse/adapter/impl/operation/readdir.c
	lib/webfuse/adapter/impl/operation/open.c
	lib/webfuse/adapter/impl/operation/close.c
	lib/webfuse/adapter/impl/operation/read.c
)

target_include_directories(webfuse-adapter-static PRIVATE
	lib
	${FUSE3_INCLUDE_DIRS} 
)

target_compile_options(webfuse-adapter-static PUBLIC
	${FUSE3_CFLAGS_OTHER}
)

set_target_properties(webfuse-adapter-static PROPERTIES OUTPUT_NAME webfuse-adapter)
set_target_properties(webfuse-adapter-static PROPERTIES C_VISIBILITY_PRESET hidden)

add_library(webfuse-adapter SHARED 
	lib/webfuse/adapter/api.c
)

target_include_directories(webfuse-adapter PRIVATE
	${FUSE3_INCLUDE_DIRS} 
)

target_compile_options(webfuse-adapter PUBLIC
	${FUSE3_CFLAGS_OTHER}
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

install(TARGETS webfuse-adapter DESTINATION lib${LIB_SUFFIX} COMPONENT libraries)
install(FILES "${PROJECT_BINARY_DIR}/libwebfuse-adapter.pc" DESTINATION lib${LIB_SUFFIX}/pkgconfig COMPONENT libraries)

install(FILES include/webfuse_adapter.h DESTINATION include COMPONENT headers)
install(DIRECTORY include/webfuse/adapter DESTINATION include/webfuse COMPONENT headers)

endif(NOT WITHOUT_ADAPTER)
