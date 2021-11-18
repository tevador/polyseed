/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#include "polyseed.h"
#include "dependency.h"
#include "phrase.h"
#include "birthday.h"
#include "lang.h"
#include "gf.h"
#include "rs_code.h"

#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define RESERVED_BITS 5
#define SECRET_BITS 150
#define TOTAL_BITS GF_BITS * POLYSEED_NUM_WORDS
#define KDF_NUM_ITERATIONS 4096

void polyseed_create(polyseed_data* data_out) {
    assert(data_out != NULL);
    CHECK_DEPS();
    data_out->birthday = birthday_encode(GET_TIME());
    data_out->reserved = 0;
    GET_RANDOM_BYTES(data_out->secret, POLYSEED_SECRET_SIZE);
    unsigned rem_bits = SECRET_BITS;
    for (int i = 0; i < POLYSEED_SECRET_SIZE; ++i) {
        unsigned mask_bits = rem_bits > (sizeof(uint8_t) * CHAR_BIT)
            ? (sizeof(uint8_t) * CHAR_BIT)
            : rem_bits;
        uint8_t mask = (1u << mask_bits) - 1;
        data_out->secret[i] &= mask;
        rem_bits -= mask_bits;
    }
}

time_t polyseed_get_birthday(const polyseed_data* data) {
    assert(data != NULL);
    return birthday_decode(data->birthday);
}

void polyseed_encode_internal(const polyseed_data* data, const polyseed_lang* lang,
    polyseed_coin coin, polyseed_phrase words_out) {

    gf_poly poly = { 0 };
    unsigned rem_bits = GF_BITS;
    polyseed_gf_write(&poly, &rem_bits, data->birthday, DATE_BITS);
    polyseed_gf_write(&poly, &rem_bits, data->reserved, RESERVED_BITS);
    unsigned seed_rem_bits = SECRET_BITS;
    for (int i = 0; seed_rem_bits > 0; ++i) {
        unsigned chunk_bits = MIN((unsigned)CHAR_BIT, seed_rem_bits);
        seed_rem_bits -= chunk_bits;
        polyseed_gf_write(&poly, &rem_bits, data->secret[i], chunk_bits);
    }
    assert(rem_bits == 0);

    polyseed_rs_encode(&poly);

    poly.coeff[RS_NUM_CHECK_DIGITS] ^= coin;

    for (int i = 0; i < POLYSEED_NUM_WORDS; ++i) {
        words_out[i] = lang->words[poly.coeff[i]];
    }
}

static void write_str(char** pos, const char* str) {
    char* loc = *pos;
    while (*str != '\0') {
        *loc = *str;
        ++str;
        ++loc;
    }
    *pos = loc;
}

void polyseed_encode(const polyseed_data* data, const polyseed_lang* lang,
    polyseed_coin coin, polyseed_str str_out) {

    assert(data != NULL);
    assert(lang != NULL);
    assert(str_out != NULL);
    CHECK_DEPS();

    polyseed_phrase words;

    polyseed_encode_internal(data, lang, coin, words);

    polyseed_str str_tmp;
    char* pos = str_tmp;
    int w;

    for (w = 0; w < POLYSEED_NUM_WORDS - 1; ++w) {
        write_str(&pos, words[w]);
        write_str(&pos, lang->separator);
    }
    write_str(&pos, words[w]);
    *pos = '\0';

    if (lang->compose) {
        UTF8_COMPOSE(str_tmp, str_out);
    }
    else {
        strncpy(str_out, str_tmp, POLYSEED_STR_SIZE);
    }
}

polyseed_status polyseed_decode_internal(const polyseed_phrase words,
    polyseed_coin coin, const polyseed_lang** lang_out,
    polyseed_data* data_out) {
    
    gf_poly poly = { 0 };
    if (!polyseed_phrase_decode(words, poly.coeff, lang_out)) {
        return POLYSEED_ERR_LANG;
    }
    poly.coeff[RS_NUM_CHECK_DIGITS] ^= coin;
    poly.degree = POLY_MAX_DEGREE;
    if (!polyseed_rs_check(&poly)) {
        return POLYSEED_ERR_CHECKSUM;
    }

    unsigned used_bits = RS_NUM_CHECK_DIGITS * GF_BITS;
    data_out->reserved = 0;
    data_out->birthday = 0;
    memset(data_out->secret, 0, POLYSEED_SECRET_SIZE);

    polyseed_gf_read(&poly, &used_bits, &data_out->birthday, DATE_BITS);
    polyseed_gf_read(&poly, &used_bits, &data_out->reserved, RESERVED_BITS);

    unsigned seed_rem_bits = SECRET_BITS;
    for (int i = 0; seed_rem_bits > 0; ++i) {
        unsigned chunk_bits = MIN((unsigned)CHAR_BIT, seed_rem_bits);
        seed_rem_bits -= chunk_bits;
        unsigned chunk_data = data_out->secret[i];
        polyseed_gf_read(&poly, &used_bits, &chunk_data, chunk_bits);
        data_out->secret[i] = chunk_data;
    }

    assert(used_bits == TOTAL_BITS);

    if (data_out->reserved != 0) {
        return POLYSEED_ERR_RESERVED;
    }

    return POLYSEED_OK;
}

polyseed_status polyseed_decode(const char* str,
    polyseed_coin coin, const polyseed_lang** lang_out,
    polyseed_data* data_out) {

    assert(str != NULL);
    assert((gf_elem)coin < GF_SIZE);
    assert(data_out != NULL);
    CHECK_DEPS();

    polyseed_str str_tmp;

    /* canonical decomposition */
    UTF8_DECOMPOSE(str, str_tmp);

    str_tmp[POLYSEED_STR_SIZE - 1] = '\0';

    polyseed_phrase words;

    char* pos = str_tmp;
    char* word = str_tmp;
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
    }

    if (w != POLYSEED_NUM_WORDS) {
        return POLYSEED_ERR_NUM_WORDS;
    }

    return polyseed_decode_internal(words, coin, lang_out, data_out);
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

void polyseed_keygen(const polyseed_data* data, polyseed_coin coin,
    size_t key_size, uint8_t* key_out) {

    assert(data != NULL);
    assert((gf_elem)coin < GF_SIZE);
    assert(key_out != NULL);
    CHECK_DEPS();

    uint8_t salt[32] = "POLYSEED v1";
    store32(&salt[12], coin);           /* domain separate by coin */
    store32(&salt[16], data->birthday); /* domain separate by birthday */
    store32(&salt[20], data->reserved); /* domain separate by reserved bits */
    
    PBKDF2_SHA256(data->secret, POLYSEED_SECRET_SIZE, salt, sizeof(salt),
        KDF_NUM_ITERATIONS, key_out, key_size);
}
