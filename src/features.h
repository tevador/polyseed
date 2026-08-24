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

#ifndef FEATURES_H
#define FEATURES_H

#include "storage.h"

#include <stdbool.h>

#define FEATURE_BITS 5
#define FEATURE_MASK ((1u<<FEATURE_BITS)-1)

#define INTERNAL_FEATURES 2
#define USER_FEATURES 3
#define USER_FEATURES_MASK ((1<<USER_FEATURES)-1)
#define ENCRYPTED_MASK 16

static inline unsigned make_features(unsigned user_features) {
    return user_features & USER_FEATURES_MASK;
}

static inline unsigned get_features(unsigned features, unsigned mask) {
    return features & (mask & USER_FEATURES_MASK);
}

static inline bool is_encrypted(unsigned features) {
    return (features & ENCRYPTED_MASK) != 0;
}

POLYSEED_PRIVATE bool polyseed_features_supported(unsigned features);

#endif
