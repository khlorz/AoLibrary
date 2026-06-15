/***************************************************************************************
* Algorithm Find Implementations
***************************************************************************************/

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

template<typename T1, typename T2>
concept HasComparableKey = requires(T1 t1, T2 t2)
{
    { t1.first < t2 } -> std::convertible_to<bool>;
};

template<typename T, typename K>
concept IsValidLowerBoundType = IsPairType<T> && HasComparableKey<T, K>;

}

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
* @return iterator to lower bound position for value
*/
template<typename It, typename K, typename Comparator = std::less<void>>
It FindLowerBoundGeneral(It it_begin, It it_end, const K& value, Comparator comparator = Comparator{}) noexcept
{
    It it_current = it_begin;
    for (PtrSize current_count = (PtrSize)(it_end - it_current); current_count > 0; )
    {
        PtrSize temp_count = current_count >> 1;
        It mid = it_current + temp_count;
        if (comparator(*mid, value))
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
* @return iterator to lower bound position for value
*/
template<typename It, typename K, typename Comparator = std::less<void>>
constexpr It FindLowerBoundBranchless(It it_begin, It it_end, const K& value, Comparator compare = Comparator{}) noexcept
{
    size_t length = it_end - it_begin;
    if (length == 0)
        return it_end;
    size_t step = std::bit_floor(length);
    if (step != length && compare(it_begin[step], value))
    {
        length -= step + 1;
        if (length == 0)
            return it_end;
        step = std::bit_ceil(length);
        it_begin = it_end - step;
    }
    for (step /= 2; step != 0; step /= 2)
    {
        if (compare(it_begin[step], value))
            it_begin += step;
    }
    return it_begin + compare(*it_begin, value);
}

/*
* @details Eytzinger based lower bound algorith
*
* - Based on the Algorithmica algorithm, translated to C++
* 
* - Upon benchmarking, it became faster than the Branchless implementation after 100000 elements
* 
* - Benchmark and profile in your use case just to be sure
*
* @tparam It iterator type (can be a pointer)
* @tparam K key type
* @tparam Comparator comparison predicate (default: std::less<void>)
* @param p_start pointer to container address or start
* @param p_end pointer to container end address
* @param key value to be found
* @return iterator to lower bound position for value
*/
template<typename It, typename K, typename Comparator = std::less<void>>
It FindLowerBoundEytzinger(It it_begin, It it_end, const K& value, Comparator comp = Comparator{}) noexcept
{
    using diff_t = AoL::SizeT;

    diff_t n = (diff_t)(it_end - it_begin);
    if (n <= 0)
        return it_end;

    // Eytzinger uses 1-based indexing
    diff_t k = 1;

    while (k <= n)
    {
        const auto& elem = *(it_begin + k);

        AOL_MACRO_FUNC_PREFETCH(std::to_address(it_begin + k) + 16);

        // if elem < value => go right
        k = 2 * k + (comp(elem, value) ? 1 : 0);
    }

    // recover last valid node
    k >>= AOL_MACRO_FUNC_COUNTR_ONE(k);

    if (k == 0 || k > n)
        return it_end;

    return it_begin + k;
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
* @return iterator to lower bound position for value
*/
template<typename It, typename K, typename Comparator = std::less<void>>
It FindLowerBoundRandom(It it_begin, It it_end, const K& value, Comparator comp = Comparator{}) noexcept
{
    if (it_begin == it_end)
    {
        return it_end;
    }

    for (size_t len = it_end - it_begin; len > 1; )
    {
        It mid = it_begin + (std::rand() % len);
        if (!comp(*mid, value))
        {
            it_end = mid + 1;
        }
        else
        {
            it_begin = mid + 1;
        }
    }

    return comp(*it_begin, value) ? it_end : it_begin;
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
* @return iterator to lower bound position for value
*/
template<typename It, typename K, typename Comparator = std::less<void>>
It FindLowerBound(It it_begin, It it_end, const K& value, Comparator comparator = Comparator{}) noexcept
{
    return FindLowerBoundEytzinger(it_begin, it_end, value, comparator);
}


} // AoL namespace

// EOF