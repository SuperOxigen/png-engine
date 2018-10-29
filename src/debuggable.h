/*
 *  Image-Formats - Debuggable
 *      Some macros to help with the debugging processes.
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */

#ifndef _DEBUGGABLE_H_
#define _DEBUGGABLE_H_

#if defined(_DEBUG) && defined(__GNUC__)

#define WARN_UNUSED_RESULT __attribute__ ((warn_unused_result))
#define NORETURN __attribute__ ((noreturn))
#define NONNULL __attribute__ ((nonnull))
#define DEPRICATED __attribute__((deprecated))

#define __FUNC__ __FUNCTION__

#define FORMAT_PRINTF(fmtidx, varidx) __attribute__((format (printf, fmtidx, varidx)))

#else /* if no DEBUG */

#define WARN_UNUSED_RESULT
#define NORETURN
#define NONNULL
#define DEPRICATED

#define __FUNC__ "n/a"

#define FORMAT_PRINTF(fmtidx, varidx)

#endif /* no DEBUG */

#endif /* _DEBUGGABLE_H_ */
