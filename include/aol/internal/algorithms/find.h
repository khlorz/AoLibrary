/***************************************************************************************
* Algorithm Find Implementations
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_ALGORITHM_FIND_H
#define AOL_HEADER_INTERNAL_ALGORITHM_FIND_H

#include "aol/configs.h"
#include "aol/traits.h"
#include "aol/types.h"

#include <algorithm>    // std::find
#include <bit>          // std::bit_floor, bit_ceil
#include <utility>      // std::forward
#include <functional>   // std::less

namespace AoL
{

// Find a value from a container using brute force
// - use std::find for custom size container
template<typename It, typename T>
constexpr auto FindBrute(It it_begin, It it_end, T&& val) noexcept
{
	return std::find(it_begin, it_end, std::forward<T>(val));
}

// Find a value from a container with custom execution
// - use std::find for custom size container
template<typename It, typename T, typename E>
constexpr auto FindBrute(E&& e, It it_begin, It it_end, T&& val) noexcept requires std::is_execution_policy_v<E>
{
	return std::find(std::forward<E>(e), it_begin, it_end, std::forward<T>(val));
}

/*
* @details General usage lower bound algorithm
*
* - Just a slightly modified implementation of std::lower_bound
*
* @tparam It iterator type (can be a pointer)
* @tparam K key type
* @tparam Comparator comparison predicate (default: std::less<void>)
* @param p_start pointer to container address or start
* @param p_end pointer to container end address
* @param key value to be found
* @param compare predicate for < comparison (defaulted to std::less<void>)
* @return iterator to lower bound position for value
*/
template<typename It, typename K, typename Comparator = std::less<void>>
It FindLowerBoundGeneral(It it_begin, It it_end, const K& value, Comparator compare = Comparator{}) noexcept
{
    It it_current = it_begin;
    for (PtrSize current_count = (PtrSize)(it_end - it_current); current_count > 0; )
    {
        PtrSize temp_count = current_count >> 1;
        It mid = it_current + temp_count;
        if (compare(*mid, value))
        {
            it_current = ++mid;
            current_count -= temp_count + 1;
        }
        else
        {
            current_count = temp_count;
        }
    }
    return it_current;
}

/*
* @details Branchless lower bound algorithm
*
* - A 'branchless' implementation for lower bound
* 
* - Usually faster than the general one, but always profile it
*
* - Upon benchmarking, it became slower than the Eytzinger implementation after 10000 elements
*
* @tparam It iterator type (can be a pointer)
* @tparam K key type
* @tparam Comparator comparison predicate (default: std::less<void>)
* @param p_start pointer to container address or start
* @param p_end pointer to container end address
* @param key value to be found
* @param compare predicate for < comparison (defaulted to std::less<void>)
* @return iterator to lower bound position for value
*/
template<typename It, typename K, typename Comparator = std::less<void>>
constexpr It FindLowerBoundBranchless(It it_begin, It it_end, const K& value, Comparator compare = Comparator{}) noexcept
{
    using diff_t = SizeT;

    diff_t length = it_end - it_begin;
    if (length == 0)
    {
        return it_end;
    }

    diff_t step = std::bit_floor(length);
    if (step != length && compare(it_begin[step], value))
    {
        length -= step + 1;
        if (length == 0)
        {
            return it_end;
        }
        step = std::bit_ceil(length);
        it_begin = it_end - step;
    }

    for (step /= 2; step != 0; step /= 2)
    {
        if (compare(it_begin[step], value))
        {
            it_begin += step;
        }
    }

    return it_begin + compare(*it_begin, value);
}

/*
* @details Random lower bound algorith
*
* - Binary search but random
*
* - Can be really fast, can be really slow, depends on your luck
*
* @tparam It iterator type (can be a pointer)
* @tparam K key type
* @tparam Comparator comparison predicate (default: std::less<void>)
* @param p_start pointer to container address or start
* @param p_end pointer to container end address
* @param key value to be found
* @param compare predicate for < comparison (defaulted to std::less<void>)
* @return iterator to lower bound position for value
*/
template<typename It, typename K, typename Comparator = std::less<void>>
It FindLowerBoundRandom(It it_begin, It it_end, const K& value, Comparator compare = Comparator{}) noexcept
{
    using diff_t = SizeT;

    if (it_begin == it_end)
    {
        return it_end;
    }

    it_end--;

    while (it_begin < it_end)
    {
        It mid = it_begin + (std::rand() % static_cast<diff_t>(it_end - it_begin));
        if (!compare(*mid, value))
        {
            it_end = mid + 1;
        }
        else
        {
            it_begin = mid + 1;
        }
    }

    return compare(*it_begin, value) ? it_end : it_begin;
}

/*
* @details Default lower bound algorithm of the library
*
* - This uses the Eytzinger implementation
*
* - Requires the container to be sorted, otherwise, it is UB
*
* @tparam It iterator type (can be a pointer)
* @tparam K key type
* @tparam Comparator comparison predicate (default: std::less<void>)
* @param p_start pointer to container address or start
* @param p_end pointer to container end address
* @param key value to be found
* @param compare predicate for < comparison (defaulted to std::less<void>)
* @return iterator to lower bound position for value
*/
template<typename It, typename K, typename Comparator = std::less<void>>
It FindLowerBound(It it_begin, It it_end, const K& value, Comparator compare = Comparator{}) noexcept
{
    return FindLowerBoundBranchless(it_begin, it_end, value, compare);
}


} // AoL namespace


#endif // AOL_HEADER_INTERNAL_ALGORITHM_FIND_H