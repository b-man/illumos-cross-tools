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
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 *
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Copyright 1989, 1994 by Mortice Kern Systems Inc.
 * All rights reserved.
 */

/*
 * Copyright 2017 Nexenta Systems, Inc.
 */

#ifndef	_REGEX_H
#define	_REGEX_H

#include <sys/feature_tests.h>
#include <sys/types.h>

#ifdef	__cplusplus
extern "C" {
#endif


/*
 * wchar_t is a built-in type in standard C++ and as such is not
 * defined here when using standard C++. However, the GNU compiler
 * fixincludes utility nonetheless creates its own version of this
 * header for use by gcc and g++. In that version it adds a redundant
 * guard for __cplusplus. To avoid the creation of a gcc/g++ specific
 * header we need to include the following magic comment:
 *
 * we must use the C++ compiler's type
 *
 * The above comment should not be removed or changed until GNU
 * gcc/fixinc/inclhack.def is updated to bypass this header.
 */
#if !defined(__cplusplus) || (__cplusplus < 199711L && !defined(__GNUG__))
#ifndef _WCHAR_T
#define	_WCHAR_T
#if defined(_LP64)
typedef int	wchar_t;
#else
typedef long    wchar_t;
#endif
#endif	/* !_WCHAR_T */
#endif	/* !defined(__cplusplus) ... */

typedef ssize_t regoff_t;

/* regcomp flags */
#define	REG_BASIC	0x00000
#define	REG_EXTENDED	0x00001		/* Use Extended Regular Expressions */
#define	REG_NOSUB	0x00002		/* Don't set subexpression */
#define	REG_ICASE	0x00004		/* Ignore case in match */
#define	REG_NEWLINE	0x00008		/* Treat \n as regular character */
#define	REG_DELIM	0x00010		/* legacy, no effect */
#define	REG_DEBUG	0x00020		/* legacy, no effect */
#define	REG_ANCHOR	0x00040		/* legacy, no effect */
#define	REG_WORDS	0x00080		/* legacy, no effect */
#define	REG_EGREP	0x01000		/* legacy, no effect */
#define	REG_DUMP	0x02000		/* internal */
#define	REG_PEND	0x04000		/* NULs are ordinary characters */
#define	REG_NOSPEC	0x08000		/* no special characters */

/* internal flags */
#define	REG_MUST	0x00100		/* legacy, no effect */

/* regexec flags */
#define	REG_NOTBOL	0x00200		/* string is not BOL */
#define	REG_NOTEOL	0x00400		/* string has no EOL */
#define	REG_NOOPT	0x00800		/* legacy, no effect */
#define	REG_STARTEND	0x10000		/* match whole pattern */
#define	REG_TRACE	0x20000		/* tracing of execution */
#define	REG_LARGE	0x40000		/* force large representation */
#define	REG_BACKR	0x80000		/* force use of backref code */

/* regcomp and regexec return codes */
#define	REG_OK		0		/* success (non-standard) */
#define	REG_NOMATCH	1		/* regexec failed to match */
#define	REG_ECOLLATE	2		/* invalid collation element ref. */
#define	REG_EESCAPE	3		/* trailing \ in pattern */
#define	REG_ENEWLINE	4		/* \n found before end of pattern */
#define	REG_ENSUB	5		/* more than 9 \( \) pairs (OBS) */
#define	REG_ESUBREG	6		/* number in \[0-9] invalid */
#define	REG_EBRACK	7		/* [ ] imbalance */
#define	REG_EPAREN	8		/* ( ) imbalance */
#define	REG_EBRACE	9		/* \{ \} imbalance */
#define	REG_ERANGE	10		/* invalid endpoint in range */
#define	REG_ESPACE	11		/* no memory for compiled pattern */
#define	REG_BADRPT	12		/* invalid repetition */
#define	REG_ECTYPE	13		/* invalid char-class type */
#define	REG_BADPAT	14		/* syntax error */
#define	REG_BADBR	15		/* \{ \} contents bad */
#define	REG_EFATAL	16		/* internal error, not POSIX.2 */
#define	REG_ECHAR	17		/* invalid multibyte character */
#define	REG_STACK	18		/* backtrack stack overflow */
#define	REG_ENOSYS	19		/* function not supported (XPG4) */
#define	REG__LAST	20		/* first unused code */
#define	REG_EBOL	21		/* ^ anchor and not BOL */
#define	REG_EEOL	22		/* $ anchor and not EOL */
#define	REG_ATOI	255		/* convert name to number (!) */
#define	REG_ITOA	256		/* convert number to name (!) */

#define	_REG_BACKREF_MAX 9		/* Max # of subexp. backreference */

typedef struct {		/* regcomp() data saved for regexec() */
	size_t  re_nsub;	/* # of subexpressions in RE pattern */

	/*
	 * Internal use only.  Note that any changes to this structure
	 * have to preserve sizing, as it is baked into applications.
	 */
	struct re_guts *re_g;
	int re_magic;
	const char *re_endp;

	/* here for compat */
	size_t	re_len;		/* # wchar_t chars in compiled pattern */
	struct _regex_ext_t *re_sc;	/* for binary compatibility */
} regex_t;

/* subexpression positions */
typedef struct {
	const char	*rm_sp, *rm_ep;	/* Start pointer, end pointer */
	regoff_t	rm_so, rm_eo;	/* Start offset, end offset */
	int		rm_ss, rm_es;	/* Used internally */
} regmatch_t;


/*
 * IEEE Std 1003.2 ("POSIX.2") regular expressions API.
 */

extern int regcomp(regex_t *_RESTRICT_KYWD, const char *_RESTRICT_KYWD, int);
extern int regexec(const regex_t *_RESTRICT_KYWD, const char *_RESTRICT_KYWD,
    size_t, regmatch_t *_RESTRICT_KYWD, int);
extern size_t regerror(int, const regex_t *_RESTRICT_KYWD, char *_RESTRICT_KYWD,
    size_t);
extern void regfree(regex_t *);

#ifdef	__cplusplus
}
#endif

#endif	/* _REGEX_H */
