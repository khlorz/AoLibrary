/***************************************************************************************
* AoLibrary key ordered map
****************************************************************************************
* - The library's std::unordered_map
***************************************************************************************/
#ifndef AOL_HEADER_UNORDERED_MAP_H
#define AOL_HEADER_UNORDERED_MAP_H


#include "configs.h"
#include "macros.h"
#include "traits.h"
#include "types.h"
#include "hashes.h"
#include "allocators.h"

#if defined(AOL_CONFIG_FLAG_USE_STD_UNORDERED_MAP)
#include <unordered_map>
#elif defined(AOL_CONFIG_FLAG_USE_ANKERL_UNORDERED_DENSE_MAP)
#include "unordered_dense/unordered_dense.h"
#endif

#include <utility>


namespace AoL
{

/*************************************************
* Hash maps
*************************************************/

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
using HashMapPair
#if defined(AOL_CONFIG_FLAG_USE_STD_UNORDERED_MAP)
= std::pair<const K, V>;
#elif defined(AOL_CONFIG_FLAG_USE_ANKERL_UNORDERED_DENSE_MAP)
= std::pair<K, V>;
#else
#error "No custom hash map pair yet!"
#endif

/**
* @details Hash map container
*
* - This container is not sorted/ordered but is associated with hashes of the key type
*
* - Fast look up and fast insertion. Better suited for types where lookups are a lot.
*
* - By default, AoLibrary uses ankerl::unordered_dense::map
*
* -- For more information, go to 'github.com/martinus/unordered_dense'
*
* - Read and look at the defines at include/libconfig.h for more information on the type aliases.
*
* @tparam K Key type
* @tparam V Mapped value type
* @tparam H Hash class/function (default: ankerl::unordered_dense::hash<K>)
* @tparam P Key-value pair type (default: HashMapPair<K,V>)
* @tparam A Allocator type (default: Internal::DefaultPoolAllocator<P>)
*/
template<
	typename K,
	typename V,
	typename H = Internal::DefaultHash<K>,
	typename P = HashMapPair<K, V>,
	typename A = DefaultAllocator<P>
>
using HashMap
#if defined(AOL_CONFIG_FLAG_USE_STD_UNORDERED_MAP)
= std::unordered_map<K, V, ankerl::unordered_dense::hash<K>, std::equal_to<K>, A>;
#elif defined(AOL_CONFIG_FLAG_USE_ANKERL_UNORDERED_DENSE_MAP)
= ankerl::unordered_dense::map<K, V, H, std::equal_to<K>, A>;
#else
#error "No custom hash map yet!"
#endif

/**
* @details HashMap but specialized for pool allocators
*
* - Default pool allocator is backed by mimalloc
*
* - Internally operates on `mi_heap_t`
*
* @tparam K Key type
* @tparam V Mapped value type
* @tparam H Hash class/function (default: ankerl::unordered_dense::hash<K>)
* @tparam P Key-value pair type (default: HashMapPair<K,V>)
* @tparam A Allocator type (default: Internal::DefaultPoolAllocator<P>)
*/
template<
	typename K,
	typename V,
	typename H = Internal::DefaultHash<K>,
	typename P = HashMapPair<K, V>,
	typename A = DefaultPoolAllocator<P>
>
using HashMapPool = HashMap<K, V, H, P, A>;

}


#endif // AOL_HEADER_UNORDERED_MAP_H