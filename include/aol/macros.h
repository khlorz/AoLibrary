/***************************************************************************************
* AoLibrary Macros
****************************************************************************************
* - All macros used by the library will be used here
* - Macros must always start with AOL_ to not pollute the namespace
***************************************************************************************/
#ifndef AOL_MACROS_H
#define AOL_MACROS_H

#include "internal/macros/functions.h"

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
* Attributes used throughout the library
*/
#define AOL_ATTRIB_NO_DISCARD [[nodiscard]]
#define AOL_ATTRIB_BRANCH_UNLIKELY [[unlikely]]
#define AOL_ATTRIB_BRANCH_LIKELY [[likely]]
#define AOL_ATTRIB_NO_UNQ_ADDRESS [[no_unique_address]]

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


#endif // AOL_INTERNAL_MACROS_H