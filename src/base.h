/*
 *  Image-Formats - Base
 *      Common types for the project.
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */

#ifndef _BASE_H_
#define _BASE_H_

#include <stdint.h>
#include <stdbool.h>

typedef bool bool_t;

typedef enum {
    STATUS_OK,
    STATUS_FAILURE,
    STATUS_ILLEGAL_ARGUMENT,
    STATUS_NULL_ARGUMENT,
    STATUS_BAD_CRC,
    STATUS_UNKNOWN_TYPE,
    STATUS_OUT_OF_MEMORY,
    STATUS_INCOMPLETE_PACKET,
    STATUS_BAD_PACKET
} status_t;

/*
 *  Global constants.
 */

/* For most of PNG u32 types have a maximum value of 2^31-1. */
static uint32_t const kSigned32Max = 0x7fffffffu;

#endif /* _BASE_H_ */
