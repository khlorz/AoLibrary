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


} // AoL namespace


/*************************************************
* Implementation includes
*************************************************/

#include "containers-subrange-impl.h"
#include "containers-partitions-impl.h"

#endif // AOL_INTERNAL_CONTAINERS_H