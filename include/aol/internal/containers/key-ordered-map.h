/*************************************************
* AoLibrary Ordered Map implementations
*************************************************/
#ifndef AOL_HEADER_INTERNAL_CONTAINERS_KEY_ORDERED_MAP_H
#define AOL_HEADER_INTERNAL_CONTAINERS_KEY_ORDERED_MAP_H

#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/types.h"
#include "aol/traits.h"
#include "aol/vector.h"
#include "aol/algorithms.h"

namespace AoL::Internal
{

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

	constexpr auto operator == (const KeyValuePairEx& other) const noexcept
	{
		return this->first == other.first;
	}
};

template<typename P>
struct PairLessComparator
{
	using pair_type = P;

	template<typename T>
	constexpr bool operator () (AoL::Traits::ConstRefOrCopyType<P> lhs, const T& rhs) noexcept
	{
		return lhs.first < rhs;
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
template<typename K, typename V, typename P, typename C, typename A>
struct KeyOrderMapEx
{
public:
	using container_type = AoL::Vector<P, A>;

	using value_type = P;
	using key_type = K;
	using mapped_type = V;

	using size_type = SizeT;

	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;

private:
	using less_than_comp_type = C;

	less_than_comp_type less_than_comp;

public:
	container_type container_obj;
#if AOL_DEBUG_ON
	bool build_flag;
#endif

	KeyOrderMapEx() noexcept :
		container_obj{ },
		less_than_comp{ }
#if AOL_DEBUG_ON
		, build_flag{ false }
#endif
	{
	}

	KeyOrderMapEx(const KeyOrderMapEx& other) noexcept = default;
	KeyOrderMapEx& operator = (const KeyOrderMapEx& other) noexcept = default;
	KeyOrderMapEx(KeyOrderMapEx&& other) noexcept = default;
	KeyOrderMapEx& operator = (KeyOrderMapEx&& other) noexcept = default;

	explicit KeyOrderMapEx(SizeT initial_capacity) noexcept :
		container_obj{ }
#if AOL_DEBUG_ON
		, build_flag{ false }
#endif
	{
		container_obj.reserve(initial_capacity);
	}

	explicit KeyOrderMapEx(const A& allocator) noexcept :
		container_obj{ allocator }
#if AOL_DEBUG_ON
		, build_flag{ false }
#endif
	{
	}

	explicit KeyOrderMapEx(const container_type& other_data) noexcept :
		container_obj{ other_data }
#if AOL_DEBUG_ON
		, build_flag{ false }
#endif
	{
		Sort(container_obj.begin(), container_obj.end());
	}

	explicit KeyOrderMapEx(container_type&& other_data) noexcept :
		container_obj{ other_data }
#if AOL_DEBUG_ON
		, build_flag{ false }
#endif
	{
		Sort(container_obj.begin(), container_obj.end());
	}

	template<typename It>
	explicit KeyOrderMapEx(It it_start, It it_end) noexcept :
		container_obj{ it_start, it_end }
#if AOL_DEBUG_ON
		, build_flag{ false }
#endif
	{
		static_assert(std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<It>::iterator_category>, "Invalid iterator type!");
		Sort(container_obj.begin(), container_obj.end());
	}

	constexpr void build_start() noexcept
	{
		assert(!build_flag && "Already building! Call build_end() first!");
#if AOL_DEBUG_ON
		build_flag = true;
#endif
	}

	template<typename InKey, typename InValue>
	constexpr void build_add(InKey&& key, InValue&& value) noexcept requires std::is_convertible_v<InKey, key_type>&& std::is_convertible_v<InValue, mapped_type>
	{
		assert(build_flag && "Building haven't started yet! Call build_start() first!");
#if AOL_DEBUG_ON
		const InKey& ref_key = key;
		auto it = AoL::FindBrute(container_obj.begin(), container_obj.end(), value_type{.first = key, .second = value});
		assert(it == container_obj.end() && "Key already exists!");
#endif
		container_obj.emplace_back(std::forward<InKey>(key), std::forward<InValue>(value));
	}

	constexpr void build_end() noexcept
	{
		assert(build_flag && "Building haven't started yet! Call build_start() first!");
#if AOL_DEBUG_ON
		build_flag = false;
#endif
		Sort(container_obj.begin(), container_obj.end());
	}

	template<typename InKey, typename InValue>
	constexpr void insert(InKey&& key, InValue&& value) noexcept requires std::is_convertible_v<InKey, key_type>&& std::is_convertible_v<InValue, mapped_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		const value_type* p_ret = AoL::FindLowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val, less_than_comp);
		if (p_ret >= container_obj.data() + container_obj.size())
		{
			container_obj.emplace_back(std::forward<InKey>(key), std::forward<InValue>(value));
		}
		else
		{
			assert(p_ret->first != key_val && "Item already exists!");
			container_obj.insert(container_obj.begin() + (p_ret - container_obj.data()), value_type{ std::forward<InKey>(key), std::forward<InValue>(value) });
		}
	}

	template<typename InKey>
	constexpr mapped_type& operator[](InKey&& key) noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
#if AOL_DEBUG_ON
		value_type* p_ret = this->find(std::forward<InKey>(key));
		assert(p_ret != nullptr && "Invalid key!");
		return p_ret->second;
#else
		return AoL::FindLowerBound(container_obj.data(), container_obj.data() + container_obj.size(), std::forward<InKey>(key), less_than_comp)->second;
#endif // !NDEBUG
	}

	template<typename InKey, typename R = Traits::ConstRefOrCopyType<mapped_type>>
	constexpr R operator[](InKey&& key) const noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
#if AOL_DEBUG_ON
		const value_type* p_ret = this->find(std::forward<InKey>(key));
		assert(p_ret != nullptr && "Invalid key!");
		return p_ret->second;
#else
		return AoL::FindLowerBound(container_obj.data(), container_obj.data() + container_obj.size(), std::forward<InKey>(key), less_than_comp)->second;
#endif // !NDEBUG
	}

	template<typename InKey>
	mapped_type& at_ref(InKey&& key) noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const auto& key_val = std::forward<InKey>(key);
		value_type* p_ret = AoL::FindLowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val, less_than_comp);
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

	template<typename InKey, typename R = Traits::ConstRefOrCopyType<mapped_type>>
	R at_ref(InKey&& key) const noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const auto& key_val = std::forward<InKey>(key);
		const value_type* p_ret = AoL::FindLowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val, less_than_comp);
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
		value_type* p_ret = AoL::FindLowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val, less_than_comp);
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
		const value_type* p_ret = AoL::FindLowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val, less_than_comp);
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

	AOL_ATTRIB_NO_DISCARD constexpr void clear() noexcept
	{
		return container_obj.clear();
	}

	AOL_ATTRIB_NO_DISCARD constexpr P* data() noexcept
	{
		return container_obj.data();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const P* data() const noexcept
	{
		return container_obj.data();
	}

	AOL_ATTRIB_NO_DISCARD constexpr bool empty() const noexcept
	{
		return container_obj.empty();
	}

	AOL_ATTRIB_NO_DISCARD constexpr size_type size() const noexcept
	{
		return container_obj.size();
	}

	AOL_ATTRIB_NO_DISCARD constexpr iterator begin() noexcept
	{
		return container_obj.begin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_iterator begin() const noexcept
	{
		return container_obj.cbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_iterator cbegin() const noexcept
	{
		return container_obj.cbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr iterator end() noexcept
	{
		return container_obj.end();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_iterator end() const noexcept
	{
		return container_obj.cend();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_iterator cend() const noexcept
	{
		return container_obj.cend();
	}

	AOL_ATTRIB_NO_DISCARD constexpr reverse_iterator rbegin() noexcept
	{
		return container_obj.rbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_reverse_iterator rbegin() const noexcept
	{
		return container_obj.crbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_reverse_iterator crbegin() const noexcept
	{
		return container_obj.crbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr reverse_iterator rend() noexcept
	{
		return container_obj.rend();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_reverse_iterator rend() const noexcept
	{
		return container_obj.crend();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_reverse_iterator crend() const noexcept
	{
		return container_obj.crend();
	}
};

} // AoL::Internal namespace


#endif // AOL_HEADER_INTERNAL_CONTAINERS_KEY_ORDERED_MAP_H