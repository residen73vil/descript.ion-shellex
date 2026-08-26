#ifndef COMPILER_COMPAT_H
#define COMPILER_COMPAT_H

#define STRINGIZE_IMPL(x) #x
#define STRINGIZE(x) STRINGIZE_IMPL(x)

/* Identify the compiler family */
#if defined(_MSC_VER)
	#pragma message("_MSC_VER is " STRINGIZE(_MSC_VER))
	#if _MSC_VER == 1200
		#define COMPILER_MSVC600
		#pragma message("MSVC600 detected")
	#elif _MSC_VER == 1400
		#define COMPILER_MSVC1400 2
		#pragma message("MSVC1400 detected")
	#endif
#elif defined(__MINGW32__) || defined(__MINGW64__)
	#define COMPILER_MINGW
	#pragma message("MINGW detected")
#endif



/* Identify architecture */
#if defined(_M_X64) || defined(_M_AMD64) || \
		defined(__x86_64__) || defined(__amd64__)
	#define COMPILER_64
	#pragma message("64bit detected")
#else
	#define COMPILER_32
	#pragma message("32bit detected")

#endif

#if defined(COMPILER_MSVC600)
	#define GWLP_USERDATA GWL_USERDATA
	#define GWLP_ID GWL_ID
	#define CINT UINT
	#define RINT INT
	#define R2INT UINT
	typedef long LONG_PTR;
#else
	#define CINT UINT_PTR
	#define RINT INT_PTR
	#define R2INT UINT
	#include <shobjidl.h>
#endif

#endif
