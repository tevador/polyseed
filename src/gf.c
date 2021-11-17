/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#include "gf.h"
#include "dependency.h"

void polyseed_gf_write(gf_poly* poly, unsigned* rem_bits, unsigned value,
    unsigned bits) {
    if (*rem_bits == 0) {
        poly->degree++;
        *rem_bits = GF_BITS;
    }
    unsigned digit_bits = MIN(*rem_bits, bits);
    unsigned rest_bits = bits - digit_bits;
    *rem_bits -= digit_bits;
    poly->coeff[poly->degree] |= ((value >> rest_bits)
        & ((1u << digit_bits) - 1)) << *rem_bits;
    if (rest_bits > 0) {
        polyseed_gf_write(poly, rem_bits, value & ((1u << rest_bits) - 1),
            rest_bits);
    }
}

void polyseed_gf_read(const gf_poly* poly, unsigned* used_bits, unsigned* value,
    unsigned bits) {
    unsigned coeff_index = *used_bits / GF_BITS;
    unsigned bit_index = *used_bits % GF_BITS;
    unsigned digit_bits = MIN((unsigned)GF_BITS - bit_index, bits);
    unsigned rem_bits = GF_BITS - bit_index - digit_bits;
    unsigned rest_bits = bits - digit_bits;
    *value |= ((poly->coeff[coeff_index] >> rem_bits)
        & ((1u << digit_bits) - 1)) << rest_bits;
    *used_bits += digit_bits;
    if (rest_bits > 0) {
        polyseed_gf_read(poly, used_bits, value, rest_bits);
    }
}
