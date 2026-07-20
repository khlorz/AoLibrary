/***************************************************************************************
* AoLibrary Macros - Defines
****************************************************************************************
* - All macros that does not fall into function or attribute category
* - Pretty much other macros used in the code base that I cannot properly categorize
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_MACROS_DEFINES_H
#define AOL_HEADER_INTERNAL_MACROS_DEFINES_H


#include "aol/configs.h"


/**
* Portable C++ standard version
* - MSVC defines __cplusplus as 199711L unless /Zc:__cplusplus is set
* - _MSVC_LANG always reflects the actual standard
* - GCC/Clang define __cplusplus correctly
*/
#if defined(_MSC_VER) && !defined(__clang__)
#define AOL_CXX_STANDARD _MSVC_LANG
#else
#define AOL_CXX_STANDARD __cplusplus
#endif

#define AOL_CXX17_OR_LATER  (AOL_CXX_STANDARD >= 201703L)
#define AOL_CXX20_OR_LATER  (AOL_CXX_STANDARD >= 202002L)
#define AOL_CXX23_OR_LATER  (AOL_CXX_STANDARD >= 202302L)


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
* Debug macro for better readability in the codebase
* Strictly for use in this library only
*/
#if defined(NDEBUG)
#define AOL_DEBUG_ON 0
#define AOL_DEBUG_OFF 1
#else
#define AOL_DEBUG_ON 1
#define AOL_DEBUG_OFF 0
#endif


#endif // AOL_HEADER_INTERNAL_MACROS_DEFINES_H