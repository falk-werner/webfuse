#ifndef _WSFS_RESPONSE_PARSER_H
#define _WFSF_RESPONSE_PARSER_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#include <jansson.h>
#include "wsfs/status.h"

struct wsfs_response
{
	wsfs_status status;
	int id;
	json_t * result;	
};

#ifdef __cplusplus
extern "C" {
#endif

extern void wsfs_response_parse(
	char const * buffer,
	size_t buffer_length,
	struct wsfs_response * response);

#ifdef __cplusplus
}
#endif

#endif

