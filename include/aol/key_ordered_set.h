/***************************************************************************************
* AoLibrary key ordered map
****************************************************************************************
* - The library's std::set
***************************************************************************************/
#ifndef AOL_HEADER_KEY_ORDERED_SET_H
#define AOL_HEADER_KEY_ORDERED_SET_H


#include "configs.h"
#include "macros.h"
#include "traits.h"
#include "types.h"
#include "allocators.h"

#if defined(AOL_CONFIG_FLAG_USE_STD_KEYORDERED_SET)
#include <set>
#elif defined(AOL_CONFIG_FLAG_USE_ABSEIL_KEYORDERED_SET)
#include "absl/container/btree_set.h"
#endif

#include <utility>


namespace AoL
{

/**
* @details Key-ordered associative set
*
* - As with other sets, this won't have any duplicate values. Any duplicates will be discarded
*
* - Unlike InsertOrderSet, this will always be sorted by key, so take note.
*
* - For the most part, each map aliases will have the same interfaces. Although some map type have additional interfaces
*
* - By default, AoLibrary uses the btree_map from Google
*
* -- For more information, go to 'code.google.com/archive/p/cpp-btree/'
*
* - Read and look at the defines at include/libconfig.h for more information on the type aliases
*
* @tparam T Element type
* @tparam A Allocator type (default: Internal::DefaultAllocator<T>)
*/
template<
	typename T,
	typename A = DefaultAllocator<T>
>
using KeyOrderSet
#if defined(AOL_USE_STD_ORDERED_SET)
= std::set<T, std::less<T>, A>;
#elif defined(AOL_CONFIG_FLAG_USE_ABSEIL_KEYORDERED_SET)
= absl::btree_set<T, std::less<T>, A>;
#else
#error "No custom key ordered set yet!"
#endif

/**
* @details KeyOrderSet but specialized for pool allocators
*
* - Default pool allocator is backed by mimalloc
*
* - Internally operates on `mi_heap_t`
*
* @tparam T Element type
* @tparam A Allocator type (default: Internal::DefaultPoolAllocator<T>)
*/
template<
	typename T,
	typename A = DefaultPoolAllocator<T>
>
using KeyOrderSetPool = KeyOrderSet<T, A>;

} // AoL namespace


#endif