/***************************************************************************************
* AoLibrary Allocators - General
****************************************************************************************
* - Allocators used for general purposes (allocator for most containers, etc.)
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_ALLOCATORS_GENERAL_H
#define AOL_HEADER_INTERNAL_ALLOCATORS_GENERAL_H


#include "aol/configs.h"

#if defined(AOL_CONFIG_FLAG_USE_MIMALLOCATOR_ALLOCATOR)
#include "mimalloc/mimalloc.h"
#elif defined(AOL_CONFIG_FLAG_USE_STD_ALLOCATOR)
#include <memory> // std::allocator
#endif


namespace AoL
{

/**
* Allocator used for containers
*
* - The default will be std::allocator<T>
*
* - However, the new and delete are overridden by mimalloc's new and delete by default too
*
* - This allocator will be used by all containers from AoL by default
*
* - AoLibrary has no custom allocator yet, but std::allocator already does the job really well
*
* @tparam T value type
*/
template<typename T>
using DefaultAllocator
#ifdef AOL_CONFIG_FLAG_USE_MIMALLOCATOR_ALLOCATOR
= mi_stl_allocator<T>;
#elif defined(AOL_CONFIG_FLAG_USE_STD_ALLOCATOR)
= std::allocator<T>;
#else
#error "No custom allocator yet!"
#endif

} // AoL namespace


#endif // AOL_HEADER_INTERNAL_ALLOCATORS_GENERAL_H