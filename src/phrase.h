/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef PHRASE_H
#define PHRASE_H

#include "polyseed.h"

typedef const char* polyseed_phrase[POLYSEED_NUM_WORDS];

POLYSEED_PRIVATE void polyseed_encode_internal(const polyseed_data* data,
    const polyseed_lang* lang, polyseed_coin coin, polyseed_phrase words_out);

POLYSEED_PRIVATE polyseed_status polyseed_decode_internal(
    const polyseed_phrase words, polyseed_coin coin,
    const polyseed_lang** lang_out, polyseed_data* data_out);

#endif
