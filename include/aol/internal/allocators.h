#pragma once

/***************************************************************************************
* AoLibrary library
****************************************************************************************
* - The library's main allocator
* - The default allocator used by the library whenever no custom allocator is provided
* - Do be careful when integrating types from the library that uses a different
*	allocator as it might be a problem when moving or copying objects
***************************************************************************************/

#include "config.h"

#if defined(AOL_USE_MIMALLOCATOR_ALLOCATOR) || defined(AOL_USE_MIMALLOCATOR_POOL_ALLOCATOR)
#include "mimalloc/mimalloc.h"
#endif

#ifdef AOL_USE_BOOST_POOL_ALLOCATOR
#include "boost/pool/pool_alloc.hpp"
#endif

namespace AoL::Internal
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
#ifdef AOL_USE_MIMALLOCATOR_ALLOCATOR
= mi_stl_allocator<T>;
#else
= std::allocator<T>;
#endif

/**
* Allocator but for memory pools
* 
* - The default will be mi_heap_stl_allocator<T>
* 
* - RottenLibary has no custom allocator yet, but mi_heap_stl_allocator<T> is worth using
* 
* - Use this for cases with a lot of allocations and deallocations for better performance
* 
* - The 'Pool' variants of AoLibrary's containers already use this allocator by default
* 
* @tparam T value type
*/
template<typename T>
using DefaultPoolAllocator
#ifdef AOL_USE_MIMALLOCATOR_POOL_ALLOCATOR
= mi_heap_stl_allocator<T>;
#elif AOL_USE_BOOST_POOL_ALLOCATOR
= boost::pool_allocator<T>;
#else
#error "#define a different pool allocator!"
#endif

/**
* Allocators for AoLibrary's strings
* 
* - The default will be mi_stl_allocator<char>
* 
* -- From testings, mi_stl_allocator<char> performs better than std::allocator<char>
* 
* - No need to change this allocator unless for some reason, the performance degrades
*/
using DefaultStringAllocator
#ifdef AOL_USE_MIMALLOCATOR_STRING_ALLOCATOR
= mi_stl_allocator<char>;
#else
= std::allocator<char>;
#endif

/**
* Allocators for AoLibrary's strings
*
* - The default will be mi_stl_allocator<char>
*
* -- From testings, mi_stl_allocator<char> performs better than std::allocator<char>
*
* - No need to change this allocator unless for some reason, the performance degrades
*/
using DefaultStringPoolAllocator
#ifdef AOL_USE_MIMALLOCATOR_POOL_ALLOCATOR
= mi_heap_stl_allocator<char>;
#elif AOL_USE_BOOST_POOL_ALLOCATOR
= boost::pool_allocator<char>;
#else
#error "#define a different pool allocator!"
#endif

}