#ifndef USERDB_H
#define USERDB_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct userdb;

extern struct userdb * userdb_create(
    char const * pepper);

extern void userdb_dispose(struct userdb * db);

extern bool userdb_save(
    struct userdb * db,
    char const * filename);

extern bool userdb_load(
    struct userdb * db,
    char const * filename);

extern void userdb_add(
    struct userdb * db,
    char const * username,
    char const * password);

extern void userdb_remove(
    struct userdb * db,
    char const * user);

extern bool userdb_check(
    struct userdb * db,
    char const * username,
    char const * password);

#ifdef __cplusplus
}
#endif

#endif