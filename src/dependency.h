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

#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include "polyseed.h"

#include <assert.h>
#include <stdbool.h>

extern polyseed_dependency polyseed_deps;

#define CHECK_DEPS() do {\
    assert(polyseed_deps.randbytes != NULL); \
    assert(polyseed_deps.pbkdf2_sha256 != NULL); \
    assert(polyseed_deps.memzero != NULL); \
    assert(polyseed_deps.u8_nfc != NULL); \
    assert(polyseed_deps.u8_nfkd != NULL); \
    assert(polyseed_deps.time != NULL); \
    assert(polyseed_deps.alloc != NULL); \
    assert(polyseed_deps.free != NULL); } while(false)

/* only normalize strings that contain non-ASCII characters */
static size_t utf8_nfkd_lazy(const char* str, polyseed_str norm) {
    size_t size = 0;
    const unsigned char* pos = (const unsigned char*)str;
    while (*pos != '\0' && size < POLYSEED_STR_SIZE - 1) {
        if (*pos >= 0x80) { /* non-ASCII */
            return polyseed_deps.u8_nfkd(str, norm);
        }
        norm[size] = *pos;
        pos++;
        size++;
    }
    norm[size] = '\0';
    return size;
}

#define GET_RANDOM_BYTES(a, b) polyseed_deps.randbytes((a), (b))
#define PBKDF2_SHA256(pw, pwlen, salt, saltlen, iter, key, keylen) \
    polyseed_deps.pbkdf2_sha256((pw), (pwlen), (salt), (saltlen), (iter), \
    (key), (keylen))
#define MEMZERO_LOC(x) polyseed_deps.memzero((void*)&(x), sizeof(x))
#define MEMZERO_PTR(x, type) polyseed_deps.memzero((x), sizeof(type))
#define UTF8_COMPOSE(a, b) polyseed_deps.u8_nfc((a), (b))
#define UTF8_DECOMPOSE(a, b) utf8_nfkd_lazy((a), (b))
#define GET_TIME() polyseed_deps.time()
#define ALLOC(x) polyseed_deps.alloc(x)
#define FREE(x) polyseed_deps.free(x)

#endif
