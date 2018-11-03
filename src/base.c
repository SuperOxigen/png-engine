/*
 *  Image-Formats - Base
 *      Common types for the project.
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */

#include "base.h"

char_t const *status_string(status_t status)
{
    switch (status)
    {
        case STATUS_OK:
            return "OK";
        case STATUS_FAILURE:
            return "FAILURE";
        case STATUS_ILLEGAL_ARGUMENT:
            return "ILLEGAL_ARGUMENT";
        case STATUS_NULL_ARGUMENT:
            return "NULL_ARGUMENT";
        case STATUS_BAD_CRC:
            return "BAD_CRC";
        case STATUS_UNKNOWN_TYPE:
            return "UNKNOWN_TYPE";
        case STATUS_OUT_OF_MEMORY:
            return "OUT_OF_MEMORY";
        case STATUS_INCOMPLETE_PACKET:
            return "INCOMPLETE_PACKET";
        case STATUS_BAD_PACKET:
            return "BAD_PACKET";
    }
    return "UNKNOWN";
}
