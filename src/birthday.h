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

#ifndef BIRTHDAY_H
#define BIRTHDAY_H

#include <stdint.h>
#include <assert.h>

#define EPOCH ((uint64_t)1635768000)  /* 1st November 2021 12:00 UTC */
#define TIME_STEP ((uint64_t)2629746) /* 30.436875 days = 1/12 of the Gregorian year */

#define DATE_BITS 10
#define DATE_MASK ((1u << DATE_BITS) - 1)

static inline unsigned birthday_encode(uint64_t time) {
    /* Handle broken time() implementations. */
    if (time == (uint64_t)-1 || time < EPOCH) {
        return 0;
    }
    return ((time - EPOCH) / TIME_STEP) & DATE_MASK;
}

static inline uint64_t birthday_decode(unsigned birthday) {
    return EPOCH + birthday * TIME_STEP;
}

#endif
