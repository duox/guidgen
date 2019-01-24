/*pch.h*/
#define STRICT
#define _WIN32_WINNT	0x0500

#include <windows.h>
#include <windowsx.h>

#include <crtdbg.h>
#include <tchar.h>

#include <string>

#pragma warning(disable: 4481)	//: nonstandard extension used: override specifier 'abstract'

#ifndef _MSC_VER_71
# define _MSC_VER_71		1310
#endif // ndef _MSC_VER_71
#define _MSC_VER_2015		1500

#if !defined abstract && _MSC_VER < 1800
# define abstract	=0
#endif // ndef abstract
#if !defined override && _MSC_VER < 1800
# define override
#endif // ndef !defined override && _MSC_VER < 1800

#ifdef _MSC_VER_71
# define _itoa_s( val, buf, size, radix )		_itoa( val, buf, radix )
# define _snprintf_s							_snprintf
#endif

#undef nullptr
#if _MSC_VER < _MSC_VER_2015
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

#define countof(a)	( sizeof(a) / sizeof(*a) )

/*END OF pch.h*/
