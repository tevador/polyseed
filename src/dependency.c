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

#include "polyseed.h"
#include "dependency.h"
#include "lang.h"

#include <stdlib.h>
#include <time.h>

POLYSEED_PRIVATE polyseed_dependency polyseed_deps;

static uint64_t stdlib_time() {
    return (uint64_t)time(NULL);
}

void polyseed_inject(const polyseed_dependency* deps) {
    polyseed_deps = *deps;
    if (polyseed_deps.time == NULL) {
        polyseed_deps.time = &stdlib_time;
    }
    if (polyseed_deps.alloc == NULL) {
        polyseed_deps.alloc = &malloc;
    }
    if (polyseed_deps.free == NULL) {
        polyseed_deps.free = &free;
    }
    CHECK_DEPS();

    /* self-test */
#ifndef NDEBUG
    for (int i = 0; i < polyseed_get_num_langs(); ++i) {
        const polyseed_lang* lang = polyseed_get_lang(i);
        polyseed_lang_check(lang);
    }
#endif
}
