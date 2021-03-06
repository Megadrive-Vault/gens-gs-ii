/**
 * \file popt/system.h
 */
#pragma once

#include "config.h"

#ifdef _MSC_VER
/* Gens/GS II: MSVC doesn't support 'inline' in C mode. */
/* It's also missing some sys/stat.h macros. */
#define inline __inline
#ifndef S_ISREG
#define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
#endif
#endif /* _MSC_VER */

/* Gens/GS II: Only use GCC visibility if it's supported. */
/* TODO: 4.2.0 minimum, or older? */
#if !defined(_WIN32) && \
    defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#ifndef HAVE_GCC_VISIBILITY
#define HAVE_GCC_VISIBILITY 1
#endif
#endif

#if defined (__GLIBC__) && defined(__LCLINT__)
/*@-declundef@*/
/*@unchecked@*/
extern __const __int32_t *__ctype_tolower;
/*@unchecked@*/
extern __const __int32_t *__ctype_toupper;
/*@=declundef@*/
#endif

/**
 * Define _GNU_SOURCE:
 * - glibc-2.17: Required for secure_getenv().
 * - MinGW-w64: Required for vasprintf().
 */
#define _GNU_SOURCE 1

#include <ctype.h>

/* XXX isspace(3) has i18n encoding signednesss issues on Solaris. */
#define	_isspaceptr(_chp)	isspace((int)(*(unsigned char *)(_chp)))

#include <errno.h>
#include <fcntl.h>
#include <limits.h>

#ifdef HAVE_MCHECK_H
#include <mcheck.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
/* Gens/GS II: MSVC complains about const casts. */
/* Also, include wchar.h here to prevent redefinitions. */
#include <wchar.h>
#define free(ptr) free((void*)(ptr))
#define realloc(ptr, size) realloc((void*)(ptr), (size))
#define memcpy(dest, src, n) memcpy((void*)(dest), (src), (n))

/*  Needed for open(), lseek(), _access(), etc. */
#include <io.h>

/* MSVC doesn't have ssize_t. */
/* FIXME: Large file support on MSVC? */
typedef long ssize_t;

/**
 * MSVC 2013 (12.0) added proper support for strtoll() and strtoull().
 * Older verisons don't have these functions, but they do have
 * the equivalent functions _strtoi64() and _strtoui64().
 */
#if _MSC_VER < 1800
#define strtoll(nptr, endptr, base)  _strtoi64(nptr, endptr, base)
#define strtoull(nptr, endptr, base) _strtoui64(nptr, endptr, base)
#endif /* _MSC_VER < 1800 */

// _execvp() is defined in process.h.
#include <process.h>
#define execvp(file, argv) _execvp(file, argv)
#endif /* _MSC_VER */

#if defined(HAVE_UNISTD_H) && !defined(__LCLINT__)
#include <unistd.h>
#endif

#ifdef __NeXT
/* access macros are not declared in non posix mode in unistd.h -
 don't try to use posix on NeXTstep 3.3 ! */
#include <libc.h>
#endif

#ifdef HAVE_GCC_VISIBILITY
#pragma GCC visibility push(hidden)
#endif
/*@-incondefs@*/
/*@mayexit@*/ /*@only@*/ /*@out@*/ /*@unused@*/
void * xmalloc (size_t size)
	/*@globals errno @*/
	/*@ensures maxSet(result) == (size - 1) @*/
	/*@modifies errno @*/;

/*@mayexit@*/ /*@only@*/ /*@unused@*/
void * xcalloc (size_t nmemb, size_t size)
	/*@ensures maxSet(result) == (nmemb - 1) @*/
	/*@*/;

/*@mayexit@*/ /*@only@*/ /*@unused@*/
void * xrealloc (/*@null@*/ /*@only@*/ void * ptr, size_t size)
	/*@ensures maxSet(result) == (size - 1) @*/
	/*@modifies *ptr @*/;

/*@mayexit@*/ /*@only@*/ /*@unused@*/
char * xstrdup (const char *str)
	/*@*/;
/*@=incondefs@*/

#if !defined(HAVE_STPCPY)
/* Copy SRC to DEST, returning the address of the terminating '\0' in DEST.  */
static inline char * stpcpy (char *dest, const char * src) {
    register char *d = dest;
    register const char *s = src;

    do
	*d++ = *s;
    while (*s++ != '\0');
    return d - 1;
}
#endif

#ifdef HAVE_GCC_VISIBILITY
#pragma GCC visibility pop
#endif

/* Memory allocation via macro defs to get meaningful locations from mtrace() */
#if defined(HAVE_MCHECK_H) && defined(__GNUC__)
#define	vmefail()	(fprintf(stderr, "virtual memory exhausted.\n"), exit(EXIT_FAILURE), NULL)
#define	xmalloc(_size) 		(malloc(_size) ? : vmefail())
#define	xcalloc(_nmemb, _size)	(calloc((_nmemb), (_size)) ? : vmefail())
#define	xrealloc(_ptr, _size)	(realloc((_ptr), (_size)) ? : vmefail())
#define xstrdup(_str)   (strcpy((malloc(strlen(_str)+1) ? : vmefail()), (_str)))
#else
#define	xmalloc(_size) 		malloc(_size)
#define	xcalloc(_nmemb, _size)	calloc((_nmemb), (_size))
#define	xrealloc(_ptr, _size)	realloc((_ptr), (_size))
#define	xstrdup(_str)	strdup(_str)
#endif  /* defined(HAVE_MCHECK_H) && defined(__GNUC__) */

#ifndef HAVE_SECURE_GETENV
#  ifdef HAVE___SECURE_GETENV
#    define secure_getenv __secure_getenv
#  else
     /* Gens/GS II: secure_getenv() is implemented in system.h. */
/*#    error neither secure_getenv nor __secure_getenv is available*/
char *popt_secure_getenv(const char *name);
#    define secure_getenv popt_secure_getenv
#  endif
#endif

#if !defined(__GNUC__) && !defined(__attribute__)
#define __attribute__(x) 
#endif
#define UNUSED(x) x __attribute__((__unused__))

#include "popt.h"
