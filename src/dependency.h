/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#ifdef _MSC_VER
#pragma warning(error: 4013) /* calls to undefined functions */
#pragma warning(error: 4090) /* different const qualifiers */
#pragma warning(error: 4133) /* incompatible pointer types */
#pragma warning(disable: 4146) /* unary minus applied to unsigned type */
#endif

#include "polyseed.h"

#include <assert.h>
#include <stdbool.h>

extern polyseed_dependency polyseed_deps;

static inline void check_dependency() {
    assert(polyseed_deps.rand != NULL);
    assert(polyseed_deps.pbkdf2_sha256 != NULL);
    assert(polyseed_deps.u8_nfc != NULL);
    assert(polyseed_deps.u8_nfkd != NULL);
}

#define GET_RANDOM_BYTES(a, b) polyseed_deps.rand((a), (b))
#define PBKDF2_SHA256(pw, pwlen, salt, saltlen, iter, key, keylen) \
    polyseed_deps.pbkdf2_sha256((pw), (pwlen), (salt), (saltlen), (iter), \
    (key), (keylen))
#define UTF8_COMPOSE(a, b) polyseed_deps.u8_nfc((a), (b))
#define UTF8_DECOMPOSE(a, b) polyseed_deps.u8_nfkd((a), (b))

#endif
