/***************************************************************************************
* Algorithm Sort Implementations
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_ALGORITHMS_SORT_H
#define AOL_HEADER_INTERNAL_ALGORITHMS_SORT_H


#include "aol/configs.h"
#include "aol/traits.h"
#include "aol/types.h"

#include <algorithm>	// std::sort
#include <utility>		// std::forward
#include <iterator>		// std::make_reverse_iterator

namespace AoL
{

// Sort the whole container
// - use std::sort for custom size container
template<typename It>
constexpr void Sort(It it_begin, It it_end) noexcept
{
	std::sort(it_begin, it_end);
}

// Sort the whole container with comparator
// - use std::sort for custom size container
template<typename It, typename F>
constexpr void Sort(It it_begin, It it_end, F&& f) noexcept requires (!std::is_execution_policy_v<F>)
{
	std::sort(it_begin, it_end, std::forward<F>(f));
}

// Sort the whole container with custom execution
// - use std::sort for custom size container
template<typename It, typename E>
constexpr void Sort(E&& e, It it_begin, It it_end) noexcept requires std::is_execution_policy_v<E>
{
	std::sort(std::forward<E>(e), it_begin, it_end);
}

// Sort the whole container with custom execution and comparator
// - use std::sort for custom size container
template<typename It, typename F, typename E>
constexpr void Sort(E&& e, It it_begin, It it_end, F&& f) noexcept requires (!std::is_execution_policy_v<F>) && std::is_execution_policy_v<E>
{
	std::sort(std::forward<E>(e), it_begin, it_end, std::forward<F>(f));
}

// Reverse sort the whole container
// - use std::sort for custom size container
template<typename It>
constexpr void SortReverse(It it_begin, It it_end) noexcept
{
	std::sort(std::make_reverse_iterator(it_end), std::make_reverse_iterator(it_begin));
}

// Reverse sort the whole container with comparator
// - use std::sort for custom size container
template<typename It, typename F>
constexpr void SortReverse(It it_begin, It it_end, F&& f) noexcept requires (!std::is_execution_policy_v<F>)
{
	std::sort(std::make_reverse_iterator(it_end), std::make_reverse_iterator(it_begin), std::forward<F>(f));
}

// Reverse sort the whole container with custom execution
// - use std::sort for custom size container
template<typename It, typename E>
constexpr void SortReverse(E&& e, It it_begin, It it_end) noexcept requires std::is_execution_policy_v<E>
{
	std::sort(std::forward<E>(e), std::make_reverse_iterator(it_end), std::make_reverse_iterator(it_begin));
}

// Reverse sort the whole container with custom execution and comparator
// - use std::sort for custom size container
template<typename It, typename F, typename E>
constexpr void SortReverse(E&& e, It it_begin, It it_end, F&& f) noexcept requires (!std::is_execution_policy_v<F>) && std::is_execution_policy_v<E>
{
	std::sort(std::forward<E>(e), std::make_reverse_iterator(it_end), std::make_reverse_iterator(it_begin), std::forward<F>(f));
}

} // AoL namespace


#endif // AOL_HEADER_INTERNAL_ALGORITHMS_SORT_H