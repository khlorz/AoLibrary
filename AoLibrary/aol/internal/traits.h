#pragma once

#include "config.h"

#include <type_traits>
#include <concepts>

namespace AoL
{

//---------------------------------------
// Internal Traits
// - Basically, you do not have to worry about using this
// - In case you do, maybe remove this from Internal namespace
//---------------------------------------
namespace Internal
{

template<typename T>
concept HasContainerTypeTag = requires(T t)
{
	t.internal_type_tag;
};

} // namespace Internal

/**
* AoLibrary traits
* 
* - To differentiate with other functions that can be really confusing, it is put in the Traits namespace
* 
* - This is includes all metaprogramming-related things, especially concepts
*/
namespace Traits
{

/**
* Concept. Checks for 'using Base = <type here>'
* 
* - This is used mostly for CRTP classes
* 
* - Also used for AoL containers since all containers has them
* 
* @tparam C class type
*/
template<typename C>
concept HasBaseAlias = requires
{
	typename C::Base;
};

/**
* Concept. Checks for < operator
* 
* - Used for classes that needs them like KeyOrderedMap
* 
* @tparam T value type
*/
template<typename T>
concept HasLessThanComparator = requires(T t1, T t2)
{
	t1.first < t2.first;
};

/**
* Concept. Checks if the container is a AoLibrary custom container
* 
* @tparam C container type
* @tparam T non-cvref container type
*/
template<typename C, typename T = std::remove_cvref_t<C>>
concept IsRotContainer =
(
	HasBaseAlias<T>						&&
	Internal::HasContainerTypeTag<T>	&&
	std::is_base_of_v<
		typename T::Base,
		T
	>
);

/**
* Concept. Checks if the container is a AoLibrary custom container
* 
* - Basically checks for the essentials functions:
* 
* -- begin(), end(), size(), empty()
*
* @tparam C container type
*/
template<typename C>
concept IsSTLContainer = requires(C c)
{
	c.begin();
	c.end();
	c.cbegin();
	c.cend();
	c.rbegin();
	c.rend();
	c.size();
	c.empty();
};

/**
* Concept. Checks if the passed value is a temporary one.
*
* - Used for functions parameter types
*
* @tparam T value type
*/
template<typename T>
concept IsTemporaryValue = std::is_rvalue_reference_v<T&&>;

/**
* Concept. Checks if the type is cheap to copy
*
* - Used for return types and parameter types
*
* @tparam T value type
*/
template<typename T>
concept IsCheapToCopy = std::copyable<T> && (!std::is_reference_v<T>) && (sizeof(T) <= 2 * sizeof(void*) || std::is_trivially_copyable_v<T>);

/**
* Conditional type. Resulting type will either be a copy type or const-ref type
* 
* - Used for return types and parameter types
* 
* @tparam value type
*/
template<typename T>
using ConstRefOrCopyType =
	std::conditional_t<
		IsCheapToCopy<T>,
		T,
		const T&
	>;

/**
* Compile-time bool. Used for static_assert()
* 
* - This is so static_assert() would not always or automatically assert
* 
* - Useful for if constexpr branch to assert the wrong branch taken
* 
* @tparam ...Ts variadic types. Use this point out where the evaluation went wrong
*/
template<typename... Ts>
inline constexpr bool AssertFalse = false;

}


}