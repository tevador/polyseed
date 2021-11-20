/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef GF_POLY_H
#define GF_POLY_H

#include "gf.h"
#include "gf_elem.h"

#include <assert.h>
#include <stdbool.h>

static inline bool gf_poly_is_zero(const gf_poly* x) {
    return x->degree == 0 && x->coeff[0] == 0;
}

static void gf_poly_set_degree(gf_poly* x) {
    unsigned degree = 0;
    for (unsigned i = 0; i <= POLY_MAX_DEGREE; ++i) {
        degree = (x->coeff[i] != 0) ? i : degree;
    }
    x->degree = degree;
}

static void gf_poly_monomial(gf_poly* x, gf_elem coeff, unsigned degree) {
    for (unsigned i = 0; i < degree; ++i) {
        x->coeff[i] = 0;
    }
    x->coeff[degree] = coeff;
    x->degree = coeff != 0 ? degree : 0;
}

static void gf_poly_copy(const gf_poly* x, gf_poly* y) {
    for (unsigned i = 0; i <= x->degree; ++i) {
        y->coeff[i] = x->coeff[i];
    }
    y->degree = x->degree;
}

static void gf_poly_zero(gf_poly* x) {
    x->coeff[0] = 0;
    x->degree = 0;
}

static void gf_poly_add(const gf_poly* x, const gf_poly* y, gf_poly* z) {
    if (x->degree < y->degree) {
        /* swap */
        const gf_poly* temp = x;
        x = y;
        y = temp;
    }
    unsigned max_degree = x->degree;
    unsigned min_degree = y->degree;
    unsigned i = 0;
    unsigned degree = 0;
    for (; i <= min_degree; ++i) {
        z->coeff[i] = x->coeff[i] ^ y->coeff[i];
        degree = (z->coeff[i] != 0) ? i : degree;
    }
    for (; i <= max_degree; ++i) {
        z->coeff[i] = x->coeff[i];
        degree = i;
    }
    z->degree = degree;
}

static void gf_poly_shift(gf_poly* x, int shift) {
    int i = x->degree += shift;
    for (; i >= shift; --i) {
        x->coeff[i] = x->coeff[i - 1];
    }
    for (; i >= 0; --i) {
        x->coeff[i] = 0;
    }
}

static void gf_poly_mul(const gf_poly* x, const gf_poly* y, gf_poly* z) {
    assert(x->degree + y->degree <= POLY_MAX_DEGREE);
    for (unsigned i = 0; i <= x->degree; ++i) {
        for (unsigned j = 0; j <= y->degree; ++j) {
            z->coeff[i + j] ^= gf_elem_mul(x->coeff[i], y->coeff[j]);
        }
    }
    unsigned degree = 0;
    for (unsigned i = 0; i <= x->degree + y->degree; ++i) {
        if (z->coeff[i] != 0) {
            degree = i;
        }
    }
    z->degree = degree;
}

static void gf_poly_div_rem(const gf_poly* nom, const gf_poly* x,
    gf_poly* quotient, gf_poly* rem) {

    assert(!gf_poly_is_zero(x));

    gf_poly_zero(quotient);
    gf_poly_copy(nom, rem);
    gf_elem divisor_term = gf_elem_inv(x->coeff[x->degree]);
    while (rem->degree >= x->degree && !gf_poly_is_zero(rem)) {
        unsigned degree_diff = rem->degree - x->degree;
        gf_elem digit = gf_elem_mul(rem->coeff[rem->degree], divisor_term);
        gf_poly mono;
        gf_poly_monomial(&mono, digit, degree_diff);
        gf_poly term = { 0 };
        gf_poly_mul(x, &mono, &term);
        gf_poly_add(quotient, &mono, quotient);
        gf_poly_add(rem, &term, rem);
    }
}

static gf_elem gf_poly_eval(const gf_poly* poly, gf_elem x) {
    if (x == 0) {
        return poly->coeff[0];
    }
    /* Horner's method */
    gf_elem result = poly->coeff[poly->degree];
    for (int i = poly->degree - 1; i >= 0; --i) {
        result = gf_elem_mul(result, x) ^ poly->coeff[i];
    }
    return result;
}

#endif
