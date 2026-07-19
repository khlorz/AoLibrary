/***************************************************************************************
* AoLibrary Allocators - Pool
****************************************************************************************
* - Allocators used for pooling memory purposes
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_ALLOCATORS_POOL_H
#define AOL_HEADER_INTERNAL_ALLOCATORS_POOL_H


#include "aol/configs.h"

#if defined(AOL_CONFIG_FLAG_USE_MIMALLOCATOR_POOL_ALLOCATOR)
#include "mimalloc/mimalloc.h"
#elif define(AOL_CONFIG_FLAG_USE_BOOST_POOL_ALLOCATOR)
#include "boost/pool/pool_alloc.hpp"
#endif

#include <memory>	// std::allocator


namespace AoL
{

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
#ifdef AOL_CONFIG_FLAG_USE_MIMALLOCATOR_POOL_ALLOCATOR
= mi_heap_stl_allocator<T>;
#elif AOL_CONFIG_FLAG_USE_BOOST_POOL_ALLOCATOR
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
using DefaultStringPoolAllocator
#ifdef AOL_CONFIG_FLAG_USE_MIMALLOCATOR_POOL_ALLOCATOR
= mi_heap_stl_allocator<char>;
#elif AOL_CONFIG_FLAG_USE_BOOST_POOL_ALLOCATOR
= boost::pool_allocator<char>;
#else
#error "#define a different pool allocator!"
#endif

} // AoL namespace


#endif // AOL_HEADER_INTERNAL_ALLOCATORS_POOL_H