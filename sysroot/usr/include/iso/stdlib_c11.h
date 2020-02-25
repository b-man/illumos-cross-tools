/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright 2016 Joyent, Inc.
 */

/*
 * An application should not include this header directly.  Instead it
 * should be included only through the inclusion of other illumos headers.
 *
 * The contents of this header is limited to identifiers specified in
 * the C11 standard and in conflict with the C++ implementation of the
 * standard header.  The C++ standard may adopt the C11 standard at
 * which point it is expected that the symbols included here will
 * become part of the C++ std namespace.
 */

#ifndef _ISO_STDLIB_C11_H
#define	_ISO_STDLIB_C11_H

#include <sys/feature_tests.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __cplusplus >= 199711L
namespace std {
#endif

/*
 * The following have been added as a result of the ISO/IEC 9899:2011
 * standard. For a strictly conforming C application, visibility is
 * contingent on the value of __STDC_VERSION__ (see sys/feature_tests.h).
 * For non-strictly conforming C applications, there are no restrictions
 * on the C namespace.
 */

/*
 * Work around fix-includes and other bad actors with using multiple headers.
 */
#if !defined(_NORETURN_KYWD)
#if __STDC_VERSION__ - 0 >= 201112L
#define	_NORETURN_KYWD	_Noreturn
#else
#define	_NORETURN_KYWD
#endif	/* __STDC_VERSION__ - 0 >= 201112L */
#endif	/* !defined(_NORETURN_KYWD) */

#if !defined(_STRICT_SYMBOLS) || defined(_STDC_C11)
extern void *aligned_alloc(size_t, size_t);
#endif /* !_STRICT_SYMBOLS || _STDC_C11 */

#if !defined(_STRICT_SYMBOLS) || defined(_STDC_C11) || __cplusplus >= 201103L
extern int at_quick_exit(void (*)(void));
extern _NORETURN_KYWD void quick_exit(int);
#endif /* !_STRICT_SYMBOLS || _STDC_C11 || __cplusplus >= 201103L */

#if __cplusplus >= 199711L
}
#endif

/*
 * ISO C11 Annex K functions are not allowed to be in the standard
 * namespace; however, it is implementation-defined as to whether or
 * not they are in the global namespace and we opt to make them
 * available to software.
 */
#if __EXT1_VISIBLE

#ifndef	_ERRNO_T_DEFINED
#define	_ERRNO_T_DEFINED
typedef int errno_t;
#endif

/* K.3.6 */
typedef void (*constraint_handler_t)(const char *_RESTRICT_KYWD,
    void *_RESTRICT_KYWD, errno_t);
/* K.3.6.1.1 */
extern constraint_handler_t set_constraint_handler_s(constraint_handler_t);
/* K.3.6.1.2 */
extern _NORETURN_KYWD void abort_handler_s(const char *_RESTRICT_KYWD,
    void *_RESTRICT_KYWD, errno_t);
/* K3.6.1.3 */
extern void ignore_handler_s(const char *_RESTRICT_KYWD, void *_RESTRICT_KYWD,
    errno_t);
#endif	/* __EXT1_VISIBLE */

#ifdef __cplusplus
}
#endif

#endif /* _ISO_STDLIB_C11_H */
