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

// Find a value from a container
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

template<typename C, typename K> requires Internal::IsValidLowerBoundType<C, K>
C* FindLowerBound(const C* p_start, const C* p_end, const K& key)
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

template<typename It, typename K, typename Comparator>
It FindLowerBound(It it_begin, It it_end, const K& value, Comparator comparator)
{
    It p_current = it_begin;
    for (PtrSize current_count = (PtrSize)(it_end - p_current); current_count > 0; )
    {
        PtrSize temp_count = current_count >> 1;
        It mid = p_current + temp_count;
        if (comparator(*mid, value))
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

} // AoL namespace

// EOF