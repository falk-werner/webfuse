#include "userdb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

#include <openssl/rand.h>
#include <openssl/evp.h>

#define USERDB_HASH_ALGORITHM "sha512"
#define USERDB_MAJOR 1
#define USERDB_MINOR 0

#define USERDB_SALT_SIZE 32

struct userdb
{    
    json_t * users;
    char * pepper;
    char * hash_algorithm;
};

static bool is_compatible(json_t * meta)
{
    bool result = false;
    if (json_is_object(meta))
    {
        json_t * type = json_object_get(meta, "type");
        json_t * major = json_object_get(meta, "major");
        json_t * minor = json_object_get(meta, "minor");
        json_t * hash_algorithm = json_object_get(meta, "hash_algorithm");

        result = (
            json_is_string(type) &&
            (0 == strcmp(json_string_value(type), "wf-userdb")) &&
            json_is_integer(major) && 
            (USERDB_MAJOR == json_integer_value(major)) &&
            json_is_integer(minor)  &&
            json_is_string(hash_algorithm)
        );

        if (result)
        {
            char const * algorithm = json_string_value(hash_algorithm);
            result = (NULL != EVP_get_digestbyname(algorithm));
        }

    }
    return result;
}

static char hex_char(unsigned char value)
{
    switch (value)
    {
        case  0x00: return '0';
        case  0x01: return '1';
        case  0x02: return '2';
        case  0x03: return '3';
        case  0x04: return '4';
        case  0x05: return '5';
        case  0x06: return '6';
        case  0x07: return '7';
        case  0x08: return '8';
        case  0x09: return '9';
        case  0x0a: return 'a';
        case  0x0b: return 'b';
        case  0x0c: return 'c';
        case  0x0d: return 'd';
        case  0x0e: return 'e';
        case  0x0f: return 'f';
        default: return '?';
    }
}

static char * to_hex(unsigned char const * value, size_t length)
{
    char * result = malloc((2 * length) + 1);
    if (NULL != result)
    {
        for (size_t i = 0, j = 0; i < length; i++, j+=2)
        {
            unsigned char high = (value[i] >> 4) & 0x0f;
            unsigned char low =   value[i]       & 0x0f;

            result[j    ] = hex_char(high);
            result[j + 1] = hex_char(low);
        }

        result[2 * length] = '\0';
    }

    return result;
}

static char * generate_salt(void)
{
    unsigned char buffer[USERDB_SALT_SIZE];
    int rc = RAND_bytes(buffer, USERDB_SALT_SIZE);
    if (1 != rc)
    {
        fprintf(stderr, "fatal: failed to generate salt (OpenSSL RAND_bytes failed)\n");
        exit(EXIT_FAILURE);
    }

    return to_hex(buffer, USERDB_SALT_SIZE);
}

static char * compute_hash(
    struct userdb * db, 
    char const * password, 
    char const * salt)
{
    EVP_MD const * digest = EVP_get_digestbyname(db->hash_algorithm);
    if (NULL == digest)
    {
        fprintf(stderr, "error: hash algorithm %s not supported\n", db->hash_algorithm);
        return NULL;
    }

    char * result = NULL;
    unsigned int hash_size = EVP_MD_size(digest);
    unsigned char * hash = malloc(hash_size);

    if (NULL != hash)
    {
        EVP_MD_CTX * context = EVP_MD_CTX_new();        
        EVP_DigestInit_ex(context, digest, NULL);        
        EVP_DigestUpdate(context, password, strlen(password));
        EVP_DigestUpdate(context, salt, strlen(salt));
        EVP_DigestUpdate(context, db->pepper, strlen(db->pepper));
        EVP_DigestFinal_ex(context, hash, &hash_size);
        EVP_MD_CTX_free(context);

        result = to_hex(hash, hash_size);
        free(hash);
    }

    return result;
}

struct userdb * userdb_create(
    char const * pepper)
{
    struct userdb * db = malloc(sizeof(struct userdb));
    if (NULL != db)
    {
        db->users = json_object();
        db->pepper = strdup(pepper);
        db->hash_algorithm = strdup(USERDB_HASH_ALGORITHM);
    }

    return db;
}

void userdb_dispose(
    struct userdb * db)
{
    json_decref(db->users);
    free(db->pepper);
    free(db->hash_algorithm);
    free(db);
}

bool userdb_save(
    struct userdb * db,
    char const * filename)
{
    json_t * container = json_object();

    json_t * meta = json_object();
    json_object_set_new(meta, "type", json_string("wf-userdb"));
    json_object_set_new(meta, "major", json_integer(USERDB_MAJOR));
    json_object_set_new(meta, "minor", json_integer(USERDB_MINOR));
    json_object_set_new(meta, "hash_algorithm", json_string(db->hash_algorithm));
    json_object_set_new(container, "meta", meta);

    json_object_set(container, "users", db->users);

    int result = json_dump_file(container, filename, JSON_INDENT(2));
    json_decref(container);

    return (0 == result);
}


bool userdb_load(
    struct userdb * db,
    char const * filename)
{
    bool result = false;
    json_t * container = json_load_file(filename, 0, NULL);
    if (NULL != container)
    {
        json_t * meta = json_object_get(container, "meta");
        json_t * users = json_object_get(container, "users");

        if ((is_compatible(meta)) && (json_is_object(users))) {
            json_t * hash_algorithm = json_object_get(meta, "hash_algorithm");
            free(db->hash_algorithm);
            db->hash_algorithm = strdup(json_string_value(hash_algorithm));

            json_decref(db->users);
            json_incref(users);
            db->users = users;

            result = true;
        }

        json_decref(container);
    }

    return result;
}

void userdb_add(
    struct userdb * db,
    char const * username,
    char const * password)
{
    char * salt = generate_salt();
    char * hash = compute_hash(db, password, salt);

    json_t * user = json_object();
    json_object_set_new(user, "password_hash", json_string(hash));
    json_object_set_new(user, "salt", json_string(salt));

    json_object_set_new(db->users, username, user);

    free(salt);
    free(hash);
}

void userdb_remove(
    struct userdb * db,
    char const * user)
{
    json_object_del(db->users, user);
}

static char const * json_object_get_string(
    json_t * object,
    char const * key)
{
    char const * result = NULL;

    json_t * string_holder = json_object_get(object, key);
    if (json_is_string(string_holder))
    {
        result = json_string_value(string_holder);
    }

    return result;
}

bool userdb_check(
    struct userdb * db,
    char const * username,
    char const * password)
{
    bool result = false;

    json_t * user = json_object_get(db->users, username);
    if (json_is_object(user))
    {
        char const * salt = json_object_get_string(user, "salt");
        char const * hash = json_object_get_string(user, "password_hash");

        char * computed_hash = compute_hash(db, password, salt);

        result = (0 == strcmp(computed_hash, hash));
        free(computed_hash);
    }

    return result;
}