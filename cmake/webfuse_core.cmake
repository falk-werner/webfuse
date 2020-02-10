# libwebfuse-core

add_library(webfuse-core STATIC 
	lib/webfuse/core/slist.c
	lib/webfuse/core/message.c
	lib/webfuse/core/message_queue.c
	lib/webfuse/core/status.c
	lib/webfuse/core/string.c
	lib/webfuse/core/path.c
	lib/webfuse/core/base64.c
	lib/webfuse/core/lws_log.c
)

set_target_properties(webfuse-core PROPERTIES OUTPUT_NAME webfuse-core)
target_include_directories(webfuse-core PUBLIC lib)
set_target_properties(webfuse-core PROPERTIES C_VISIBILITY_PRESET hidden)

install(DIRECTORY include/webfuse/core DESTINATION include/webfuse)
