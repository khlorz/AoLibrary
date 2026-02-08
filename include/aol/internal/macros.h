#pragma once

/***************************************************************************************
* AoLibrary Macros
****************************************************************************************
* - All macros used by the library will be used here
* - Macros must always start with AOL_ to not pollute the namespace
***************************************************************************************/

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


