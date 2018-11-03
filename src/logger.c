/*
 *  Image-Formats - Logging
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */

#include <stdio.h>
#include <stdarg.h>

#include "logger.h"

static char_t const *level_to_string(int32_t level)
{
    switch (level)
    {
        case LOGGER_TRACE:
            return "TRACE";
        case LOGGER_DEBUG:
            return "DEBUG";
        case LOGGER_INFO:
            return "INFO";
        case LOGGER_WARN:
            return "WARN";
        case LOGGER_ERROR:
            return "ERROR";
        default:
            return "XXXXX";
    }
}

int32_t loggerf(
    int32_t level, char_t const *filename, int32_t lineno, char_t const *funcname,
    char_t const *format, ...)
{
    int32_t total, ret;
    va_list args;

    total = fprintf(
        stderr, "[%5s] %s:%d (%s) ",
        level_to_string(level), filename, lineno, funcname);
    if (total < 0)
    {
        return total;
    }

    va_start(args, format);
    ret = vfprintf(stderr, format, args);
    va_end(args);

    if (ret < 0)
    {
        return ret;
    }
    total += ret;

    ret = fprintf(stderr, "\n");
    if (ret < 0)
    {
        return ret;
    }

    return total + ret;
}
