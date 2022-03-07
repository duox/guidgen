/*pch.h*/
// SPDX-License-Identifier: LGPL-2.1+
/** @file
 *
 * @brief Pre-compiled header generation header file.
 *
 */
#define STRICT
#define _WIN32_WINNT	0x0501

#define	_MSC_VER_50		1100	/* 5.0 */
#define	_MSC_VER_60		1200	/* 6.0 (98) */
#define	_MSC_VER_70		1300	/* 7.0	*/
#define	_MSC_VER_71		1310	/* 2003	7.1	*/
#define	_MSC_VER_80		1400	/* 2005	*/
#define	_MSC_VER_90		1500	/* 2008	*/
#define	_MSC_VER_100	1600	/* 2010	*/
#define	_MSC_VER_110	1700	/* 2012	*/
#define	_MSC_VER_120	1800	/* 2013	*/
#define	_MSC_VER_140	1900	/* 2015	*/

#define	_MSC_2003	_MSC_VER_71		/* 2003	7.1	*/
#define	_MSC_2005	_MSC_VER_80		/* 2005	*/
#define	_MSC_2008	_MSC_VER_90		/* 2008	*/
#define	_MSC_2010	_MSC_VER_100	/* 2010	*/
#define	_MSC_2012	_MSC_VER_110	/* 2012	*/
#define	_MSC_2013	_MSC_VER_120	/* 2013	*/
#define	_MSC_2015	_MSC_VER_140	/* 2015	*/

#ifdef _MSC_VER

// abstract and override specifiers
#pragma warning(disable: 4481)	//: nonstandard extension used: override specifier 'abstract'
#if !defined abstract && defined _MSC_VER && _MSC_VER < _MSC_2015
# define abstract	=0
#endif // ndef abstract
#if !defined override && defined _MSC_VER && _MSC_VER < _MSC_2015
# define override
#endif // !defined override && defined _MSC_VER && _MSC_VER < _MSC_2015

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
#if defined _MSC_VER && _MSC_VER < _MSC_2015
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

#include <windows.h>
#include <windowsx.h>

#include <crtdbg.h>
#include <tchar.h>

#ifndef countof
# define countof(a)		( sizeof(a) / sizeof(*(a)) )
#endif // ndef countof

/*END OF pch.h*/
