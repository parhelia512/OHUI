#ifndef _COMMTYPEDEF_32434534543_H
#define _COMMTYPEDEF_32434534543_H

#include <stdarg.h>
#include <BaseTsd.h> //INT_PTR
#include <string>
#include "common.h" // TARGETINFOTYPE

#if defined(_WINDOWS) || defined(__WIN32__)
 
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

 
#define LOG_BUFF_SIZE 4096

#ifndef tstring
	#ifdef _UNICODE
		#define tstring std::wstring
	#else
		#define tstring std::string
	#endif
#endif

#ifndef INT_MAX
#define INT_MAX       2147483647 
#endif

#ifndef _INTSIZEOF
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#endif

#ifndef _VA_LIST_DEFINED
#ifdef _M_CEE_PURE
typedef System::ArgIterator va_list;
#else
typedef char *  va_list;
#endif
#define _VA_LIST_DEFINED
#endif

typedef void* (*doListT)(int n, va_list arglist);

//return the n'th argument from va_list 
#ifndef va_nth_arg
 
#if     defined(_M_CEE)
#error varargs.h not supported when targetting _M_CEE (use stdarg.h)
#elif   defined(_M_IX86)
#define va_nth_arg(ap,n)    (&( *(PINT_PTR *)((ap + n*_INTSIZEOF(PINT_PTR)) - _INTSIZEOF(PINT_PTR)) ))
#elif defined(_M_IA64)
#define va_nth_arg(arglist, n)   &(*(PINT_PTR *)((arglist + n*_SLOTSIZEOF(PINT_PTR)+ _APALIGN(PINT_PTR,arglist)) -_SLOTSIZEOF(PINT_PTR)))
#elif defined(_M_AMD64)
#define va_nth_arg(ap, n)   \
	(&( ( sizeof(t) > sizeof(__int64) || ( sizeof(PINT_PTR) & (sizeof(PINT_PTR) - 1) ) != 0 ) \
	? **(PINT_PTR **)( ( ap + n* sizeof(__int64) ) - sizeof(__int64) ) \
	:  *(PINT_PTR  *)( ( ap + n* sizeof(__int64) ) - sizeof(__int64) ) ))
#else
#define va_nth_arg(ap,n) (&( *(PINT_PTR *)((ap + n* _INTSIZEOF(PINT_PTR)) - _INTSIZEOF(PINT_PTR)) ))
#endif
#endif

namespace nsTaShan
{
	//return the n'th argument from va_list 
	//INT_PTR** va_nth_arg(va_list arglist, int n);

	template<typename T>
	void* doList(T* pThis, doListT fn, int nArgumentCount, ...)
	{
		va_list arglist;
		va_start(arglist, nArgumentCount);

		void* pRtn = pThis->doList(fn, nArgumentCount, arglist);

		va_end(arglist);
		return pRtn;
	}

	enum SENDTYPE
	{ 


		COMMTEST,
	};
}
#endif
