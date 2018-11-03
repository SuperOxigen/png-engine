/*
 *  Image-Formats - Logging
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */

#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "base.h"
#include "debuggable.h"

#define LOGGER_TRACE 0
#define LOGGER_DEBUG 1
#define LOGGER_INFO 2
#define LOGGER_WARN 3
#define LOGGER_ERROR 4

int32_t FORMAT_PRINTF(5, 6) loggerf(
    int32_t level, char_t const *filename, int32_t lineno, char_t const *funcname,
    char_t const *format, ...);

#ifdef _TRACE
#define LOG_TRACE(...) \
    loggerf(LOGGER_TRACE, __FILE__, __LINE__, __FUNC__, __VA_ARGS__)
#define TRACE_ENTRY DTRACE("Entered: " __FUNC__)
#define TRACE_EXIT DTRACE("Exit: " __FUNC__)
#define TRACE_EXIT_LABEL(message) DTRACE("Exit: " __FUNC__ ": " message)
#else /* if no _TRACE */
#define LOG_TRACE(...)
#define TRACE_ENTRY
#define TRACE_EXIT
#define TRACE_EXIT_LABEL(message)
#endif /* no _TRACE */

#ifdef _DEBUG
#define LOG_DEBUG(...) \
    loggerf(LOGGER_DEBUG, __FILE__, __LINE__, __FUNC__, __VA_ARGS__)
#else /* if no _DEBUG */
#define LOG_DEBUG(...)
#endif /* no _DEBUG */

#endif /* _LOGGING_H_ */
