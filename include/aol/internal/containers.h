#pragma once

/***************************************************************************************
* AoLibrary containers
****************************************************************************************
* - The library's main container types
* - It is better to use these if you include the library for better ease of use
* - Some custom containers and functions are provided as well
* - Additional containers/functions will be added in the future as the library grows
***************************************************************************************/

#include "config.h"

#include "macros.h"
#include "allocators.h"
#include "hashes.h"
#include "types.h"
#include "traits.h"
#include "math.h"
#include "algorithm.h"

#include <iterator>		// iterator/const_iterators
#include <compare>		// <=> operator
#include <cassert>		// assert macro
#include <utility>		// std::pair
#include <ranges>		// std::views

/*************************************************
* Array includes
*************************************************/

#if defined(AOL_USE_STD_ARRAY)
#include <array>
#endif


/*************************************************
* Vector includes
*************************************************/

#if defined(AOL_USE_STD_VECTOR)
#include <vector>
#endif


/*************************************************
* Hash/Unordered map/set includes
*************************************************/

#if defined(AOL_USE_STD_UNORDERED_MAP)
#include <unordered_map>
#elif defined(AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP)
#include "unordered_dense/unordered_dense.h"
#endif

#if defined(AOL_USE_STD_UNORDERED_SET)
#include <unordered_set>
#elif defined(AOL_USE_ROBINHOOD_UNORDERED_DENSE_SET)
#include "unordered_dense/unordered_dense.h"
#endif


/*************************************************
* Key-ordered map includes
*************************************************/

#if defined(AOL_USE_STD_KEYORDERED_MAP)
#include <map>
#elif defined(AOL_USE_BOOST_KEYORDERED_MAP)
#include "boost/container/map.hpp"
#elif defined(AOL_USE_ABSEIL_KEYORDERED_MAP)
#include "absl/container/btree_map.h"
#endif


/*************************************************
* Insert-ordered map includes
*************************************************/
#if defined(AOL_USE_TSL_INSERTORDERED_MAP)
#include "tsl/ordered_map.h"
#endif


/*************************************************
* Key-ordered set includes
*************************************************/

#if defined(AOL_USE_STD_KEYORDERED_SET)
#include <set>
#elif defined(AOL_USE_ABSEIL_KEYORDERED_SET)
#include "absl/container/btree_set.h"
#endif


/*************************************************
* Insert-ordered Map includes
*************************************************/

#if defined(AOL_USE_TSL_INSERTORDERED_SET)
#include "tsl/ordered_set.h"
#endif


namespace AoL
{

/*************************************************
* Forward declarations
*************************************************/

namespace Internal
{

struct AOL_EMPTY_BASE_OPTIMIZATION ContainerTag {};
struct AOL_EMPTY_BASE_OPTIMIZATION SubPartitionTag {};
struct AOL_EMPTY_BASE_OPTIMIZATION MainPartitionTag {};
struct AOL_EMPTY_BASE_OPTIMIZATION SubPartitionViewTag {};

template<
	typename T,
	typename A
>
struct VectorPartitionEx;

template<
	typename T,
	SizeT S
>
struct CyclicBufferFixed;

template<
	typename T,
	typename A
>
struct CyclicBufferDynamic;

template<
	typename K,
	typename V
>
struct KeyValuePairEx;

template<
	typename K,
	typename V,
	typename P,
	typename A
>
struct KeyOrderMapEx;

template<
	typename It
>
struct SubrangeEx;

}

/*************************************************
* Arrays
*************************************************/

/**
* @details Static sized container at runtime
*
* - Compile-time size
*
* - Statically fixed (no resizes)
*
* - Use this instead of a C array for safer usage
*
* @tparam T Element type
* @tparam S Array size
*/
template<
	typename T,
	SizeT S
>
using Array
#if defined(AOL_USE_STD_ARRAY)
= std::array<T, S>;
#else
#error "No custom array yet!"
#endif

template<
	typename T
>
struct ArrayNamed2;

template<
	typename T
>
struct ArrayNamed3;

template<
	typename T
>
struct ArrayNamed4;


/*************************************************
* Vectors
*************************************************/

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
	typename A = Internal::DefaultAllocator<T>
>
using Vector
#if defined(AOL_USE_STD_VECTOR)
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
	typename A = Internal::DefaultPoolAllocator<T>
>
using VectorPool = Vector<T, A>;

/**/
template<
	typename T,
	typename A = Internal::DefaultAllocator<T>
>
using VectorPartition = Internal::VectorPartitionEx<T, A>;


/*************************************************
* Ordered maps
*************************************************/

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
#if defined(AOL_USE_STD_KEYORDERED_MAP) || defined(AOL_USE_ABSEIL_KEYORDERED_MAP)
= std::pair<const K, V>;
#elif defined(AOL_USE_BOOST_KEYORDERED_MAP)
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
	typename A = Internal::DefaultAllocator<P>
>
using KeyOrderMap
#if defined(AOL_USE_STD_KEYORDERED_MAP)
= std::map<K, V>;
#elif defined(AOL_USE_BOOST_KEYORDERED_MAP)
= boost::container::map<K, V, std::less<K>, A>;
#elif defined(AOL_USE_ABSEIL_KEYORDERED_MAP)
= absl::btree_map<K, V, std::less<K>, A>;
#else
= Internal::KeyOrderMapEx<K, V, P, A, Vector<P>>;
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
	typename A = Internal::DefaultPoolAllocator<P>
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
	typename A = Internal::DefaultAllocator<P>
>
using FlatKeyOrderMap = Internal::KeyOrderMapEx<K, V, P, A>;

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
	typename A = Internal::DefaultPoolAllocator<P>
>
using FlatKeyOrderMapPool = Internal::KeyOrderMapEx<K, V, P, A>;

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
#if defined(AOL_USE_TSL_INSERTORDERED_MAP)
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
	typename A = Internal::DefaultAllocator<P>
>
using InsertOrderMap
#if defined(AOL_USE_TSL_INSERTORDERED_MAP)
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
	typename A = Internal::DefaultPoolAllocator<P>
>
using InsertOrderMapPool = InsertOrderMap<K, V, H, P, A>;


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
#if defined(AOL_USE_STD_UNORDERED_MAP)
= std::pair<const K, V>;
#elif defined(AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP)
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
	typename A = Internal::DefaultAllocator<P>
>
using HashMap
#if defined(AOL_USE_STD_UNORDERED_MAP)
= std::unordered_map<K, V, ankerl::unordered_dense::hash<K>, std::equal_to<K>, A>;
#elif defined(AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP)
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
	typename A = Internal::DefaultPoolAllocator<P>
>
using HashMapPool = HashMap<K, V, H, P, A>;


/*************************************************
* Ordered sets
*************************************************/

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
	typename A = Internal::DefaultAllocator<T>
>
using KeyOrderSet
#if defined(AOL_USE_STD_ORDERED_SET)
= std::set<T, std::less<T>, A>;
#elif defined(AOL_USE_ABSEIL_KEYORDERED_SET)
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
	typename A = Internal::DefaultPoolAllocator<T>
>
using KeyOrderSetPool = KeyOrderSet<T, A>;

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
	typename A = Internal::DefaultAllocator<T>
>
using InsertOrderSet
#if defined(AOL_USE_TSL_INSERTORDERED_MAP)
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
	typename A = Internal::DefaultPoolAllocator<T>
>
using InsertOrderSetPool = InsertOrderSet<T, H, A>;


/*************************************************
* Hash sets
*************************************************/

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
	typename A = Internal::DefaultAllocator<T>
>
using HashSet
#if defined(AOL_USE_STD_UNORDERED_MAP)
= std::unordered_set<T, ankerl::unordered_dense::hash<T>, std::equal_to<T>, A>;
#elif defined(AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP)
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
	typename A = Internal::DefaultPoolAllocator<T>
>
using HashSetPool = HashSet<T, H, A>;


/*************************************************
* Cyclic buffers
*************************************************/


/**
* @details Fixed size cyclic buffer
* 
* - Cyclic buffer is a FIFO container that overwrite the oldest element when
*   hitting the max number of elements it can contain
* 
* - This uses an array so this will immediately construct empty elements
* 
* - Only accepts power of two value for size (e.g. 2, 4, 8, 16, 32, etc)
* 
* @tparam T element type
* @tparam S max item count/size
*/
template<
	typename T,
	SizeT S
>
using CyclicBufferF = Internal::CyclicBufferFixed<T, S>;

/**
* @details Dynamic size cyclic buffer
*
* - Cyclic buffer is a FIFO container that overwrite the oldest element when
*   hitting the max number of elements it can contain
*
* - This uses a vector and will only allocate when needed until it reaches the max
*   number of elements it can contain
*
* - Only accepts power of two value for size (e.g. 2, 4, 8, 16, 32, etc)
* 
* @tparam T element type
* @tparam A allocator type
*/
template<
	typename T,
	typename A = Internal::DefaultAllocator<T>
>
using CyclicBufferD = Internal::CyclicBufferDynamic<T, A>;


/*************************************************
* Subrange
*************************************************/

#if defined(AOL_USE_STD_SUBRANGE)
template<
	typename It,
	typename Sentinel = It,
	std::ranges::subrange_kind Kind = std::sized_sentinel_for<Se, It> ? std::ranges::subrange_kind::sized : std::ranges::subrange_kind::unsized
>
using Subrange = std::ranges::subrange<It, Sentinel, Kind>;
#else
template<
	typename It
>
using Subrange = Internal::SubrangeEx<It>;
#endif

/*************************************************
* Container queries
*************************************************/

/**
* @details  Gets the begin iterator of a container.
*
* - Usable on STL-compatible containers.
*
* @tparam C Container type
* @param c Container instance
* @return Iterator to the beginning of the container
*/
template<typename C>
constexpr auto GetBeginIt(C& c) noexcept
{
	using std::begin;
	return begin(c);
}

/**
* @details  Gets the const begin iterator of a container.
*
* - Usable on STL-compatible containers.
*
* @tparam C Container type
* @param c Container instance
* @return Const iterator to the beginning of the container
*/
template<typename C>
constexpr auto GetBeginIt(const C& c) noexcept
{
	using std::cbegin;
	return cbegin(c);
}

/**
* @details  Gets the end iterator of a container.
*
* - Usable on STL-compatible containers.
*
* @tparam C Container type
* @param c Container instance
* @return Iterator to the end of the container
*/
template<typename C>
constexpr auto GetEndIt(C& c) noexcept
{
	using std::end;
	return end(c);
}

/**
* @details  Gets the const end iterator of a container.
*
* - Usable on STL-compatible containers.
*
* @tparam C Container type
* @param c Container instance
* @return Const iterator to the end of the container
*/
template<typename C>
constexpr auto GetEndIt(const C& c) noexcept
{
	using std::cend;
	return cend(c);
}

/**
* @details  Gets the reverse begin iterator of a container.
*
* - Usable on STL-compatible containers.
*
* @tparam C Container type
* @param c Container instance
* @return Reverse iterator to the beginning of the reversed container
*/
template<typename C>
constexpr auto GetBeginReverseIt(C& c) noexcept
{
	using std::rbegin;
	return rbegin(c);
}

/**
* @details Gets the const reverse begin iterator of a container.
*
* - Usable on STL-compatible containers.
*
* @tparam C Container type
* @param c Container instance
* @return Const reverse iterator to the beginning of the reversed container
*/
template<typename C>
constexpr auto GetBeginReverseIt(const C& c) noexcept
{
	using std::crbegin;
	return crbegin(c);
}

/**
* @details  Gets the reverse end iterator of a container.
*
* - Usable on STL-compatible containers.
*
* @tparam C Container type
* @param c Container instance
* @return Reverse iterator to the end of the reversed container
*/
template<typename C>
constexpr auto GetEndReverseIt(C& c) noexcept
{
	using std::rend;
	return rend(c);
}

/**
* @details Gets the const reverse end iterator of a container.
*
* - Usable on STL-compatible containers.
*
* @tparam C Container type
* @param c Container instance
* @return Const reverse iterator to the end of the reversed container
*/
template<typename C>
constexpr auto GetEndReverseIt(const C& c) noexcept
{
	using std::crend;
	return crend(c);
}

/**
* @details Returns the size of a container.
*
* - Usually returns `size_t`.
*
* @tparam C Container type
* @param c Container instance
* @return Number of elements in the container
*/
template<typename C>
constexpr auto GetContainerSize(const C& c) noexcept
{
	using std::size;
	return size(c);
}

/**
* @details Checks whether a container is empty.
*
* @tparam C Container type
* @param c Container instance
* @return true if the container is empty, false otherwise
*/
template<typename C>
constexpr auto IsContainerEmpty(const C& c) noexcept
{
	using std::empty;
	return empty(c);
}

/**
* @details Returns the underlying data pointer of a container.
*
* - Equivalent to `data()`. Applicable to both AoL containers and STL containers.
*
* @tparam C Container type
* @param c Container instance
* @return Pointer to the container's underlying data
*/
template<typename C>
constexpr auto GetContainerData(const C& c) noexcept
{
	return c.data();
}

/**
* @details Returns the underlying data pointer of a container
*
* - Equivalent to `data()`. Applicable to both AoL containers and STL containers.
*
* @tparam C Container type
* @param c Container instance
* @return Pointer to the container's underlying data
*/
template<typename C>
constexpr auto GetContainerData(C& c) noexcept
{
	return c.data();
}

namespace Traits
{

/**
* Concept. Checks if the container is a AoLibrary custom container
* 
* @tparam C container type
* @tparam T non-cvref container type
*/
template<typename C, typename T = std::remove_cvref_t<C>>
concept IsAoLContainer = std::is_same_v<typename T::container_tag, Internal::ContainerTag>;

} // Traits namespace

} // AoL namespace


/*************************************************
* Implementation includes
*************************************************/

#include "containers-base-impl.h"
#include "containers-array-impl.h"
#include "containers-vector-impl.h"
#include "containers-ordered-map-impl.h"
#include "containers-cyclic-buffer-impl.h"
#include "containers-subrange-impl.h"
#include "containers-partitions-impl.h"

// containers.h EOF