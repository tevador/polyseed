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
#include "features.h"

static unsigned reserved_features = FEATURE_MASK ^ ENCRYPTED_MASK;

POLYSEED_PRIVATE bool polyseed_features_supported(unsigned features) {
    return (features & reserved_features) == 0;
}

int polyseed_enable_features(unsigned mask) {
    int num_enabled = 0;
    reserved_features = FEATURE_MASK ^ ENCRYPTED_MASK;
    for (int i = 0; i < USER_FEATURES; ++i) {
        unsigned fmask = 1u << i;
        if (mask & fmask) {
            reserved_features ^= fmask;
            num_enabled++;
        }
    }
    return num_enabled;
}
