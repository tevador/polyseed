/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef GF_ELEM_H
#define GF_ELEM_H

#include "gf.h"

#include <stdint.h>
#include <assert.h>

typedef uint_least16_t gf_storage;
typedef uint_fast16_t gf_elem;

extern gf_storage polyseed_log_table[GF_SIZE];
extern gf_storage polyseed_exp_table[2 * GF_SIZE];
extern gf_storage polyseed_inv_table[GF_SIZE];

static inline gf_elem gf_elem_mul(gf_elem a, gf_elem b) {
    if (b == 0 || a == 0)
        return 0;
    if (b == 1)
        return a;
    if (a == 1)
        return b;
    return polyseed_exp_table[polyseed_log_table[a] + polyseed_log_table[b]];
}

static inline gf_elem gf_elem_exp(gf_elem i) {
    return polyseed_exp_table[i];
}

static inline gf_elem gf_elem_inv(gf_elem i) {
    assert(i != 0);
    return i > 1 ? polyseed_inv_table[i] : 1;
}

static inline gf_elem gf_elem_add(gf_elem a, gf_elem b) {
    return a ^ b;
}

#endif
