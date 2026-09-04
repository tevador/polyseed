/*
 * Copyright (c) 2020-2026 tevador <tevador@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LANG_H
#define LANG_H

#include "polyseed.h"

#include <stdbool.h>

#define POLYSEED_LANG_SIZE 2048

typedef struct polyseed_lang {
    const char* name;
    const char* name_en;
    const char* separator;
    bool is_sorted;
    bool has_prefix;
    bool has_accents;
    bool compose;
    const char* words[POLYSEED_LANG_SIZE];
} polyseed_lang;

typedef const char* polyseed_phrase[POLYSEED_NUM_WORDS];

POLYSEED_PRIVATE int polyseed_lang_find_word(const polyseed_lang* lang,
    const char* word);

POLYSEED_PRIVATE polyseed_status polyseed_phrase_decode(const polyseed_phrase phrase,
    uint_fast16_t idx_out[POLYSEED_NUM_WORDS], const polyseed_lang** lang_out);

POLYSEED_PRIVATE polyseed_status polyseed_phrase_decode_explicit(const polyseed_phrase phrase,
    const polyseed_lang* lang, uint_fast16_t idx_out[POLYSEED_NUM_WORDS]);

POLYSEED_PRIVATE void polyseed_lang_check(const polyseed_lang* lang);

#endif
