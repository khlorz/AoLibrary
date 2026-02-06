#pragma once

#include "config.h"

#include "allocators.h"
#include "hashes.h"
#include "types.h"
#include "traits.h"
#include "math.h"
#include "algorithm.h"

#include <array>		// std::array
#include <vector>		// std::vector
#include <iterator>		// iterator/const_iterators
#include <compare>		// <=> operator
#include <cassert>		// assert macro
#include <utility>		// std::pair

/*************************************************
* Hash/Unordered map/set includes
*************************************************/

#ifndef AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP
#include <unordered_map>
#endif
#ifndef AOL_USE_ROBINHOOD_UNORDERED_DENSE_SET
#include <unordered_set>
#endif
#if defined(AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP) || defined(AOL_USE_ROBINHOOD_UNORDERED_DENSE_SET)
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
#ifdef AOL_USE_TSL_INSERTORDERED_MAP
#include "tsl/ordered_map.h"
#endif


/*************************************************
* Key-ordered set includes
*************************************************/

#ifdef AOL_USE_ABSEIL_KEYORDERED_SET
#include "absl/container/btree_set.h"
#else
#include <set>
#endif


/*************************************************
* Insert-ordered Map includes
*************************************************/

#ifdef AOL_USE_TSL_INSERTORDERED_SET
#include "tsl/ordered_set.h"
#endif


namespace AoL
{

/*************************************************
* Forward declarations
*************************************************/

enum class ContainerTypeTag
{
	None,
	KeyOrderMap,
	KeyOrderSet
};


namespace Internal
{

template<
	typename K,
	typename V
>
struct KeyValuePairEx;

template<
	typename K,
	typename V,
	typename P,
	typename A,
	typename C
>
struct KeyOrderMapEx;

}

/*************************************************
* Arrays
*************************************************/

/**
* Static sized container at runtime
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
using Array = std::array<T, S>;

/**
* Array of two elements that can be access through x and y, respectively
* 
* - This is basically a union of an array and two objects of the same types
*
* @tparam T Element type
*/
template<
	typename T
>
union NamedArray2
{
private:
	static constexpr SizeT ArrSize = 2;

public:
	static_assert(std::is_trivial_v<T>, "NamedArray2 requires T to be trivial");
	static_assert(std::is_standard_layout_v<T>, "NamedArray2 requires T to be standard-layout");
	
	T data_arr[ArrSize];
	struct
	{
		T x;
		T y;
	};

	constexpr NamedArray2() noexcept(std::is_nothrow_default_constructible_v<T>)
		: data_arr{} 
	{}

	constexpr NamedArray2(const T& x_, const T& y_) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: data_arr{ x_, y_ }
	{}

	T& operator [] (SizeT idx) noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}

	Traits::ConstRefOrCopyType<T> operator [] (SizeT idx) const noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}
};

/**
* Array of three elements that can be access through x, y, and z, respectively
*
* - This is basically a union of an array and three objects of the same types
*
* @tparam T Element type
*/
template<
	typename T
>
union NamedArray3
{
private:
	static constexpr SizeT ArrSize = 3;

public:
	static_assert(std::is_trivial_v<T>, "NamedArray3 requires T to be trivial");
	static_assert(std::is_standard_layout_v<T>, "NamedArray3 requires T to be standard-layout");

	T data_arr[ArrSize];
	struct
	{
		T x;
		T y;
		T z;
	};

	constexpr NamedArray3() noexcept(std::is_nothrow_default_constructible_v<T>)
		: data_arr{} {
	}

	constexpr NamedArray3(Traits::ConstRefOrCopyType<T> x_, Traits::ConstRefOrCopyType<T> y_, Traits::ConstRefOrCopyType<T> z_) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: data_arr{ x_, y_, z_ } {
	}

	T& operator [] (SizeT idx) noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}

	Traits::ConstRefOrCopyType<T> operator [] (SizeT idx) const noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}
};

/**
* Array of four elements that can be access through x, y, z, and w, respectively
*
* - This is basically a union of an array and four objects of the same types
*
* @tparam T Element type
*/
template<
	typename T
>
union NamedArray4
{
private:
	static constexpr SizeT ArrSize = 4;

public:
	static_assert(std::is_trivial_v<T>, "NamedArray4 requires T to be trivial");
	static_assert(std::is_standard_layout_v<T>, "NamedArray4 requires T to be standard-layout");

	T data_arr[ArrSize];
	struct
	{
		T x;
		T y;
		T z;
		T w;
	};

	constexpr NamedArray4() noexcept(std::is_nothrow_default_constructible_v<T>)
		: data_arr{}
	{}

	constexpr NamedArray4(Traits::ConstRefOrCopyType<T> x_, Traits::ConstRefOrCopyType<T> y_, Traits::ConstRefOrCopyType<T> z_, Traits::ConstRefOrCopyType<T> w_) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: data_arr{ x_, y_, z_, w_ }
	{}

	T& operator [] (SizeT idx) noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}

	Traits::ConstRefOrCopyType<T> operator [] (SizeT idx) const noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}
};


/*************************************************
* Vectors
*************************************************/

/**
* Resizable container at runtime
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
#ifdef AOL_USE_CUSTOM_VECTOR_TYPE
#error "No RottenLibrary custom vector type yet!"
#else
= std::vector<T, A>;
#endif

/**
* Vector but specialized for pool allocation
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


/*************************************************
* Ordered maps
*************************************************/

/**
* Key–value pair type used by KeyOrderMap
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
* Key-ordered associative map
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
* @tparam P Key–value pair type (default: KeyOrderMapPair<K,V>)
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
* KeyOrderMap but specialized for pool allocators
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
* Insert–value pair type used by InsertOrderedMap
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
#error "No designated pair type yet!"
#endif

/**
* Insertion-ordered associative map
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
* @tparam P Key–value pair type (default: InsertOrderMapPair<K,V>)
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
#ifdef AOL_USE_TSL_INSERTORDERED_MAP
= tsl::ordered_map<K, V, H, std::equal_to<K>, A>;
#else
#error "No alternative insert ordered map type yet!"
#endif

/**
* InsertOrderMap but specialized for pool allocators
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
* Insert–value pair type used by InsertOrderedMap
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
#if defined(AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP)
= std::pair<K, V>;
#else
= std::pair<const K, V>;
#endif

/**
* Hash map container
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
#ifdef AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP
= ankerl::unordered_dense::map<K, V, H, std::equal_to<K>, A>;
#else
= std::unordered_map<K, V, ankerl::unordered_dense::hash<K>, std::equal_to<K>, A>;
#endif

/**
* HashMap but specialized for pool allocators
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
* Key-ordered associative set
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
#ifdef AOL_USE_ABSEIL_KEYORDERED_SET
= absl::btree_set<T, std::less<T>, A>;
#else
= std::set<T, std::less<T>, A>;
#endif

/**
* KeyOrderSet but specialized for pool allocators
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
* Insertion-ordered associative set
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
#ifdef AOL_USE_TSL_INSERTORDERED_MAP
= tsl::ordered_set<T, H, std::equal_to<T>, A>;
#else
#error "No alternative insert ordered map type yet!"
#endif

/**
* InsertOrderSet but specialized for pool allocators
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
* Hash map but for sets
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
#ifdef AOL_USE_ROBINHOOD_UNORDERED_DENSE_MAP
= ankerl::unordered_dense::set<T, H, std::equal_to<T>, A>;
#else
= std::unordered_set<T, ankerl::unordered_dense::hash<T>, std::equal_to<T>, A>;
#endif

/**
* HashSet but specialized for pool allocators
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
* Container queries
*************************************************/

/**
* @brief Gets the begin iterator of a container.
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
* @brief Gets the const begin iterator of a container.
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
* @brief Gets the end iterator of a container.
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
* @brief Gets the const end iterator of a container.
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
* @brief Gets the reverse begin iterator of a container.
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
* Gets the const reverse begin iterator of a container.
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
* @brief Gets the reverse end iterator of a container.
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
* Gets the const reverse end iterator of a container.
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
* Returns the size of a container.
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
* Checks whether a container is empty.
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
* Returns the underlying data pointer of a container.
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
* Returns the underlying data pointer of a container
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


/*************************************************
* Internal implementations
*************************************************/

namespace Internal
{

template<ContainerTypeTag CT, typename T>
struct ContainerKeyType
{
	using type = SizeT;
};

template<typename T>
struct ContainerKeyType<ContainerTypeTag::KeyOrderMap, T>
{
	using type = typename T::first_type;
};

template<ContainerTypeTag CT, typename T>
struct ContainerValueType
{
	using type = T;
};

template<typename T>
struct ContainerValueType<ContainerTypeTag::KeyOrderMap, T>
{
	using type = typename T::second_type;
};

template<typename C>
using ContainerAllocatorType = std::conditional_t<Traits::IsSTLContainer<C> || Traits::IsRotContainer<C>, typename C::allocator_type, void>;

template<ContainerTypeTag CT, typename T, typename C>
struct ContainerBase
{
	static constexpr ContainerTypeTag internal_type_tag = CT;

	using container_type = typename C;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;

	ContainerBase() noexcept :
		container_obj{ }
	{
	}

	ContainerBase(SizeT initial_capacity) noexcept :
		container_obj{ }
	{
	}

	ContainerBase(const ContainerAllocatorType<C>& allocator) noexcept :
		container_obj{ allocator }
	{
	}

	ContainerBase(const container_type&& other_data) noexcept :
		container_obj{ other_data }
	{
		std::sort(container_obj.begin(), container_obj.end());
		std::unique(container_obj.begin(), container_obj.end());
	}

	ContainerBase(container_type&& other_data) noexcept :
		container_obj{ other_data }
	{
		std::sort(container_obj.begin(), container_obj.end());
		std::unique(container_obj.begin(), container_obj.end());
	}

	template<typename It> requires std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<It>::iterator_category>
	ContainerBase(It it_start, It it_end) noexcept :
		container_obj{ it_start, it_end }
	{
		std::sort(container_obj.begin(), container_obj.end());
		std::unique(container_obj.begin(), container_obj.end());
	}

	//ContainerBase()

	void clear() noexcept
	{
		container_obj.clear();
	}

	void reserve(SizeT new_capacity) noexcept
	{
		container_obj.reserve(new_capacity);
	}

	constexpr auto begin() noexcept
	{
		return container_obj.begin();
	}

	constexpr auto begin() const noexcept
	{
		return container_obj.cbegin();
	}

	constexpr auto end() noexcept
	{
		return container_obj.end();
	}

	constexpr auto end() const noexcept
	{
		return container_obj.cend();
	}

	constexpr auto cbegin() const noexcept
	{
		return container_obj.cbegin();
	}

	constexpr auto cend() const noexcept
	{
		return container_obj.cend();
	}

	constexpr auto rbegin() noexcept
	{
		return container_obj.rbegin();
	}

	constexpr auto rend() noexcept
	{
		return container_obj.rend();
	}

	constexpr auto rbegin() const noexcept
	{
		return container_obj.rbegin();
	}

	constexpr auto rend() const noexcept
	{
		return container_obj.rend();
	}

	constexpr auto crbegin() const noexcept
	{
		return container_obj.crbegin();
	}

	constexpr auto crend() const noexcept
	{
		return container_obj.crend();
	}

	constexpr auto size() const noexcept
	{
		return container_obj.size();
	}

	constexpr auto empty() const noexcept
	{
		return container_obj.empty();
	}

	constexpr auto data() noexcept
	{
		return container_obj.data();
	}

	constexpr auto data() const noexcept
	{
		return container_obj.data();
	}

protected:
	container_type container_obj;

};


/**
* POD pair of values used for maps and such
* 
* - Main class for key/value pairing
* 
* @tparam K first/key type
* @tparam V second/value type
*/
template<typename K, typename V>
struct KeyValuePairEx
{
	using first_type = K;
	using second_type = V;

	first_type	first;
	second_type	second;

	constexpr auto operator <=> (const KeyValuePairEx& other) const noexcept
	{
		return this->first <=> other.first;
	}
};

/**
* Container: OrderedMap
* 
* - Sorted container
* 
* - Uses vector as storage
* 
* - Can add items that automatically sorts
* 
* - Can add items that does not sort but can be manually sorted after to save time
* 
* @tparam K key type
* @tparam V value type
* @tparam P pair type
* @tparam A allocator type
* @tparam C map container type
*/
template<typename K, typename V, typename P, typename A, typename C>
struct KeyOrderMapEx : public Internal::ContainerBase<ContainerTypeTag::KeyOrderMap, P, C>
{
public:
	using Base = typename Internal::ContainerBase<ContainerTypeTag::KeyOrderMap, P, C>;

private:
	using Base::container_obj;

public:
	using Base::internal_type_tag;

	using key_type = typename ContainerKeyType<internal_type_tag, P>::type;
	using mapped_type = typename ContainerValueType<internal_type_tag, P>::type;
	using value_type = typename P;

#ifndef NDEBUG
	bool build_flag;
#endif

	KeyOrderMapEx() noexcept :
		Base{ }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
	}

	KeyOrderMapEx(SizeT initial_capacity) noexcept :
		Base{ initial_capacity }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
	}

	KeyOrderMapEx(const A& allocator) noexcept :
		Base{ allocator }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
	}

	KeyOrderMapEx(const Base::container_type& other_data) noexcept :
		Base{ other_data }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
		Sort(container_obj);
	}

	KeyOrderMapEx(Base::container_type&& other_data) noexcept :
		Base{ other_data }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
		Sort(container_obj);
	}

	template<typename It> requires std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<It>::iterator_category>
	KeyOrderMapEx(It it_start, It it_end) noexcept :
		Base{ it_start, it_end }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
		Sort(container_obj);
	}

	constexpr void build_start() noexcept
	{
		assert(!build_flag && "Already building! Call build_end() first!");
#ifndef NDEBUG
		build_flag = true;
#endif
	}

	template<typename InKey, typename InValue>
	constexpr void build_add(InKey&& key, InValue&& value) noexcept requires std::is_convertible_v<InKey, key_type> && std::is_convertible_v<InValue, mapped_type>
	{
		assert(build_flag && "Building haven't started yet! Call build_start() first!");
#ifndef NDEBUG
		const InKey& ref_key = key;
		auto it = std::find(container_obj.begin(), container_obj.end(), ref_key);
		assert(it == container_obj.end() && "Key already exists!");
#endif
		container_obj.emplace_back(std::forward<InKey>(key), std::forward<InValue>(value));
	}

	constexpr void build_end() noexcept
	{
		assert(build_flag && "Building haven't started yet! Call build_start() first!");
#ifndef NDEBUG
		build_flag = false;
#endif
		Sort(container_obj);
	}

	template<typename InKey, typename InValue>
	constexpr void insert(InKey&& key, InValue&& value) noexcept requires std::is_convertible_v<InKey, key_type> && std::is_convertible_v<InValue, mapped_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		const value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
		if (p_ret >= container_obj.data() + container_obj.size())
		{
			container_obj.emplace_back(std::forward<InKey>(key), std::forward<InValue>(value));
		}
		else
		{
			assert(p_ret->first != key_val && "Item already exists!");
			container_obj.insert(container_obj.begin() + (p_ret - container_obj.data()), value_type{std::forward<InKey>(key), std::forward<InValue>(value)});
		}
	}

	template<typename InKey>
	constexpr mapped_type& operator[](InKey&& key) noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
#ifndef NDEBUG
		value_type* p_ret = this->find(std::forward<InKey>(key));
		assert(p_ret != nullptr && "Invalid key!");
		return p_ret->second;
#else
		return LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), std::forward<InKey>(key))->second;
#endif // !NDEBUG
	}

	template<typename InKey, typename R = Traits::ConstRefOrCopyType<mapped_type>>
	constexpr R operator[](InKey&& key) const noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
#ifndef NDEBUG
		const value_type* p_ret = this->find(std::forward<InKey>(key));
		assert(p_ret != nullptr && "Invalid key!");
		return p_ret->second;
#else
		return LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), std::forward<InKey>(key))->second;
#endif // !NDEBUG
	}

	template<typename InKey>
	mapped_type& at_ref(InKey&& key) noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const auto& key_val = std::forward<InKey>(key);
		value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
		if (p_ret < container_obj.data() + container_obj.size() && p_ret->first == key_val)
		{
			return p_ret->second;
		}
		else
		{
			auto it = container_obj.insert(container_obj.begin() + (p_ret - container_obj.data()), value_type{std::forward<InKey>(key), mapped_type{}});
			return it->second;
		}
	}

	template<typename InKey, typename R = Traits::ConstRefOrCopyType<mapped_type>>
	R at_ref(InKey&& key) const noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const auto& key_val = std::forward<InKey>(key);
		const value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
		if (p_ret < container_obj.data() + container_obj.size() && p_ret->first == key_val)
		{
			return p_ret->second;
		}
		else
		{
			auto it = container_obj.insert(container_obj.begin() + (p_ret - container_obj.data()), value_type{ std::forward<InKey>(key), mapped_type{} });
			return it->second;
		}
	}

	template<typename InKey>
	mapped_type* at_ptr(InKey&& key) noexcept requires std::is_convertible_v<InKey, key_type>
	{
		value_type* p_ret = this->find(std::forward<InKey>(key));
		return p_ret != nullptr ? &p_ret->second : nullptr;
	}

	template<typename InKey>
	const mapped_type* at_ptr(InKey&& key) const noexcept requires std::is_convertible_v<InKey, key_type>
	{
		const value_type* p_ret = this->find(std::forward<InKey>(key));
		return p_ret != nullptr ? &p_ret->second : nullptr;
	}

	template<typename InKey>
	constexpr value_type* find(InKey&& key) noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const auto& key_val = std::forward<InKey>(key);
		value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
		if (p_ret < container_obj.data() + container_obj.size() && p_ret->first == key_val)
		{
			return p_ret;
		}
		else
		{
			return nullptr;
		}
	}

	template<typename InKey>
	constexpr const value_type* find(InKey&& key) const noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const auto& key_val = std::forward<InKey>(key);
		const value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
		if (p_ret < container_obj.data() + container_obj.size() && p_ret->first == key_val)
		{
			return p_ret;
		}
		else
		{
			return nullptr;
		}
	}

	template<typename InKey>
	constexpr bool contains(InKey&& key) const noexcept requires std::is_convertible_v<InKey, key_type>
	{
		return this->find(std::forward<InKey>(key)) != nullptr;
	}

};

} // namespace Internal

} // namespace AoL