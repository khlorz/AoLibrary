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
* @details Slightly modified lower_bound algorithm
* 
* - Requires the container to be sorted, otherwise, it will be UB
* 
* - This overload requires the pointer to the container and the pointer to the end of the container
* 
* - The value type of the container must have std::pair interface, requiring the member 'first'
* 
* - Usually faster than the iterator overload of this function
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
It FindLowerBound(It it_begin, It it_end, const K& value, Comparator comparator = Comparator{})
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

} // AoL namespace

// EOF