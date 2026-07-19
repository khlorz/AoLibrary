/***************************************************************************************
* AoLibrary key ordered map
****************************************************************************************
* - The library's std::map
***************************************************************************************/
#ifndef AOL_HEADER_KEY_ORDERED_MAP_H
#define AOL_HEADER_KEY_ORDERED_MAP_H

#include "configs.h"
#include "allocators.h"

#if defined(AOL_CONFIG_FLAG_USE_STD_KEYORDERED_MAP)
#include <map>
#elif defined(AOL_CONFIG_FLAG_USE_BOOST_KEYORDERED_MAP)
#include "boost/container/map.hpp"
#elif defined(AOL_CONFIG_FLAG_USE_ABSEIL_KEYORDERED_MAP)
#include "absl/container/btree_map.h"
#endif
#include "internal/containers-ordered-map-impl.h"

#include <utility>

namespace AoL
{

/**
* @details Key-value pair type used by KeyOrderMap
*
* - This will be the main pair type used for KeyOrderMap type
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
using KeyOrderMapPair
#if defined(AOL_CONFIG_FLAG_USE_STD_KEYORDERED_MAP) || defined(AOL_CONFIG_FLAG_USE_ABSEIL_KEYORDERED_MAP)
= std::pair<const K, V>;
#elif defined(AOL_CONFIG_FLAG_USE_BOOST_KEYORDERED_MAP)
= std::pair<K, V>;
#else
= Internal::KeyValuePairEx<K, V>;
#endif

/**
* @details Key-ordered associative map
*
* - Compared to InsertOrderMap, this map is sorted by key.
*
* - For the most part, each map aliases will have the same interfaces. Although some map type have additional interfaces.
*
* - By default, AoLibrary will be using absl::btree_map
*
* -- For more information, go to 'github.com/abseil/abseil-cpp'
*
* - Read and look at the defines at include/libconfig.h for more information on the type aliases
*
* @tparam K Key type
* @tparam V Mapped value type
* @tparam P Key-value pair type (default: KeyOrderMapPair<K,V>)
* @tparam A Allocator type (default: Internal::DefaultAllocator<P>)
*/
template<
	typename K,
	typename V,
	typename P = KeyOrderMapPair<K, V>,
	typename A = DefaultAllocator<P>
>
using KeyOrderMap
#if defined(AOL_CONFIG_FLAG_USE_STD_KEYORDERED_MAP)
= std::map<K, V>;
#elif defined(AOL_CONFIG_FLAG_USE_BOOST_KEYORDERED_MAP)
= boost::container::map<K, V, std::less<K>, A>;
#elif defined(AOL_CONFIG_FLAG_USE_ABSEIL_KEYORDERED_MAP)
= absl::btree_map<K, V, std::less<K>, A>;
#else
= Internal::KeyOrderMapEx<K, V, P, Internal::PairLessComparator<P>, A>;
#endif

/**
* @details KeyOrderMap but specialized for pool allocators
*
* - Default pool allocator is backed by mimalloc
*
* - Internally operates on `mi_heap_t`
*
* @tparam K Key type
* @tparam V Mapped value type
* @tparam P Key-value pair type (default: KeyOrderMapPair<K,V>)
* @tparam A Allocator type (default: Internal::DefaultPoolAllocator<P>)
*/
template<
	typename K,
	typename V,
	typename P = KeyOrderMapPair<K, V>,
	typename A = DefaultPoolAllocator<P>
>
using KeyOrderMapPool = KeyOrderMap<K, V, P, A>;

/**
* @details Key-value pair type used by FlatKeyOrderMap
*
* - This will be the main pair type used for FlatKeyOrderMap type
*
* - Each aliases will have the same, more or less, interfaces so they are interchangeable depending on the map used
*
* - Read and look at the defines at include/config.h for more information on the type aliases
*
* @tparam K Key type
* @tparam V Mapped value type
*/
template<
	typename K,
	typename V
>
using FlatKeyOrderMapPair = Internal::KeyValuePairEx<K, V>;

/**
* @details flat Key-ordered associative map
*
* - Compared to InsertOrderMap, this map is sorted by key.
*
* - Internally uses a vector
*
* - For fast operations, use the build functions: build_start -> build_add... -> build_end
*
* - Used for "Add elements then sort after"
*
* - Optimized for lookups more than insertion
*
* @tparam K Key type
* @tparam V Mapped value type
* @tparam P Key-value pair type (default: KeyOrderMapPair<K,V>)
* @tparam A Allocator type (default: Internal::DefaultAllocator<P>)
*/
template<
	typename K,
	typename V,
	typename P = FlatKeyOrderMapPair<K, V>,
	typename A = DefaultAllocator<P>
>
using FlatKeyOrderMap = Internal::KeyOrderMapEx<K, V, P, Internal::PairLessComparator<P>, A>;

/**
* @details FlatKeyOrderMap but specialized for pool allocators
*
* - Default pool allocator is backed by mimalloc
*
* - Internally operates on `mi_heap_t`
*
* @tparam K Key type
* @tparam V Mapped value type
* @tparam P Key-value pair type (default: KeyOrderMapPair<K,V>)
* @tparam A Allocator type (default: Internal::DefaultPoolAllocator<P>)
*/
template<
	typename K,
	typename V,
	typename P = Internal::KeyValuePairEx<K, V>,
	typename A = DefaultPoolAllocator<P>
>
using FlatKeyOrderMapPool = Internal::KeyOrderMapEx<K, V, P, Internal::PairLessComparator<P>, A>;

} // AoL namespace


#endif // AOL_HEADER_KEY_ORDERED_MAP_H