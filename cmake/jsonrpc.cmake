# jsonrpc

add_library(jsonrpc STATIC
	lib/jsonrpc/proxy.c
	lib/jsonrpc/server.c
	lib/jsonrpc/method.c
	lib/jsonrpc/request.c
	lib/jsonrpc/response.c
	lib/jsonrpc/error.c
)

target_include_directories(jsonrpc PUBLIC lib)
set_target_properties(jsonrpc PROPERTIES C_VISIBILITY_PRESET hidden)
