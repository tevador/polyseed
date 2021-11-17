/* Copyright (c) 2020-2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef BIRTHDAY_H
#define BIRTHDAY_H

#include <time.h>

#define EPOCH ((time_t)1635768000)  /* 1st November 2021 12:00 UTC */
#define TIME_STEP ((time_t)2629746) /* 30.436875 days = 1/12 of the Gregorian year */

#define DATE_BITS 10
#define DATE_MASK ((1u << DATE_BITS) - 1)

static inline unsigned birthday_encode(time_t time) {
    return ((time - EPOCH) / TIME_STEP) & DATE_MASK;
}

static inline time_t birthday_decode(unsigned birthday) {
    return EPOCH + birthday * TIME_STEP;
}

#endif
