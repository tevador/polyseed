/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef POLYSEED_H
#define POLYSEED_H

#include <stdint.h>
#include <time.h>

/* Number of words in the mnemonic phrase */
#define POLYSEED_NUM_WORDS 16

/* The size of the serialized seed */
#define POLYSEED_SIZE 32

/* The serialized seed. The contents are platform-independent. */
typedef uint8_t polyseed_storage[POLYSEED_SIZE];

/* The maximum possible length of a mnemonic phrase */
#define POLYSEED_STR_SIZE 360

/* Mnemonic phrase buffer */
typedef char polyseed_str[POLYSEED_STR_SIZE];

/* Dependency injection definitions */
typedef void polyseed_randbytes(void* result, size_t n);
typedef void polyseed_pbkdf2(const uint8_t* pw, size_t pwlen,
    const uint8_t* salt, size_t saltlen, uint64_t iterations,
    uint8_t* key, size_t keylen);
typedef void polyseed_transform(const char* str, polyseed_str norm);
typedef time_t polyseed_time(time_t* t);
typedef void polyseed_memzero(void* const ptr, const size_t len);
typedef void* polyseed_malloc(size_t n);
typedef void polyseed_mfree(void* ptr);

typedef struct polyseed_dependency {
    /* Function to generate cryptographically secure random bytes */
    polyseed_randbytes* randbytes;
    /* Function to calculate PBKDF2 based on HMAC-SHA256 */
    polyseed_pbkdf2* pbkdf2_sha256;
    /* Function to convert a UTF8 string to the composed canonical form. */
    polyseed_transform* u8_nfc;
    /* Function to convert a UTF8 string to the decomposed canonical form. */
    polyseed_transform* u8_nfkd;
    /* Function to securely erase memory */
    polyseed_memzero* memzero;
    /* OPTIONAL: Function to get the current time */
    polyseed_time* time;
    /* OPTIONAL: Function to allocate memory */
    polyseed_malloc* alloc;
    /* OPTIONAL: Function to free memory */
    polyseed_mfree* free;
} polyseed_dependency;

/* List of coins. The seeds for different coins are incompatible. */
typedef enum polyseed_coin {
    POLYSEED_MONERO = 0,
    POLYSEED_AEON = 1,
    /* Other coins should be added here sequentially. */
    /* The maximum supported value is 2047. */
} polyseed_coin;

typedef enum polyseed_status {
    /* Success */
    POLYSEED_OK = 0,
    /* Wrong number of words in the phrase */
    POLYSEED_ERR_NUM_WORDS = 1,
    /* Unknown language or unsupported words */
    POLYSEED_ERR_LANG = 2,
    /* Checksum mistmach */
    POLYSEED_ERR_CHECKSUM = 3,
    /* Unsupported seed features */
    POLYSEED_ERR_UNSUPPORTED = 4,
    /* Invalid seed format */
    POLYSEED_ERR_FORMAT = 5,
    /* Memory allocation failure */
    POLYSEED_ERR_MEMORY = 6,
} polyseed_status;

/* Opaque struct with the seed data */
typedef struct polyseed_data polyseed_data;

/* Opaque struct with language data */
typedef struct polyseed_lang polyseed_lang;

/*
Shared/static library definitions 
    - define POLYSEED_SHARED when building a shared library
    - define POLYSEED_STATIC when building a static library
    - define POLYSEED_STATIC when linking to the static library
*/
#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef POLYSEED_SHARED
        #define POLYSEED_API __declspec(dllexport)
    #elif !defined(POLYSEED_STATIC)
        #define POLYSEED_API __declspec(dllimport)
    #else
        #define POLYSEED_API
    #endif
    #define POLYSEED_PRIVATE
#else
    #ifdef POLYSEED_SHARED
        #define POLYSEED_API __attribute__ ((visibility ("default")))
    #else
        #define POLYSEED_API __attribute__ ((visibility ("hidden")))
    #endif
    #define POLYSEED_PRIVATE __attribute__ ((visibility ("hidden")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Injects the dependencies of polyseed. Must be called before using the other
 * API functions.
 *
 * @param deps is a pointer to the structure with dependencies. May point to
 *        a temporary variable (the struct gets copied internally).
 *        Must not be NULL.
*/
POLYSEED_API
void polyseed_inject(const polyseed_dependency* deps);

/**
 * @return the number of supported languages.
 */
POLYSEED_API
int polyseed_get_num_langs(void);

/**
 * Returns a language by its index.
 *
 * @param i is the language index. Valid values are from zero to one less than
 *        the number of supported languages.
 *        WARNING: Callers should not rely on languages having a specific index.
 *        The internal order of languages may change in future versions.
 *
 * @return opaque pointer to the language structure.
 */
POLYSEED_API
const polyseed_lang* polyseed_get_lang(int i);

/**
 * Returns the native name of a language.
 *
 * @param lang is the pointer to a language structure. Must not be NULL.
 *
 * @return the native name of the language in UTF8.
 */
POLYSEED_API
const char* polyseed_get_lang_name(const polyseed_lang* lang);

/**
 * Returns the English name of a language.
 *
 * @param lang is the pointer to a language structure. Must not be NULL.
 *
 * @return the English name of the language.
 */
POLYSEED_API
const char* polyseed_get_lang_name_en(const polyseed_lang* lang);

/**
 * Creates a new seed.
 *
 * @return an opaque pointer to the seed data or NULL if memory allocation
 *         failed.
*/
POLYSEED_API
polyseed_data* polyseed_create(void);

/**
 * Securely erases the seed data and releases the allocated memory.
 * This function should be called for pointers obtained from the following
 * functions:
 *  - polyseed_create
 *  - polyseed_decode
 *  - polyseed_load
 *
 * @param seed is the pointer to be freed. If NULL, no action is performed.
*/
POLYSEED_API
void polyseed_free(polyseed_data* seed);

/**
 * Gets the approximate date when the seed was created.
 *
 * @param seed is the pointer to the seed data. Must not be NULL.
 *
 * @return time_t structure with the approximate date when the seed was created.
 */
POLYSEED_API
time_t polyseed_get_birthday(const polyseed_data* seed);

/**
 * Derives a secret key from the mnemonic seed.
 *
 * @param seed is a pointer to the seed data. Must not be NULL.
 * @param coin is the coin the secret key is intended for.
 * $param key_size is the required key size.
 * @param key_out is the buffer where the secret key will be stored.
 *        Must not be NULL.
*/
POLYSEED_API
void polyseed_keygen(const polyseed_data* seed, polyseed_coin coin,
    size_t key_size, uint8_t* key_out);

/**
 * Encodes the mnemonic seed into a string.
 *
 * @param seed is a pointer to the seed data. Must not be NULL.
 * @param lang is a pointer to the language to encode the seed.
 *        Must not be NULL.
 * @param coin is the coin the mnemonic phrase is intended for.
 * @param str_out is the buffer where the mnemonic phrase will be stored
 *        as a C-style string. Must not be NULL.
*/
POLYSEED_API
void polyseed_encode(const polyseed_data* seed, const polyseed_lang* lang,
    polyseed_coin coin, polyseed_str str_out);

/**
 * Decodes the seed from a mnemonic phrase.
 *
 * @param str is the mnemonic phrase as a C-style string. Must not be NULL.
 * @param coin is the coin the mnemonic phrase is intended for.
 * @param lang_out is an optional pointer. IF not NULL, the detected language
 *        of the mnemonic phrase will be stored there.
 * @param seed_out is a pointer where the seed pointer will be stored.
 *        Must not be NULL.
 *
 * @return POLYSEED_OK if the operation was successful. Other values indicate
 *         an error (in that case, *lang_out and *seed_out are undefined).
 */
POLYSEED_API
polyseed_status polyseed_decode(const char* str, polyseed_coin coin,
    const polyseed_lang** lang_out, polyseed_data** seed_out);

/**
 * Serializes the seed data in a platform-independent way.
 *
 * @param seed is the pointer to the seed data. Must not be NULL.
 * @param storage is the buffer where the seed will be stored.
 *        Must not be NULL.
*/
POLYSEED_API
void polyseed_store(const polyseed_data* seed, polyseed_storage storage);

/**
 * Loads a serialized seed.
 *
 * @param storage is the buffer with the serialized seed.
 *        Must not be NULL.
 * @param seed_out is a pointer where the seed pointer will be stored.
 *        Must not be NULL.
 *
 * @return POLYSEED_OK if the operation was successful. Other values indicate
 *         an error (in that case, *seed_out is undefined).
 */
POLYSEED_API
polyseed_status polyseed_load(const polyseed_storage storage,
    polyseed_data** seed_out);

#ifdef __cplusplus
}
#endif

#endif
