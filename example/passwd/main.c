#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/opensslv.h>
#include <openssl/evp.h>

#define HASH_ALGORITHM "sha512"

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

static char * get_password_hash(
    char const * password,
    char const * salt,
    char * pepper)
{
    EVP_MD const * digest = EVP_get_digestbyname(HASH_ALGORITHM);
    if (NULL == digest)
    {
        fprintf(stderr, "error: hash algorithm %s not supported\n", HASH_ALGORITHM);
        exit(EXIT_FAILURE);
    }

    char * result = NULL;
    unsigned int hash_size = digest->md_size;
    unsigned char * hash = malloc(hash_size);

    if (NULL != hash)
    {
        EVP_MD_CTX context;        
        EVP_MD_CTX_init(&context);
        EVP_DigestInit_ex(&context, digest, NULL);        
        EVP_DigestUpdate(&context, password, strlen(password));
        EVP_DigestUpdate(&context, salt, strlen(salt));
        EVP_DigestUpdate(&context, pepper, strlen(pepper));
        EVP_DigestFinal_ex(&context, hash, &hash_size);
        EVP_MD_CTX_cleanup(&context);

        result = to_hex(hash, hash_size);
        free(hash);
    }

    return result;
}

int main(int argc, char * argv[])
{
    OPENSSL_init();
    OPENSSL_add_all_algorithms_conf();

    char * test = get_password_hash("secret", "123\n", "");
    puts(test);
    free(test);

    return EXIT_SUCCESS;
}