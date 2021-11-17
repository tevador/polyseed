/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#include "dependency.h"
#include "rs_code.h"
#include "gf_poly.h"

#include <assert.h>
#include <string.h>

static const gf_poly generator = {
    .coeff = { 2, 1 },
    .degree = 1,
};

void polyseed_rs_encode(gf_poly* message) {
    assert(generator.degree == RS_NUM_CHECK_DIGITS);
    assert(message != NULL);
    //gf_poly_set_degree(message);
    assert(message->degree + RS_NUM_CHECK_DIGITS <= POLY_MAX_DEGREE);

    gf_poly_shift(message, RS_NUM_CHECK_DIGITS);

    gf_poly rem;
    gf_poly quotient;

    gf_poly_div_rem(message, &generator, &quotient, &rem);
    gf_poly_add(message, &rem, message);
}

static void get_syndrome(const gf_poly* message, gf_poly* syndrome) {
    for (unsigned i = 1; i <= generator.degree; ++i) {
        syndrome->coeff[i - 1] = gf_poly_eval(message, gf_elem_exp(i));
    }
    gf_poly_set_degree(syndrome);
}

bool polyseed_rs_check(const gf_poly* message) {
    //gf_poly_set_degree(message);
    gf_poly syndrome = { 0 };
    get_syndrome(message, &syndrome);
    return gf_poly_is_zero(&syndrome);
}
