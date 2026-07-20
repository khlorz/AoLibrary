/***************************************************************************************
* AoLibrary key ordered set
****************************************************************************************
* - The library's std::unordered_set
***************************************************************************************/
#ifndef AOL_HEADER_HASH_SET_H
#define AOL_HEADER_HASH_SET_H


#include "configs.h"
#include "macros.h"
#include "traits.h"
#include "types.h"
#include "hashes.h"
#include "allocators.h"

#if defined(AOL_CONFIG_FLAG_USE_STD_UNORDERED_SET)
#include <unordered_set>
#elif defined(AOL_CONFIG_FLAG_USE_ANKERL_UNORDERED_DENSE_SET)
#include "unordered_dense/unordered_dense.h"
#endif

#include <utility>
#include <functional>


namespace AoL
{

/**
* @details Hash map but for sets
*
* - This container is not sorted/ordered but is associated with hashes of the type with no duplicates
*
* - Fast look up and fast insertion. Better suited for types where lookups are a lot.
*
* - By default, AoLibrary uses ankerl::unordered_dense::set
*
* -- For more information, go to 'github.com/martinus/unordered_dense'
*
* - Read and look at the defines at include/libconfig.h for more information on the type aliases.
*
* @tparam T Element type
* @tparam H Hash class/function (default: ankerl::unordered_dense::hash<T>)
* @tparam A Allocator type (default: Internal::DefaultPoolAllocator<T>)
*/
template<
	typename T,
	typename H = Internal::DefaultHash<T>,
	typename A = DefaultAllocator<T>
>
using HashSet
#if defined(AOL_CONFIG_FLAG_USE_STD_UNORDERED_MAP)
= std::unordered_set<T, ankerl::unordered_dense::hash<T>, std::equal_to<T>, A>;
#elif defined(AOL_CONFIG_FLAG_USE_ANKERL_UNORDERED_DENSE_MAP)
= ankerl::unordered_dense::set<T, H, std::equal_to<T>, A>;
#else
#error "No custom hash set yet!"
#endif

/**
* @details HashSet but specialized for pool allocators
*
* - Default pool allocator is backed by mimalloc
*
* - Internally operates on `mi_heap_t`
*
* @tparam T Element type
* @tparam H Hash class/function (default: ankerl::unordered_dense::hash<T>)
* @tparam A Allocator type (default: Internal::DefaultPoolAllocator<T>)
*/
template<
	typename T,
	typename H = Internal::DefaultHash<T>,
	typename A = DefaultPoolAllocator<T>
>
using HashSetPool = HashSet<T, H, A>;

} // AoL namespace


#endif