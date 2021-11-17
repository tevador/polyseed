/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef GF_H
#define GF_H

#include "polyseed.h"

#include <stdint.h>

#define GF_BITS 11
#define GF_SIZE (1u << GF_BITS)
#define POLY_MAX_DEGREE 15

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef uint_least16_t gf_storage;
typedef uint_fast16_t gf_elem;

typedef struct gf_poly {
    gf_elem coeff[2 * (POLY_MAX_DEGREE + 1)];
    unsigned degree;
} gf_poly;

POLYSEED_PRIVATE void polyseed_gf_write(gf_poly* poly, unsigned* rem_bits,
    unsigned value, unsigned bits);

POLYSEED_PRIVATE void polyseed_gf_read(const gf_poly* poly, unsigned* used_bits,
    unsigned* value, unsigned bits);

#endif
