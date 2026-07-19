/***************************************************************************************
* AoLibrary containers
****************************************************************************************
* - The library's main container types
* - It is better to use these if you include the library for better ease of use
* - Some custom containers and functions are provided as well
* - Additional containers/functions will be added in the future as the library grows
***************************************************************************************/
#ifndef AOL_INTERNAL_CONTAINERS_H
#define AOL_INTERNAL_CONTAINERS_H

#include "configs.h"

#include "macros.h"
#include "aol/allocators.h"
#include "hashes.h"
#include "types.h"
#include "traits.h"
#include "math.h"
#include "aol/algorithms.h"

#include <iterator>		// iterator/const_iterators
#include <compare>		// <=> operator
#include <cassert>		// assert macro
#include <utility>		// std::pair
#include <ranges>		// std::views


namespace AoL
{

/*************************************************
* Forward declarations
*************************************************/

namespace Internal
{

/****************************
* Type tags
****************************/

struct AOL_EMPTY_BASE_OPTIMIZATION ContainerTag {};

struct AOL_EMPTY_BASE_OPTIMIZATION PartitionTag_Contiguous {};
struct AOL_EMPTY_BASE_OPTIMIZATION PartitionTag_Block {};


/****************************
* Partition
****************************/

template<
	typename C
>
struct SubPartitionEx;

template<
	typename T,
	typename A
>
struct PartitionVectorEx;

template<
	typename T,
	AoL::SizeT S
>
struct PartitionArrayEx;

/****************************
* Cyclic Buffers
****************************/

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


/****************************
* Subranges
****************************/

template<
	typename It
>
struct SubrangeEx;

}


/*************************************************
* Hash sets
*************************************************/




/*************************************************
* Subrange
*************************************************/




/*************************************************
* Partitions
*************************************************/

/*
* @details SubPartition of a container.
* 
* Constructible only from a partition. Can be copied or moved but not constructed.
*/
template<typename P>
using SubPartition = Internal::SubPartitionEx<P>;

/*
* @details Partition using AoL::Vector
* 
* As a vector, the storage is contiguous.
* 
* @tparam T value type
* @tparam A allocator type (default: Internal::DefaultAllocator<T>)
*/
template<
	typename T,
	typename A = DefaultAllocator<T>
>
using PartitionVector = Internal::PartitionVectorEx<T, A>;

/*
* @details Partition using AoL::AoL
*
* As an array, the storage is contiguous.
*
* @tparam T value type
* @tparam S array size
*/
template<
	typename T,
	AoL::SizeT S
>
using PartitionArray = Internal::PartitionArrayEx<T, S>;

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

#include "containers-subrange-impl.h"
#include "containers-partitions-impl.h"

#endif // AOL_INTERNAL_CONTAINERS_H