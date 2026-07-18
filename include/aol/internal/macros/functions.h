/***************************************************************************************
* AoLibrary Macros - Functions
****************************************************************************************
* - All macros that acts as functions are defined/implemented here
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_MACROS_FUNCTION_H
#define AOL_HEADER_INTERNAL_MACROS_FUNCTION_H


/**
* We use each platform's prefetch function
* Cannot proceed with no prefetch function
*/
#if defined(__GNUC__) || defined(__clang__)
#define BS_PREFETCH(addr) __builtin_prefetch(addr)
#elif defined(_MSC_VER)
#include <xmmintrin.h>
#define AOL_MACRO_FUNC_PREFETCH(addr) _mm_prefetch((const char*)(addr), _MM_HINT_T0)
#else
#error "No prefetch function! Create one or use a different function!"
#endif


#endif // AOL_HEADER_INTERNAL_MACROS_FUNCTION_H