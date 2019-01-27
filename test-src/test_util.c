#include "test_util.h"

#include <stdio.h>
#include <stdlib.h>

void fail(
	char const * file_name,
	int line,
	char const * message
)
{
	fprintf(stderr, "error: %s:%d: %s\n", file_name, line, message);
	exit(EXIT_FAILURE);
}

