/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
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

/*
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma once

#include <sys/cpuvar.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* PILs associated with cross calls */
#define	XC_CPUPOKE_PIL	11	/* poke to cause wakeup, no service function */
#define	XC_SYS_PIL	13	/* should be defined elsewhere */
#define	XC_HI_PIL	15	/* cross call with service function */
#define	XCALL_PIL	XC_HI_PIL /* alias for XC_HI_PIL */

#ifndef _ASM

typedef uintptr_t xc_arg_t;
typedef int (*xc_func_t)(xc_arg_t, xc_arg_t, xc_arg_t);

/*
 * Cross-call routines.
 */
#if defined(_KERNEL)

#if defined(_MACHDEP)
extern void	xc_init(void);
extern void	xc_call(xc_arg_t, xc_arg_t, xc_arg_t, cpuset_t set, xc_func_t);
extern void	xc_sync(xc_arg_t, xc_arg_t, xc_arg_t, cpuset_t set, xc_func_t);
#endif

#endif	/* _KERNEL */

#endif	/* !_ASM */

#ifdef	__cplusplus
}
#endif

