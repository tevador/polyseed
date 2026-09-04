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

#ifndef GF_H
#define GF_H

#include "polyseed.h"

#include <stdint.h>
#include <stdbool.h>

#define GF_BITS 11
#define GF_SIZE (1u << GF_BITS)
#define GF_MASK (GF_SIZE - 1)
#define POLY_NUM_CHECK_DIGITS 1

typedef uint_fast16_t gf_elem;

extern gf_elem polyseed_mul2_table[8];

typedef struct gf_poly {
    gf_elem coeff[POLYSEED_NUM_WORDS];
} gf_poly;

static inline gf_elem gf_elem_mul2(gf_elem x) {
    if (x < 1024) {
        return 2 * x;
    }
    return polyseed_mul2_table[x % 8] + 16 * ((x - 1024) / 8);
}

static gf_elem gf_poly_eval(const gf_poly* poly) {
    /* Horner's method at x = 2 */
    gf_elem result = poly->coeff[POLYSEED_NUM_WORDS - 1];
    for (int i = POLYSEED_NUM_WORDS - 2; i >= 0; --i) {
        result = gf_elem_mul2(result) ^ poly->coeff[i];
    }
    return result;
}

static inline void gf_poly_encode(gf_poly* message) {
    message->coeff[0] = gf_poly_eval(message);
}

static inline bool gf_poly_check(const gf_poly* message) {
    return gf_poly_eval(message) == 0;
}

POLYSEED_PRIVATE
void polyseed_data_to_poly(const polyseed_data* data, gf_poly* poly);

POLYSEED_PRIVATE
void polyseed_poly_to_data(const gf_poly* poly, polyseed_data* data);

#endif
