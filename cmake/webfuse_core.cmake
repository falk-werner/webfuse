# libwebfuse-core

add_library(webfuse-core STATIC 
	lib/webfuse/core/slist.c
	lib/webfuse/core/message.c
	lib/webfuse/core/message_queue.c
	lib/webfuse/core/status.c
	lib/webfuse/core/string.c
	lib/webfuse/core/base64.c
	lib/webfuse/core/lws_log.c
	lib/webfuse/core/json_util.c
    lib/webfuse/core/timer/manager.c
    lib/webfuse/core/timer/timepoint.c
    lib/webfuse/core/timer/timer.c
	lib/webfuse/core/jsonrpc/proxy.c
	lib/webfuse/core/jsonrpc/proxy_variadic.c
	lib/webfuse/core/jsonrpc/server.c
	lib/webfuse/core/jsonrpc/method.c
	lib/webfuse/core/jsonrpc/request.c
	lib/webfuse/core/jsonrpc/response.c
	lib/webfuse/core/jsonrpc/error.c

)

set_target_properties(webfuse-core PROPERTIES OUTPUT_NAME webfuse-core)
target_include_directories(webfuse-core PUBLIC lib lib/wf/jsonrpc/include)
set_target_properties(webfuse-core PROPERTIES C_VISIBILITY_PRESET hidden)

install(DIRECTORY include/webfuse/core DESTINATION include/webfuse COMPONENT headers)
