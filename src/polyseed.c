/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#include "polyseed.h"
#include "dependency.h"
#include "birthday.h"
#include "lang.h"
#include "gf.h"
#include "storage.h"
#include "rs_code.h"

#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define KDF_NUM_ITERATIONS 4096

static void write_str(char** pos, const char* str) {
    char* loc = *pos;
    while (*str != '\0') {
        *loc = *str;
        ++str;
        ++loc;
    }
    *pos = loc;
}

static int str_split(char* str, polyseed_phrase words) {
    char* pos = str;
    char* word = str;
    int w = 0;

    /* split on space */
    while (*pos != '\0') {
        while (*pos != '\0' && *pos != ' ') {
            ++pos;
        }
        words[w] = word;
        if (*pos != '\0') {
            *pos = '\0';
            ++pos;
        }
        word = pos;
        ++w;
        if (w == POLYSEED_NUM_WORDS) {
            if (*pos != '\0') {
                ++w; /* too many words */
            }
            break;
        }
    }
    return w;
}

polyseed_data* polyseed_create(void) {
    CHECK_DEPS();

    /* alocate memory */
    polyseed_data* seed = ALLOC(sizeof(polyseed_data));
    if (seed == NULL) {
        return NULL;
    }

    /* create seed */
    seed->birthday = birthday_encode(GET_TIME());
    seed->reserved = RESERVED_VALUE;
    memset(seed->secret, 0, sizeof(seed->secret));
    GET_RANDOM_BYTES(seed->secret, SECRET_SIZE);
    seed->secret[SECRET_SIZE - 1] &= CLEAR_MASK;

    /* encode polynomial */
    gf_poly poly = { 0 };
    polyseed_data_to_poly(seed, &poly);

    /* calculate checksum */
    polyseed_rs_encode(&poly);
    seed->checksum = poly.coeff[0];

    MEMZERO_LOC(poly);
    return seed;
}

void polyseed_free(polyseed_data* seed) {
    if (seed != NULL) {
        MEMZERO_PTR(seed, polyseed_data);
        FREE(seed);
    }
}

time_t polyseed_get_birthday(const polyseed_data* data) {
    assert(data != NULL);
    return birthday_decode(data->birthday);
}

void polyseed_encode(const polyseed_data* data, const polyseed_lang* lang,
    polyseed_coin coin, polyseed_str str_out) {

    assert(data != NULL);
    assert(lang != NULL);
    assert((gf_elem)coin < GF_SIZE);
    assert(str_out != NULL);
    CHECK_DEPS();

    /* encode polynomial with the existing checksum */
    gf_poly poly = { 0 };
    poly.coeff[0] = data->checksum;
    poly.degree = 1;
    polyseed_data_to_poly(data, &poly);

    /* apply coin */
    poly.coeff[RS_NUM_CHECK_DIGITS] ^= coin;

    polyseed_str str_tmp;
    char* pos = str_tmp;
    int w;

#define WORD(i) lang->words[poly.coeff[i]]

    /* output words */
    for (w = 0; w < POLYSEED_NUM_WORDS - 1; ++w) {
        write_str(&pos, WORD(w));
        write_str(&pos, lang->separator);
    }
    write_str(&pos, WORD(w));
    *pos = '\0';

#undef WORD

    /* compose if needed by the language */
    if (lang->compose) {
        UTF8_COMPOSE(str_tmp, str_out);
    }
    else {
        strncpy(str_out, str_tmp, POLYSEED_STR_SIZE);
    }

    MEMZERO_LOC(poly);
    MEMZERO_LOC(str_tmp);
}

polyseed_status polyseed_decode(const char* str,
    polyseed_coin coin, const polyseed_lang** lang_out,
    polyseed_data** seed_out) {

    assert(str != NULL);
    assert((gf_elem)coin < GF_SIZE);
    assert(seed_out != NULL);
    CHECK_DEPS();

    polyseed_str str_tmp;
    polyseed_phrase words;
    gf_poly poly = { 0 };
    polyseed_status res;
    polyseed_data* seed;

    /* canonical decomposition */
    UTF8_DECOMPOSE(str, str_tmp);

    /* make sure the string is null-terminated */
    str_tmp[POLYSEED_STR_SIZE - 1] = '\0';

    /* split into words */
    if (str_split(str_tmp, words) != POLYSEED_NUM_WORDS) {
        res = POLYSEED_ERR_NUM_WORDS;
        goto cleanup;
    }

    /* decode words into polynomial coefficients */
    if (!polyseed_phrase_decode(words, poly.coeff, lang_out)) {
        res = POLYSEED_ERR_LANG;
        goto cleanup;
    }

    /* finalize polynomial */
    poly.coeff[RS_NUM_CHECK_DIGITS] ^= coin;
    poly.degree = POLY_MAX_DEGREE;

    /* checksum */
    if (!polyseed_rs_check(&poly)) {
        res = POLYSEED_ERR_CHECKSUM;
        goto cleanup;
    }

    /* alocate memory */
    seed = ALLOC(sizeof(polyseed_data));

    if (seed == NULL) {
        res = POLYSEED_ERR_MEMORY;
        goto cleanup;
    }

    /* decode polynomial into seed data */
    polyseed_poly_to_data(&poly, seed);

    /* nonzero reserved bits indicate a future version of the seed */
    if (seed->reserved != RESERVED_VALUE) {
        polyseed_free(seed);
        res = POLYSEED_ERR_UNSUPPORTED;
        goto cleanup;
    }

    *seed_out = seed;
    res = POLYSEED_OK;

cleanup:
    MEMZERO_LOC(str_tmp);
    MEMZERO_LOC(words);
    MEMZERO_LOC(poly);
    return res;
}

static inline void store32(uint8_t* p, uint32_t u) {
    *p++ = (uint8_t)u;
    u >>= 8;
    *p++ = (uint8_t)u;
    u >>= 8;
    *p++ = (uint8_t)u;
    u >>= 8;
    *p++ = (uint8_t)u;
}

void polyseed_keygen(const polyseed_data* seed, polyseed_coin coin,
    size_t key_size, uint8_t* key_out) {

    assert(seed != NULL);
    assert((gf_elem)coin < GF_SIZE);
    assert(key_out != NULL);
    CHECK_DEPS();

    uint8_t salt[32] = "POLYSEED";
    salt[9] = 0xff;
    salt[10] = 0xff;
    salt[11] = 0xff;
    store32(&salt[12], coin);           /* domain separate by coin */
    store32(&salt[16], seed->birthday); /* domain separate by birthday */
    store32(&salt[20], seed->reserved); /* domain separate by reserved bits */
    
    PBKDF2_SHA256(seed->secret, SECRET_BUFFER_SIZE, salt, sizeof(salt),
        KDF_NUM_ITERATIONS, key_out, key_size);
}

void polyseed_store(const polyseed_data* seed, polyseed_storage storage) {
    assert(seed != NULL);
    assert(storage != NULL);

    polyseed_data_store(seed, storage);
}

polyseed_status polyseed_load(const polyseed_storage storage,
    polyseed_data** seed_out) {

    assert(storage != NULL);
    assert(seed_out != NULL);

    gf_poly poly = { 0 };
    polyseed_status res;
    polyseed_data* seed;

    /* alocate memory */
    seed = ALLOC(sizeof(polyseed_data));

    if (seed == NULL) {
        return POLYSEED_ERR_MEMORY;
    }

    /* deserialize data */
    res = polyseed_data_load(storage, seed);
    if (res != POLYSEED_OK) {
        polyseed_free(seed);
        return res;
    }

    /* encode polynomial with the existing checksum */
    poly.coeff[0] = seed->checksum;
    poly.degree = 1;
    polyseed_data_to_poly(seed, &poly);

    /* checksum */
    if (!polyseed_rs_check(&poly)) {
        res = POLYSEED_ERR_CHECKSUM;
        polyseed_free(seed);
    }
    else {
        res = POLYSEED_OK;
        *seed_out = seed;
    }

    MEMZERO_LOC(poly);
    return res;
}
