/*************************************************************************************************
Copyright Nikon Electronic Imaging Department - All rights reserved
*************************************************************************************************/

#ifndef	_NKTYPES_
#define	_NKTYPES_

#ifndef _WIN32
	#include <string.h>
	#include <stdio.h>
#else
	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	#include <stdlib.h>
	#include <windows.h>
	#include <tchar.h>
#endif

#include <limits.h>
#include "Nkstdint.h"
#include "NkEndian.h"

typedef double	DOUB_P;
typedef	void*	LPVPTR;
typedef	unsigned short	UWORD;
typedef	short	SWORD;

#ifdef _WIN32
//	_TEXT(x) macro already defined
// _T(x) macro already defined
// LPCSTR typedef already defined
// LPCTSTR typedef already defined
// BYTE typedef already defined
// FAR macro already defined
	typedef	_TUCHAR	UCHAR;
	typedef	_TSCHAR	SCHAR;
	typedef	_TCHAR	CHAR;
	typedef	SIZE_T	NK_SIZE_T;
	typedef SSIZE_T	NK_SSIZE_T;
	typedef unsigned __int64 NK_UINT_64;
#else
	#if !defined( FAR )
		#define  FAR
	#endif
	#define	_TEXT(x)		((char*)x)
	#ifndef	_T	//	_T is defined by wxWindows, so there would be no need to redefine it
		#define	_T(x)			((char*)x)
	#endif
	typedef	unsigned char	UCHAR;
	typedef	signed char		SCHAR;
	typedef	char			CHAR;
	typedef	unsigned char	BYTE;
	typedef	const UCHAR*	LPCSTR;
	typedef	const char*		LPCTSTR;
	typedef	unsigned char	UINT8;
	typedef	unsigned short	UINT16;
	typedef	uint32_t		UINT32;
	typedef	size_t			NK_SIZE_T;
	typedef	long			NK_SSIZE_T;
	typedef	unsigned long long NK_UINT_64;
#endif


// Changed this from #ifndef BOOL to #ifndef WIN32 because of a bug in Visual C++ 2.2
// Changed it to a short integer type to retain compatibility with present Mac implementation
// Changed this from #ifndef WIN32 to #if __APPLE__ because it is the proper way of doing it
#if __APPLE__
	#if !defined( BOOL ) && !defined( BOOL_DEFINED )
		#if !defined( __OBJC__ )
			typedef	char	BOOL;
			#define BOOL_DEFINED 1;	//	this helps fix some nastiness in CodeWarrior
		#endif
	#endif
#endif

// The code below doesn't do what the author thinks it does.
// #ifdef only works with macros, not type definitions.
// Since we all love defining and using ULONG all over the place, I changed
// the check for a ULONG_DEFINED 

#ifndef ULONG_DEFINED
#if !defined(ULONG) && !defined(_WIN32)
	typedef	uint32_t	ULONG;
#endif
#define ULONG_DEFINED
#endif
	
#ifndef SLONG
	typedef	int32_t	SLONG;
#endif

#ifndef NKPARAM
#if defined(__APPLE__) && defined(__LP64__)
    typedef	NK_UINT_64	NKPARAM;// Mac64 pointer or unsigned long long integer
#elif defined(_WIN64)
    typedef	NK_UINT_64	NKPARAM;// Win64 pointer or unsigned long long integer
#else
    typedef	ULONG	NKPARAM;    // Win32/Mac32 pointer or uint32_t integer
#endif
#endif

#ifndef LPVOID
	typedef	void FAR*	LPVOID;
#endif

#ifndef NKREF
	typedef	LPVOID	NKREF;
#endif

#ifndef FALSE
	#define	FALSE false
#endif

#ifndef TRUE
	#define	TRUE true
#endif

#define	kMaxULONG ULONG_MAX	// The maximum value of a ULONG type

// @func void * | NK_DEREF |
// This is how we do the first dereference of a handle.  Always use this macro if you want to
// retain cross-platform capability.
#ifdef _WIN32
	#define NK_DEREF(a)	((LPVPTR)a)	// Windows actually is a single dereference
#else
	// Macs use a double dereference
	#define NK_DEREF(a)	(StripAddress(*((Ptr*)(a))))
#endif

// make sure we know what NULL and nil mean
#ifndef NULL
	#define NULL	0
#endif

#ifndef nil
	#define nil		NULL
#endif

typedef SLONG	NKERROR;

#if __APPLE__
	typedef	char*	NkFileID;
#else
	#ifdef __cplusplus
		class CNkString;
		typedef	CNkString	NkFileID;
	#else
		typedef	UCHAR*		NkFileID;
	#endif
#endif

#ifdef _WIN32
#	if defined( _MSC_VER ) && _MSC_VER >= 1400
#		ifdef _SPRINTF
#			undef _SPRINTF
#		endif
#		define _SPRINTF	_stprintf_s

#		ifdef _STRCPY
#			undef _STRCPY
#		endif
#		define _STRCPY	_tcscpy_s

#		ifdef _STRNCPY
#			undef _STRNCPY
#		endif
#		define _STRNCPY	_tcsncpy_s

#		ifdef _STRLEN
#			undef _STRLEN
#		endif
#		define _STRLEN	_tcslen_s

#		ifdef _STRCAT
#			undef _STRCAT
#		endif
#		define _STRCAT	_tcscat_s
#	else
#		define _SPRINTF	_stprintf
#		define _STRCPY	_tcscpy
#		define _STRNCPY	_tcsncpy
#		define _STRLEN	_tcslen
#		define _STRCAT	_tcscat
#	endif
#else
	#define _SPRINTF	sprintf
	#define _STRCPY	strcpy
	#define _STRNCPY	strncpy
	#define _STRLEN	strlen
	#define _STRCAT	strcat
#endif

// these macros are used in various places

#ifndef CLEARMEM
	#define CLEARMEM( dst, sz ) ::memset( (dst), 0, (sz) )
#endif

#ifndef MOVEMEM
	#define MOVEMEM( dst, src, sz ) ::memmove( (dst), (src), (sz) )
#endif

#ifndef NEW_INSTANCE
	#if defined( _WIN32 ) && defined( _DEBUG )
		// Add some debugging information to the new operator
		#ifndef _CRTDBG_MAP_ALLOC
			#define	_CRTDBG_MAP_ALLOC
		#endif
		#include <crtdbg.h>
		#define	NEW_INSTANCE		new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#else
		#define	NEW_INSTANCE		new
	#endif
#endif

#ifndef DELETE_INSTANCE
	#define DELETE_INSTANCE delete
#endif

#ifdef _WIN32
	// these are the members of the POINT structure
	#define POINTX x
	#define POINTY y
#else
	// these are the members of the POINT structure
#ifdef Infinity_Lib
	#define POINTX x
	#define POINTY y
#else
	#define POINTX h
	#define POINTY v
#endif

#endif

#endif

//================================================================================================
