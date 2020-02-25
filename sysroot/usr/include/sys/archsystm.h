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
 * Copyright (c) 1993, 2010, Oracle and/or its affiliates. All rights reserved.
 */

#ifndef _SYS_ARCHSYSTM_H
#define	_SYS_ARCHSYSTM_H

/*
 * A selection of ISA-dependent interfaces
 */

#include <vm/seg_enum.h>
#include <vm/page.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _KERNEL
extern uint_t cpu_freq;
extern uint64_t cpu_freq_hz;
extern page_t *page_numtopp_alloc(pfn_t pfnum);
extern greg_t getfp(void);
extern void setgregs(klwp_t *, gregset_t);
extern void getgregs(klwp_t *, gregset_t);
extern void setfpregs(klwp_t *, fpregset_t *);
extern void getfpregs(klwp_t *, fpregset_t *);
extern int getpil(void);
extern void tenmicrosec(void);
extern void bind_hwcap(void);
extern void av_dispatch_autovect(uint_t);
extern void av_dispatch_softvect(uint_t);
extern void switch_sp_and_call(void *, void (*)(uint_t, uint_t), uint_t, uint_t);
extern int interrupts_enabled(void);
extern void fakesoftint(void);

extern void __adj_hrestime(void);
extern void pc_gethrestime(timespec_t *tp);
extern void hrtime_init(void);

#endif /* _KERNEL */

#ifdef __cplusplus
}
#endif

#endif	/* _SYS_ARCHSYSTM_H */
