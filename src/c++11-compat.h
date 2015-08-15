/***************************************************************************
 * c++11-compat.h: C++ 2011 compatibility header.                          *
 *                                                                         *
 * Copyright (c) 2011-2015 by David Korth.                                 *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           *
 ***************************************************************************/

#ifndef __CXX11_COMPAT_H__
#define __CXX11_COMPAT_H__

#if !defined(__cplusplus)

/**
 * We're compiling C code.
 * Provide replacements for C++ 2011 functionality.
 */
#define CXX11_COMPAT_NULLPTR
#define CXX11_COMPAT_OVERRIDE
#define CXX11_COMPAT_CHARTYPES
#define CXX11_COMPAT_STATIC_ASSERT

#else

#if defined(__GNUC__)

/* Some versions of gcc implement parts of C++11, but not all of it. */

/* Explicit virtual override: Added in gcc-4.7. */
#if (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7))
#define CXX11_COMPAT_OVERRIDE
#endif

/* nullptr: Added in gcc-4.6 */
#if (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6))
#define CXX11_COMPAT_NULLPTR
#endif

/* New character types: Added in gcc-4.4 */
#if (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 4))
#define CXX11_COMPAT_CHARTYPES
#endif

/* Static assertions: Added in gcc-4.3 (first version to support C++11) */
#if (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3))
#define CXX11_COMPAT_STATIC_ASSERT
#endif

#elif defined(_MSC_VER)

#if (_MSC_VER < 1900)
/**
 * MSVC 2015 (14.0) added support for Unicode character types.
 * (char16_t, char32_t, related string types)
 */
#define CXX11_COMPAT_CHARTYPES
#endif

#if (_MSC_VER < 1700)
/**
 * MSVC 2010 (10.0) does support override, but not final.
 * However, it has a "sealed" keyword that works almost
 * the same way as final.
 */
#define final sealed
#endif

#if (_MSC_VER < 1600)
/**
 * MSVC 2008 (9.0) and older: No C++ 2011 support at all.
 * Probably won't compile at all due to lack of stdint.h.
 */
#define CXX11_COMPAT_NULLPTR
#define CXX11_COMPAT_OVERRIDE
#define CXX11_COMPAT_STATIC_ASSERT
#endif

#endif /* compiler-specific */

#endif

/* nullptr: Represents a NULL pointer. NULL == 0 */
#ifdef CXX11_COMPAT_NULLPTR
#define nullptr 0
#endif

/* static_assert(): Compile-time assertions. */
#ifdef CXX11_COMPAT_STATIC_ASSERT
#define static_assert(expr, msg) switch (0) { case 0: case (expr): ; }
#endif

/* Unicode characters and strings. */
#ifdef CXX11_COMPAT_CHARTYPES
#include <stdint.h>
typedef uint16_t char16_t;
typedef uint32_t char32_t;

#ifdef __cplusplus

#include <string>
namespace std {
	typedef basic_string<char16_t> u16string;
	typedef basic_string<char32_t> u32string;
}
#endif /* __cplusplus */
#endif /* CXX11_COMPAT_CHARTYPES */

/* Explicit override/final. */
#ifdef CXX11_COMPAT_OVERRIDE
#define override
#define final
#endif

/** Other compatibility stuff. **/

/**
 * MSVC (and old gcc) doesn't have __func__.
 * Reference: http://gcc.gnu.org/onlinedocs/gcc/Function-Names.html
 */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
# if (defined(__GNUC__) && __GNUC__ >= 2) || defined(_MSC_VER)
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

/**
 * Older versions of MSVC aren't C99-compliant, but they
 * do have equivalent functions with different names.
 */
#ifdef _MSC_VER
#include "msvc-c99-compat.h"
#endif

/**
 * MSVC doesn't have typeof(), but as of MSVC 2010,
 * it has decltype(), which is essentially the same thing.
 */
#ifdef _MSC_VER
#define typeof(x) decltype(x)
#endif

#endif /* __CXX11_COMPAT_H__ */
