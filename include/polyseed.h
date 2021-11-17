/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef POLYSEED_H
#define POLYSEED_H

#include <stdint.h>
#include <time.h>

/* Number of words in the mnemonic phrase */
#define POLYSEED_NUM_WORDS 16

/* The size of the secret seed buffer */
#define POLYSEED_SECRET_SIZE 32

/* The maximum possible length of a decomposed phrase */
#define POLYSEED_STR_SIZE 360

typedef char polyseed_str[POLYSEED_STR_SIZE];

/* Dependency injection definitions */
typedef void polyseed_randbytes(void* result, size_t n);

typedef void polyseed_pbkdf2(const uint8_t* pw, size_t pwlen,
    const uint8_t* salt, size_t saltlen, uint64_t iterations,
    uint8_t* key, size_t keylen);

typedef void polyseed_transform(const char* str, polyseed_str norm);

typedef struct polyseed_dependency {
    /* Function to generate cryptographically secure random bytes */
    polyseed_randbytes* rand;
    /* Function to calculate PBKDF2 based on HMAC-SHA256 */
    polyseed_pbkdf2* pbkdf2_sha256;
    /* Function to convert a UTF8 string to a composed canonical form. */
    polyseed_transform* u8_nfc;
    /* Function to convert a UTF8 string to a decomposed canonical form. */
    polyseed_transform* u8_nfkd;
} polyseed_dependency;

/* Seed data structure for serialization */
typedef struct polyseed_data {
    unsigned birthday;
    unsigned reserved;
    /* padded with zeroes for future compatibility with longer seeds */
    uint8_t secret[POLYSEED_SECRET_SIZE];
} polyseed_data;

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
    /* Reserved bits are not zero */
    POLYSEED_ERR_RESERVED = 4,
} polyseed_status;

/* Opaque struct with language data */
typedef struct polyseed_lang polyseed_lang;

/*
Shared/static library definitions 
    - define POLYSEED_SHARED when building a shared library
    - define POLYSEED_STATIC when building a static library
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
 * Inject the dependencies of polyseed. Must be called before using the other
 * API functions.
 *
 * @param deps is a pointer to the structure with dependencies.
 *        Must not be NULL.
*/
POLYSEED_API void polyseed_inject(const polyseed_dependency* deps);

/**
 * Get the number of supported languages.
 *
 * @return the number of supported languages.
 */
POLYSEED_API int polyseed_get_num_langs(void);

/**
 * Get a language by its index.
 *
 * @param i is the language index. Valid values are from zero to one less than
 *        the number of supported languages.
 *        WARNING: Callers should not rely on languages having a specific index.
 *        The internal order of languages may change in future versions.
 *
 * @return opaque pointer to the language structure.
 */
POLYSEED_API const polyseed_lang* polyseed_get_lang(int i);

/**
 * Get the native name of a language.
 *
 * @param lang is the pointer to a language structure. Must not be NULL.
 *
 * @return the native name of the language in UTF8.
 */
POLYSEED_API const char* polyseed_get_lang_name(const polyseed_lang* lang);

/**
 * Get the English name of a language.
 *
 * @param lang is the pointer to a language structure. Must not be NULL.
 *
 * @return the English name of the language.
 */
POLYSEED_API const char* polyseed_get_lang_name_en(const polyseed_lang* lang);

/**
 * Create a seed.
 *
 * @param data_out is a pointer to the structure where the seed data will be stored.
 *        Must not be NULL.
*/
POLYSEED_API void polyseed_create(polyseed_data* data_out);

/**
 * Get the approximate time when the seed was created.
 *
 * @param lang is the pointer to the seed data. Must not be NULL.
 *
 * @return time_t structure with the approximate time when the seed was created.
 */
POLYSEED_API time_t polyseed_get_birthday(const polyseed_data* data);

/**
 * Derives a secret key from the mnemonic seed.
 *
 * @param data is a pointer to the seed data. Must not be NULL.
 * @param coin is the coin the secret key is intended for.
 * $param key_size is the required key size.
 * @param key_out is the buffer where the secret key will be stored.
 *        Must not be NULL.
*/
POLYSEED_API void polyseed_keygen(const polyseed_data* data,
    polyseed_coin coin, size_t key_size, uint8_t* key_out);

/**
 * Encodes the mnemonic seed into a string.
 *
 * @param data is a pointer to the seed data. Must not be NULL.
 * @param lang is a pointer to the language to encode the seed.
 *        Must not be NULL.
 * @param coin is the coin the mnemonic phrase is intended for.
 * @param str_out is the buffer where the mnemonic phrase will be stored
 *        as a C-style string. Must not be NULL.
*/
POLYSEED_API void polyseed_encode(const polyseed_data* data,
    const polyseed_lang* lang, polyseed_coin coin, polyseed_str str_out);

/**
 * Decode the seed from a mnemonic phrase.
 *
 * @param str is the mnemonic phrase as a C-style string. Must not be NULL.
 * @param coin is the coin the mnemonic phrase is intended for.
 * @param lang_out is an optional pointer. IF not NULL, the detected language
 *        of the mnemonic phrase will be stored there.
 * @param data_out is a pointer to the structure where the seed data will be stored.
 *        Must not be NULL.
 *
 * @return POLYSEED_OK if the operation was successful. Other values indicate
 *         an error (in that case, *lang_out and *data_out are undefined).
 */
POLYSEED_API polyseed_status polyseed_decode(const char* str,
    polyseed_coin coin, const polyseed_lang** lang_out,
    polyseed_data* data_out);

#ifdef __cplusplus
}
#endif

#endif
