/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#include "polyseed.h"
#include "dependency.h"

POLYSEED_PRIVATE polyseed_dependency polyseed_deps;

void polyseed_inject(const polyseed_dependency* deps) {
    polyseed_deps = *deps;
    CHECK_DEPS();
}
