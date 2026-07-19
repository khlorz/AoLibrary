/***************************************************************************************
* AoLibrary Allocators - Strings
****************************************************************************************
* - Allocators used for strings. Strings might needed a specialize allocator
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_ALLOCATORS_STRINGS_H
#define AOL_HEADER_INTERNAL_ALLOCATORS_STRINGS_H


#include "aol/configs.h"

#if defined(AOL_CONFIG_FLAG_USE_MIMALLOCATOR_ALLOCATOR) || defined(AOL_CONFIG_FLAG_USE_MIMALLOCATOR_POOL_ALLOCATOR)
#include "mimalloc/mimalloc.h"
#elif define(AOL_CONFIG_FLAG_USE_BOOST_POOL_ALLOCATOR)
#include "boost/pool/pool_alloc.hpp"
#endif

#include <memory>	// std::allocator


namespace AoL
{

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
#ifdef AOL_CONFIG_FLAG_USE_MIMALLOCATOR_STRING_ALLOCATOR
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
#ifdef AOL_CONFIG_FLAG_USE_MIMALLOCATOR_POOL_ALLOCATOR
= mi_heap_stl_allocator<char>;
#elif AOL_CONFIG_FLAG_USE_BOOST_POOL_ALLOCATOR
= boost::pool_allocator<char>;
#else
#error "#define a different pool allocator!"
#endif

} // AoL namespace

#endif // AOL_HEADER_INTERNAL_ALLOCATORS_STRINGS_H