#include <stdio.h>
#include <stdlib.h>

#include "test_util.h"
#include "wsfs/util.h"

extern void test_request_parser();

int main(int WSFS_UNUSED_PARAM(argc), char* WSFS_UNUSED_PARAM(argv[]))
{
	test_request_parser();
	return EXIT_SUCCESS;
}
