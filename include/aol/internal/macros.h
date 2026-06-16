/***************************************************************************************
* AoLibrary Macros
****************************************************************************************
* - All macros used by the library will be used here
* - Macros must always start with AOL_ to not pollute the namespace
***************************************************************************************/
#ifndef AOL_INTERNAL_MACROS_H
#define AOL_INTERNAL_MACROS_H

/**
* Empty base optimization
* - To fix the MSVC's problem with EBO
* - Always use this on every empty class to keep the EBO working across compilers
*/
#ifdef _MSC_VER
#define AOL_EMPTY_BASE_OPTIMIZATION __declspec(empty_bases)
#else
#define AOL_EMPTY_BASE_OPTIMIZATION
#endif

/**
* Attributes used throughout the library
*/
#define AOL_NO_DISCARD [[nodiscard]]
#define AOL_BRANCH_UNLIKELY [[unlikely]]
#define AOL_BRANCH_LIKELY [[likely]]

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

/**
* Counting trailing ones
*/
#if __cplusplus >= 202002L
#include <bit>
#define AOL_MACRO_FUNC_COUNTR_ONE(x) std::countr_one(x)
#else
#error "The library must be on C++20 standard!"
#endif


#endif // AOL_INTERNAL_MACROS_H