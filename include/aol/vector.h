/***************************************************************************************
* AoLibrary vector
****************************************************************************************
* - The library's std::vector
***************************************************************************************/
#ifndef AOL_HEADER_VECTOR_H
#define AOL_HEADER_VECTOR_H

#include "configs.h"
#include "allocators.h"

#if defined(AOL_CONFIG_FLAG_USE_STD_VECTOR)
#include <vector>
#endif

namespace AoL
{


/**
* @details Resizable container at runtime
*
* - Dynamically resizable container
*
* - Allocator can be customized for better use cases
*
* - For most use cases, use this until it becomes a bottleneck
*
* @tparam T Element type
* @tparam A Allocator type
*/
template<
	typename T,
	typename A = DefaultAllocator<T>
>
using Vector
#if defined(AOL_CONFIG_FLAG_USE_STD_VECTOR)
= std::vector<T, A>;
#else
#error "No custom vector yet!"
#endif

/**
* @details Vector but specialized for pool allocation
*
* - Uses a pool-based allocator
*
* - Default pool allocator is backed by mimalloc
*
* - Internally operates on `mi_heap_t`
*
* @tparam T Element type
* @tparam A Allocator type
*/
template<
	typename T,
	typename A = DefaultPoolAllocator<T>
>
using VectorPool = Vector<T, A>;


} // AoL namespace


#endif // AOL_HEADER_VECTOR_H