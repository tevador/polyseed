/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef FEATURES_H
#define FEATURES_H

#include "storage.h"

#include <stdbool.h>

#define FEATURES_DEFAULT 0
#define FEATURE_BITS 5
#define ENCRYPTED_MASK 1
#define RESERVED_MASK 0x1e

static bool is_encrypted(unsigned features) {
    return (features & ENCRYPTED_MASK) != 0;
}

static bool is_supported(unsigned features) {
    return (features & RESERVED_MASK) == 0;
}

#endif
