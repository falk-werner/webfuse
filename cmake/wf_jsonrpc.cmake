# wf_jsonrpc

add_library(wf_jsonrpc STATIC
	lib/wf/jsonrpc/src/wf/jsonrpc/api.c
	lib/wf/jsonrpc/src/wf/jsonrpc/impl/proxy.c
	lib/wf/jsonrpc/src/wf/jsonrpc/impl/server.c
	lib/wf/jsonrpc/src/wf/jsonrpc/impl/method.c
	lib/wf/jsonrpc/src/wf/jsonrpc/impl/request.c
	lib/wf/jsonrpc/src/wf/jsonrpc/impl/response.c
	lib/wf/jsonrpc/src/wf/jsonrpc/impl/error.c
)

target_link_libraries(wf_jsonrpc PUBLIC wf_timer)

target_include_directories(wf_jsonrpc PRIVATE 
	lib/wf/timer/include
	lib/wf/jsonrpc/src
)

target_include_directories(wf_jsonrpc PUBLIC 
	lib/wf/jsonrpc/include
)

set_target_properties(wf_jsonrpc PROPERTIES C_VISIBILITY_PRESET hidden)
