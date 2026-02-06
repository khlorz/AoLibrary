#pragma once

#include "config.h"

#include "types.h"

#include <algorithm>

namespace AoL
{

namespace Internal
{

template<typename T>
concept IsPairType = requires(T t)
{
    t.first;
    t.second;
};

template<typename K>
concept HasComparableKey = requires(K k1, K k2)
{
    { k1 < k2 } -> std::convertible_to<bool>;
};

template<typename T, typename K>
concept IsValidLowerBoundType = IsPairType<T> && HasComparableKey<K>;

}

template<typename C, typename K> requires Internal::IsValidLowerBoundType<C, K>
C* LowerBound(const C* p_start, const C* p_end, const K& key)
{
    C* p_current = const_cast<C*>(p_start);
    for (PtrSize current_count = (PtrSize)(p_end - p_current); current_count > 0; )
    {
        PtrSize temp_count = current_count >> 1;
        C* mid = p_current + temp_count;
        if (mid->first < key)
        {
            p_current = ++mid;
            current_count -= temp_count + 1;
        }
        else
        {
            current_count = temp_count;
        }
    }
    return p_current;
}

// Sort the whole container
// - use std::sort for custom size container
template<typename C>
constexpr void Sort(C& c) noexcept
{
	std::sort(GetBeginIt(c), GetEndIt(c));
}

// Sort the whole container with comparator
// - use std::sort for custom size container
template<typename C, typename F>
constexpr void Sort(C& c, F&& f) noexcept requires (!std::is_execution_policy_v<F>)
{
	std::sort(GetBeginIt(c), GetEndIt(c), std::forward<F>(f));
}

// Sort the whole container with custom execution
// - use std::sort for custom size container
template<typename C, typename E>
constexpr void Sort(E&& e, C& c) noexcept requires std::is_execution_policy_v<E>
{
	std::sort(std::forward<E>(e), GetBeginIt(c), GetEndIt(c));
}

// Sort the whole container with custom execution and comparator
// - use std::sort for custom size container
template<typename C, typename F, typename E>
constexpr void Sort(E&& e, C& c, F&& f) noexcept requires (!std::is_execution_policy_v<F>) && std::is_execution_policy_v<E>
{
	std::sort(std::forward<E>(e), GetBeginIt(c), GetEndIt(c), std::forward<F>(f));
}

// Reverse sort the whole container
// - use std::sort for custom size container
template<typename C>
constexpr void SortReverse(C& c) noexcept
{
	std::sort(ContainerBeginReverse(c), ContainerEndReverse(c));
}

// Reverse sort the whole container with comparator
// - use std::sort for custom size container
template<typename C, typename F>
constexpr void SortReverse(C& c, F&& f) noexcept requires (!std::is_execution_policy_v<F>)
{
	std::sort(ContainerBeginReverse(c), ContainerEndReverse(c), std::forward<F>(f));
}

// Reverse sort the whole container with custom execution
// - use std::sort for custom size container
template<typename C, typename E>
constexpr void SortReverse(E&& e, C& c) noexcept requires std::is_execution_policy_v<E>
{
	std::sort(std::forward<E>(e), ContainerBeginReverse(c), ContainerEndReverse(c));
}

// Reverse sort the whole container with custom execution and comparator
// - use std::sort for custom size container
template<typename C, typename F, typename E>
constexpr void SortReverse(E&& e, C& c, F&& f) noexcept requires (!std::is_execution_policy_v<F>) && std::is_execution_policy_v<E>
{
	std::sort(std::forward<E>(e), ContainerBeginReverse(c), ContainerEndReverse(c), std::forward<F>(f));
}

// Find a value from a container
// - use std::find for custom size container
template<typename C, typename T>
constexpr auto Find(const C& c, T&& val) noexcept
{
	return std::find(GetBeginIt(c), GetEndIt(c), std::forward<T>(val));
}

// Find a value from a container with custom execution
// - use std::find for custom size container
template<typename C, typename T, typename E>
constexpr auto Find(E&& e, const C& c, T&& val) noexcept requires std::is_execution_policy_v<E>
{
	return std::find(std::forward<E>(e), GetBeginIt(c), GetEndIt(c), std::forward<T>(val));
}


} // namespace AoL
