/***************************************************************************************
* AoLibrary insert ordered set
****************************************************************************************
* - The library's insert ordered set
***************************************************************************************/
#ifndef AOL_HEADER_INSERT_ORDERED_SET_H
#define AOL_HEADER_INSERT_ORDERED_SET_H


#include "configs.h"
#include "macros.h"
#include "traits.h"
#include "types.h"
#include "hashes.h"
#include "allocators.h"

#if defined(AOL_CONFIG_FLAG_USE_TSL_INSERTORDERED_SET)
#include "tsl/ordered_set.h"
#endif

#include <utility>
#include <functional>


namespace AoL
{

/**
* @details Insertion-ordered associative set
*
* - As with other sets, this won't have any duplicate values
*
* - Unlike KeyOrderSet, this will always be sorted by insertion, so take note.
*
* - For the most part, each map aliases will have the same interfaces. Although some map type have additional interfaces
*
* - By default, AoLibrary uses tsl::ordered_map
*
* -- For more information, go to 'github.com/Tessil/ordered-map'
*
* - Read and look at the defines at include/libconfig.h for more information on the type aliases
*
* @tparam T Element type
* @tparam H Hash class/function (default: ankerl::unordered_dense::hash<T>)
* @tparam A Allocator type (default: Internal::DefaultAllocator<T>)
*/
template<
	typename T,
	typename H = Internal::DefaultHash<T>,
	typename A = DefaultAllocator<T>
>
using InsertOrderSet
#if defined(AOL_CONFIG_FLAG_USE_TSL_INSERTORDERED_MAP)
= tsl::ordered_set<T, H, std::equal_to<T>, A>;
#else
#error "No custom insert ordered map set yet!"
#endif

/**
* @details InsertOrderSet but specialized for pool allocators
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
using InsertOrderSetPool = InsertOrderSet<T, H, A>;

} // AoL namespace


#endif // !AOL_HEADER_INSERT_ORDERED_SET_H