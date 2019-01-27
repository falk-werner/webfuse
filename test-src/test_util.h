#ifndef _WSFS_TEST_UTIL_H
#define _WSFS_TEST_UTIL_H

#define ASSERT_EQ(expected, actual) \
	do \
	{ \
		if ((expected) != (actual)) \
		{ \
			fail(__FILE__, __LINE__, "expected " #expected ", but was " #actual); \
		} \
	} \
	while (0)

#define ASSERT_NE(expected, actual) \
	do \
	{ \
		if ((expected) == (actual)) \
		{ \
			fail(__FILE__, __LINE__, "expected " #expected ", but was " #actual); \
		} \
	} \
	while (0)

extern void fail(
	char const * file_name,
	int line,
	char const * message
);

#endif

