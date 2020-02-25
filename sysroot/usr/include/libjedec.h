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
 * Copyright (c) 2018, Joyent, Inc.
 */

#ifndef _LIBJEDEC_H
#define	_LIBJEDEC_H

/*
 * Library routines that allow access to JEDEC JEP-106 vendor data.
 */

#ifdef __cplusplus
extern "C" {
#endif

extern const char *libjedec_vendor_string(uint_t, uint_t);

#ifdef __cplusplus
}
#endif

#endif /* _LIBJEDEC_H */
