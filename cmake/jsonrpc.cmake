# jsonrpc

add_library(jsonrpc STATIC
	lib/jsonrpc/api.c
	lib/jsonrpc/impl/proxy.c
	lib/jsonrpc/impl/server.c
	lib/jsonrpc/impl/method.c
	lib/jsonrpc/impl/request.c
	lib/jsonrpc/impl/response.c
	lib/jsonrpc/impl/error.c
)

target_include_directories(jsonrpc PUBLIC lib)
set_target_properties(jsonrpc PROPERTIES C_VISIBILITY_PRESET hidden)
