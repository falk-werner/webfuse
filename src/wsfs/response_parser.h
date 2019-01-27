#ifndef _WSFS_RESPONSE_PARSER_H
#define _WFSF_RESPONSE_PARSER_H

#include <stddef.h>
#include <jansson.h>
#include "wsfs/status.h"

struct wsfs_response
{
	wsfs_status status;
	int id;
	json_t * result;	
};

extern void wsfs_response_parse(
	char const * buffer,
	size_t buffer_length,
	struct wsfs_response * response);

#endif

