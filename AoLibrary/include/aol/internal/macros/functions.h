/***************************************************************************************
* AoLibrary Macros - Functions
****************************************************************************************
* - All macros that acts as functions are defined/implemented here
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_MACROS_FUNCTION_H
#define AOL_HEADER_INTERNAL_MACROS_FUNCTION_H


#include "aol/configs.h"
#include "aol/internal/macros/defines.h"


/**
* We use each platform's prefetch function
* Cannot proceed with no prefetch function
*/
#if AOL_COMPILER_GCC || AOL_COMPILER_CLANG
#define BS_PREFETCH(addr) __builtin_prefetch(addr)
#elif AOL_COMPILER_MSVC
#include <xmmintrin.h>
#define AOL_MACRO_FUNC_PREFETCH(addr) _mm_prefetch((const char*)(addr), _MM_HINT_T0)
#else
#error "No prefetch function! Create one or use a different function!"
#endif


#endif // AOL_HEADER_INTERNAL_MACROS_FUNCTION_H