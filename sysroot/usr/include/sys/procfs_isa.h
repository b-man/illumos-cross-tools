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
/*
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_PROCFS_ISA_H
#define	_SYS_PROCFS_ISA_H


/*
 * Instruction Set Architecture specific component of <sys/procfs.h>
 */

#include <sys/regset.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Possible values of pr_dmodel.
 * This isn't isa-specific, but it needs to be defined here for other reasons.
 */
#define	PR_MODEL_UNKNOWN 0
#define	PR_MODEL_ILP32	1	/* process data model is ILP32 */
#define	PR_MODEL_LP64	2	/* process data model is LP64 */

/*
 * To determine whether application is running native.
 */
#define	PR_MODEL_NATIVE	PR_MODEL_LP64

typedef	uint32_t instr_t;

#define	NPRGREG		_NGREG
#define	prgreg_t	greg_t
#define	prgregset_t	gregset_t
#define	prfpregset	fpu
#define	prfpregset_t	fpregset_t

#if defined(_SYSCALL32)
/*
 * kernel view of the 32bit register set(don't use)
 */
typedef	uint32_t instr32_t;
#define	NPRGREG32	_NGREG32
#define	prgreg32_t	greg32_t
#define	prgregset32_t	gregset32_t
#define	prfpregset32	fpu32
#define	prfpregset32_t	fpregset32_t
#endif	/* _SYSCALL32 */

#define	R_PC	REG_PC
#define	R_SP	REG_SP

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_PROCFS_ISA_H */
