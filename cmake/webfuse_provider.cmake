if(NOT WITHOUT_PROVIDER)

add_library(webfuse-provider-static STATIC 
	lib/webfuse/provider/api.c
	lib/webfuse/provider/impl/url.c
	lib/webfuse/provider/impl/client.c
	lib/webfuse/provider/impl/client_config.c
	lib/webfuse/provider/impl/client_protocol.c
	lib/webfuse/provider/impl/provider.c
	lib/webfuse/provider/impl/request.c
	lib/webfuse/provider/impl/dirbuffer.c
	lib/webfuse/provider/impl/credentials.c
	lib/webfuse/provider/impl/operation/lookup.c
	lib/webfuse/provider/impl/operation/getattr.c
	lib/webfuse/provider/impl/operation/readdir.c
	lib/webfuse/provider/impl/operation/open.c
	lib/webfuse/provider/impl/operation/close.c
	lib/webfuse/provider/impl/operation/read.c
)

set_target_properties(webfuse-provider-static PROPERTIES OUTPUT_NAME webfuse-provider)
set_target_properties(webfuse-provider-static PROPERTIES C_VISIBILITY_PRESET hidden)
target_include_directories(webfuse-provider-static PRIVATE 
	lib
	lib/wf/timer/include
	lib/wf/jsonrpc/include
)

add_library(webfuse-provider SHARED 
	lib/webfuse/provider/api.c
)

set_target_properties(webfuse-provider PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(webfuse-provider PROPERTIES SOVERSION 0)
set_target_properties(webfuse-provider PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(webfuse-provider PROPERTIES COMPILE_DEFINITIONS "WFP_API=WFP_EXPORT")

target_include_directories(webfuse-provider PUBLIC lib)
target_link_libraries(webfuse-provider PRIVATE webfuse-provider-static webfuse-core wf_jsonrpc wf_timer)

file(WRITE "${PROJECT_BINARY_DIR}/libwebfuse-provider.pc"
"prefix=\"${CMAKE_INSTALL_PREFIX}\"
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib${LIB_SUFFIX}
includedir=\${prefix}/include
Name: libwebfuse-provider
Description: Provider library for websockets filesystem
Version: ${PROJECT_VERSION}

Libs: -L\${libdir} -lwebfuse-provider  -l${LWS_LIBRARIES} -l${JANSSON_LIBRARIES}
Cflags: -I\${includedir}"
)

install(TARGETS webfuse-provider DESTINATION lib${LIB_SUFFIX} COMPONENT libraries)
install(FILES "${PROJECT_BINARY_DIR}/libwebfuse-provider.pc" DESTINATION lib${LIB_SUFFIX}/pkgconfig COMPONENT libraries)

install(FILES include/webfuse_provider.h DESTINATION include COMPONENT headers)
install(DIRECTORY include/webfuse/provider DESTINATION include/webfuse COMPONENT headers)

endif(NOT WITHOUT_PROVIDER)
