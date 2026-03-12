#pragma once

/*************************************************
* [Containers] Ordered Map implementations
*************************************************/

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
template<typename K, typename V, typename P, typename A, typename C>
struct KeyOrderMapEx : public Internal::ContainerBase<KeyOrderMapEx<K,V,P,A,C>, P, C, ContainerTag_KeyOrderMap>
{
public:
	using Base = Internal::ContainerBase<KeyOrderMapEx<K, V, P, A, C>, P, C, ContainerTag_KeyOrderMap>;

private:
	using typename Base::container_tag;

	using Base::container_obj;

public:
	using value_type = P;
	using key_type = typename ContainerKeyType<P, container_tag>::type;
	using mapped_type = typename ContainerMappedType<P, container_tag>::type;

#ifndef NDEBUG
	bool build_flag;
#endif

	KeyOrderMapEx() noexcept :
		Base{ }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
	}

	KeyOrderMapEx(SizeT initial_capacity) noexcept :
		Base{ initial_capacity }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
	}

	KeyOrderMapEx(const A& allocator) noexcept :
		Base{ allocator }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
	}

	KeyOrderMapEx(const Base::container_type& other_data) noexcept :
		Base{ other_data }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
		Sort(container_obj);
	}

	KeyOrderMapEx(Base::container_type&& other_data) noexcept :
		Base{ other_data }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
		Sort(container_obj);
	}

	template<typename It> requires std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<It>::iterator_category>
	KeyOrderMapEx(It it_start, It it_end) noexcept :
		Base{ it_start, it_end }
#ifndef NDEBUG
		, build_flag{ false }
#endif
	{
		Sort(container_obj);
	}

	constexpr void build_start() noexcept
	{
		assert(!build_flag && "Already building! Call build_end() first!");
#ifndef NDEBUG
		build_flag = true;
#endif
	}

	template<typename InKey, typename InValue>
	constexpr void build_add(InKey&& key, InValue&& value) noexcept requires std::is_convertible_v<InKey, key_type>&& std::is_convertible_v<InValue, mapped_type>
	{
		assert(build_flag && "Building haven't started yet! Call build_start() first!");
#ifndef NDEBUG
		const InKey& ref_key = key;
		auto it = std::find(container_obj.begin(), container_obj.end(), ref_key);
		assert(it == container_obj.end() && "Key already exists!");
#endif
		container_obj.emplace_back(std::forward<InKey>(key), std::forward<InValue>(value));
	}

	constexpr void build_end() noexcept
	{
		assert(build_flag && "Building haven't started yet! Call build_start() first!");
#ifndef NDEBUG
		build_flag = false;
#endif
		Sort(container_obj);
	}

	template<typename InKey, typename InValue>
	constexpr void insert(InKey&& key, InValue&& value) noexcept requires std::is_convertible_v<InKey, key_type>&& std::is_convertible_v<InValue, mapped_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		const value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
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
#ifndef NDEBUG
		value_type* p_ret = this->find(std::forward<InKey>(key));
		assert(p_ret != nullptr && "Invalid key!");
		return p_ret->second;
#else
		return LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), std::forward<InKey>(key))->second;
#endif // !NDEBUG
	}

	template<typename InKey, typename R = Traits::ConstRefOrCopyType<mapped_type>>
	constexpr R operator[](InKey&& key) const noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
#ifndef NDEBUG
		const value_type* p_ret = this->find(std::forward<InKey>(key));
		assert(p_ret != nullptr && "Invalid key!");
		return p_ret->second;
#else
		return LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), std::forward<InKey>(key))->second;
#endif // !NDEBUG
	}

	template<typename InKey>
	mapped_type& at_ref(InKey&& key) noexcept requires std::is_convertible_v<InKey, key_type>
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const auto& key_val = std::forward<InKey>(key);
		value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
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
		const value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
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
		value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
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
		const value_type* p_ret = LowerBound(container_obj.data(), container_obj.data() + container_obj.size(), key_val);
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

};

} // AoL::Internal namespace


// containers-ordered-map-impl.h EOF