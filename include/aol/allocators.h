/***************************************************************************************
* AoLibrary library
****************************************************************************************
* - The library's main allocator
* - The default allocator used by the library whenever no custom allocator is provided
* - Do be careful when integrating types from the library that uses a different
*	allocator as it might be a problem when moving or copying objects
***************************************************************************************/
#ifndef AOL_HEADER_ALLOCATORS_H
#define AOL_HEADER_ALLOCATORS_H


#include "internal/allocators/general.h"
#include "internal/allocators/pool.h"
#include "internal/allocators/strings.h"


#endif // AOL_HEADER_ALLOCATORS_H