/***************************************************************************************
* AoLibrary insert ordered map
****************************************************************************************
* - The library's insert ordered map
***************************************************************************************/
#ifndef AOL_HEADER_INSERT_ORDERED_MAP_H
#define AOL_HEADER_INSERT_ORDERED_MAP_H


#include "configs.h"
#include "macros.h"
#include "traits.h"
#include "types.h"
#include "allocators.h"
#include "hashes.h"

#if defined(AOL_CONFIG_FLAG_USE_TSL_INSERTORDERED_MAP)
#include "tsl/ordered_map.h"
#endif


namespace AoL
{

/**
* @details Insert-value pair type used by InsertOrderedMap
*
* - This will be the main pair type used for InsertOrderMap type
*
* - Each aliases will have the same, more or less, interfaces so they are interchangeable depending on the map used
*
* - Read and look at the defines at include/libconfig.h for more information on the type aliases
*
* @tparam K Key type
* @tparam V Mapped value type
*/
template<
	typename K,
	typename V
>
using InsertOrderMapPair
#if defined(AOL_CONFIG_FLAG_USE_TSL_INSERTORDERED_MAP)
= std::pair<K, V>;
#else
#error "No custom insert ordered map pair yet!"
#endif

/**
* @details Insertion-ordered associative map
*
* - Compared to KeyOrderMap, this map is sorted by the order of insertion.
*
* - For the most part, each map aliases will have the same interfaces. Although some map type have additional interfaces
*
* - By default, AoLibrary uses tsl::ordered_map
*
* -- For more information, go to 'github.com/Tessil/ordered-map'
*
* - Read and look at the defines at include/libconfig.h for more information on the type aliases
*
* @tparam K Key type
* @tparam V Mapped value type
* @tparam H Hash class/function (default: ankerl::unordered_dense::hash<K>)
* @tparam P Key-value pair type (default: InsertOrderMapPair<K,V>)
* @tparam A Allocator type (default: Internal::DefaultAllocator<P>)
*/
template<
	typename K,
	typename V,
	typename H = Internal::DefaultHash<K>,
	typename P = InsertOrderMapPair<K, V>,
	typename A = DefaultAllocator<P>
>
using InsertOrderMap
#if defined(AOL_CONFIG_FLAG_USE_TSL_INSERTORDERED_MAP)
= tsl::ordered_map<K, V, H, std::equal_to<K>, A>;
#else
#error "No custom insert ordered map yet!"
#endif

/**
* @details InsertOrderMap but specialized for pool allocators
*
* - Default pool allocator is backed by mimalloc
*
* - Internally operates on `mi_heap_t`
*
* @tparam K Key type
* @tparam V Mapped value type
* @tparam H Hash class/function (default: ankerl::unordered_dense::hash<K>)
* @tparam P Key-value pair type (default: InsertOrderMapPair<K,V>)
* @tparam A Allocator type (default: Internal::DefaultPoolAllocator<P>)
*/
template<
	typename K,
	typename V,
	typename H = Internal::DefaultHash<K>,
	typename P = InsertOrderMapPair<K, V>,
	typename A = DefaultPoolAllocator<P>
>
using InsertOrderMapPool = InsertOrderMap<K, V, H, P, A>;

}


#endif // !AOL_HEADER_INSERT_ORDERED_MAP_H