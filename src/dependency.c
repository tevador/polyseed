/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#include "polyseed.h"
#include "dependency.h"

#include <stdlib.h>
#include <time.h>

POLYSEED_PRIVATE polyseed_dependency polyseed_deps;

void polyseed_inject(const polyseed_dependency* deps) {
    polyseed_deps = *deps;
    if (polyseed_deps.time == NULL) {
        polyseed_deps.time = &time;
    }
    if (polyseed_deps.alloc == NULL) {
        polyseed_deps.alloc = &malloc;
    }
    if (polyseed_deps.free == NULL) {
        polyseed_deps.free = &free;
    }
    CHECK_DEPS();
}
