/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "polyseed.h"

#include <assert.h>
#include <stdbool.h>

extern polyseed_dependency polyseed_deps;

#define CHECK_DEPS() do {\
    assert(polyseed_deps.rand != NULL); \
    assert(polyseed_deps.pbkdf2_sha256 != NULL); \
    assert(polyseed_deps.u8_nfc != NULL); \
    assert(polyseed_deps.u8_nfkd != NULL); \
    assert(polyseed_deps.time != NULL); \
    assert(polyseed_deps.memzero != NULL); \
    assert(polyseed_deps.alloc != NULL); \
    assert(polyseed_deps.free != NULL); } while(false)

#define GET_RANDOM_BYTES(a, b) polyseed_deps.rand((a), (b))
#define PBKDF2_SHA256(pw, pwlen, salt, saltlen, iter, key, keylen) \
    polyseed_deps.pbkdf2_sha256((pw), (pwlen), (salt), (saltlen), (iter), \
    (key), (keylen))
#define UTF8_COMPOSE(a, b) polyseed_deps.u8_nfc((a), (b))
#define UTF8_DECOMPOSE(a, b) polyseed_deps.u8_nfkd((a), (b))
#define GET_TIME() polyseed_deps.time(NULL)
#define MEMZERO_LOC(x) polyseed_deps.memzero((void*)&(x), sizeof(x))
#define MEMZERO_PTR(x, type) polyseed_deps.memzero((x), sizeof(type))
#define ALLOC(x) polyseed_deps.alloc(x)
#define FREE(x) polyseed_deps.free(x)

#endif
