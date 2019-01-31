/*pch.h*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief Pre-compiled header generation header file.
 *
 */
#define STRICT
#define _WIN32_WINNT	0x0500

#ifndef _MSC_VER_71
# define _MSC_VER_71		1310
#endif // ndef _MSC_VER_71
#define _MSC_VER_2015		1500

#include <windows.h>
#include <windowsx.h>

#include <crtdbg.h>
#include <tchar.h>

#ifdef _MSC_VER

// abstract and override specifiers
#pragma warning(disable: 4481)	//: nonstandard extension used: override specifier 'abstract'
#if !defined abstract && defined _MSC_VER && _MSC_VER < _MSC_VER_2015
# define abstract	=0
#endif // ndef abstract
#if !defined override && defined _MSC_VER && _MSC_VER < _MSC_VER_2015
# define override
#endif // !defined override && defined _MSC_VER && _MSC_VER < _MSC_VER_2015

// some secure functions not present in VS 2003
#if _MSC_VER <= _MSC_VER_71
# define _itoa_s( val, buf, size, radix )		_itoa( val, buf, radix )
# define _snprintf_s							_snprintf
#endif

// not using exceptions
#define  _HAS_EXCEPTIONS	0
#if _MSC_VER <= _MSC_VER_71
class exception {};	// workaround for MSVS2003 bug
#endif // _MSC_VER <= _MSC_VER_71

#endif // def _MSC_VER

// define nullptr for those compilers that do not define it
#undef nullptr
#if defined _MSC_VER && _MSC_VER < _MSC_VER_2015
# ifndef __cplusplus
#  define nullptr		0
# else
namespace std
{
	typedef int nullptr_t;
	const nullptr_t nullptr = 0;
}
using std::nullptr;
# endif
#else
# include <cstddef>
#endif
#include <string>

#ifndef countof
# define countof(a)		( sizeof(a) / sizeof(*(a)) )
#endif // ndef countof

/*END OF pch.h*/
