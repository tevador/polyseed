/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef RS_CODE_H
#define RS_CODE_H

#include "polyseed.h"
#include "gf.h"

#include <stdbool.h>

#define RS_NUM_CHECK_DIGITS 1

POLYSEED_PRIVATE void polyseed_rs_encode(gf_poly* message);
POLYSEED_PRIVATE bool polyseed_rs_check(const gf_poly* message);

#endif
