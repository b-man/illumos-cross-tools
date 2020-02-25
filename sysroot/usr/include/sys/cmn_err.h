/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved	*/


/*
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 *
 * Copyright 2013 Nexenta Systems, Inc.  All rights reserved.
 * Copyright 2016 Joyent, Inc.
 */

#ifndef _SYS_CMN_ERR_H
#define	_SYS_CMN_ERR_H

#if !defined(_ASM) && (defined(_KERNEL) || defined(_FAKE_KERNEL))
#include <sys/va_list.h>
#endif

#include <sys/dditypes.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* Common error handling severity levels */

#define	CE_CONT		0	/* continuation		*/
#define	CE_NOTE		1	/* notice		*/
#define	CE_WARN		2	/* warning		*/
#define	CE_PANIC	3	/* panic		*/
#define	CE_IGNORE	4	/* print nothing	*/

#if !defined(_ASM) && (defined(_KERNEL) || defined(_FAKE_KERNEL))

/*PRINTFLIKE2*/
extern void cmn_err(int, const char *, ...)
    __KPRINTFLIKE(2);

extern void vzcmn_err(zoneid_t, int, const char *, __va_list)
    __KVPRINTFLIKE(3);

extern void dev_err(dev_info_t *, int, char *, ...)
    __KPRINTFLIKE(3);

extern void vdev_err(dev_info_t *, int, const char *, __va_list)
    __KVPRINTFLIKE(3);

extern void vcmn_err(int, const char *, __va_list)
    __KVPRINTFLIKE(2);

/*PRINTFLIKE3*/
extern void zcmn_err(zoneid_t, int, const char *, ...)
    __KPRINTFLIKE(3);

/*PRINTFLIKE1*/
extern void printf(const char *, ...)
    __KPRINTFLIKE(1);

extern void vzprintf(zoneid_t, const char *, __va_list)
    __KVPRINTFLIKE(2);

/*PRINTFLIKE2*/
extern void zprintf(zoneid_t, const char *, ...)
    __KPRINTFLIKE(2);

extern void vprintf(const char *, __va_list)
    __KVPRINTFLIKE(1);

/*PRINTFLIKE1*/
extern void uprintf(const char *, ...)
    __KPRINTFLIKE(1);

extern void vuprintf(const char *, __va_list)
    __KVPRINTFLIKE(1);

/*PRINTFLIKE3*/
extern size_t snprintf(char *, size_t, const char *, ...)
    __KPRINTFLIKE(3);
extern size_t vsnprintf(char *, size_t, const char *, __va_list)
    __KVPRINTFLIKE(3);
/*PRINTFLIKE2*/
extern char *sprintf(char *, const char *, ...)
    __KPRINTFLIKE(2);
extern char *vsprintf(char *, const char *, __va_list)
    __KVPRINTFLIKE(2);

/*PRINTFLIKE1*/
extern void panic(const char *, ...)
    __KPRINTFLIKE(1) __NORETURN;

extern void vpanic(const char *, __va_list)
    __KVPRINTFLIKE(1) __NORETURN;

#endif /* !_ASM && _KERNEL ... */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_CMN_ERR_H */
