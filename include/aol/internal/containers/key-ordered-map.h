/*************************************************
* AoLibrary Ordered Map implementations
*************************************************/
#ifndef AOL_HEADER_INTERNAL_CONTAINERS_KEY_ORDERED_MAP_H
#define AOL_HEADER_INTERNAL_CONTAINERS_KEY_ORDERED_MAP_H


#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/traits.h"
#include "aol/types.h"
#include "aol/vector.h"
#include "aol/algorithms.h"

#include <memory> // std::addressof


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

	constexpr bool operator == (const KeyValuePairEx& other) const noexcept
	{
		return this->first == other.first;
	}

	template<typename T>
		requires requires (const K& k, const T& t) { k <=> t; }
	constexpr auto operator <=> (const T& key) const noexcept
	{
		return this->first <=> key;
	}

	template<typename T>
		requires requires (const K& k, const T& t) { k == t; }
	constexpr bool operator == (const T& key) const noexcept
	{
		return this->first == key;
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
*/
template<typename K, typename V, typename P, typename A>
struct KeyOrderMapEx
{
	using container_type = AoL::Vector<P, A>;

	using value_type = P;
	using key_type = K;
	using mapped_type = V;

	using size_type = SizeT;

	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;

	static_assert(requires(P p){ p.first; p.second; }, "P type must have a member named \"first\" and \"second\" where \"first\" is the key and \"second\" is the value");
	static_assert(requires(const P& a, const P& b){ {a == b} -> std::convertible_to<bool>; {a <=> b} -> std::convertible_to<std::strong_ordering>; },"P must provide == and <=> (compared by key only) for sort/stable_sort/unique dedup");
	static_assert(std::same_as<typename P::first_type, K>, "P::first_type must match K");
	static_assert(std::same_as<typename P::second_type, V>, "P::second_type must match V");
	static_assert(std::totally_ordered<K>, "Key K must be totally ordered");

	container_type container_obj;
#if AOL_DEBUG_ON
	bool build_flag;
#endif

	KeyOrderMapEx() noexcept :
		container_obj{ }
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
		std::stable_sort(container_obj.begin(), container_obj.end());
		container_obj.erase(std::unique(container_obj.begin(), container_obj.end()), container_obj.end());
	}

	explicit KeyOrderMapEx(container_type&& other_data) noexcept :
		container_obj{ std::move(other_data) }
#if AOL_DEBUG_ON
		, build_flag{ false }
#endif
	{
		std::stable_sort(container_obj.begin(), container_obj.end());
		container_obj.erase(std::unique(container_obj.begin(), container_obj.end()), container_obj.end());
	}

	template<typename It>
	explicit KeyOrderMapEx(It it_start, It it_end) noexcept :
		container_obj{ it_start, it_end }
#if AOL_DEBUG_ON
		, build_flag{ false }
#endif
	{
		static_assert(std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<It>::iterator_category>, "Invalid iterator type!");
		std::stable_sort(container_obj.begin(), container_obj.end());
		container_obj.erase(std::unique(container_obj.begin(), container_obj.end()), container_obj.end());
	}

	constexpr void build_start(SizeT expected = 0) noexcept
	{
		assert(!build_flag && "Already building! Call build_end() first!");
#if AOL_DEBUG_ON
		build_flag = true;
#endif
		if (expected > 0)
		{
			container_obj.reserve(container_obj.size() + expected);
		}
	}

	template<typename InKey, typename InValue>
		requires std::is_convertible_v<InKey, key_type> && std::is_convertible_v<InValue, mapped_type>
	constexpr void build_add(InKey&& key, InValue&& value) noexcept
	{
		assert(build_flag && "Building haven't started yet! Call build_start() first!");
#if AOL_DEBUG_ON
		const InKey& ref_key = key;
		auto it = AoL::FindBrute(container_obj.begin(), container_obj.end(), key);
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
	constexpr void insert(InKey&& key, InValue&& value) noexcept
	{
		static_assert(std::is_convertible_v<InKey, key_type> && std::is_convertible_v<InValue, mapped_type>, "Input key type and value type should be convertible to the map's key and value types!");

		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		auto it = this->find_impl(key_val);
		if (it >= container_obj.end())
		{
			container_obj.emplace_back(std::forward<InKey>(key), std::forward<InValue>(value));
		}
		else
		{
			assert(it->first != key_val && "Item already exists!");
			container_obj.emplace(it, std::forward<InKey>(key), std::forward<InValue>(value));
		}
	}

	template<typename InKey>
	constexpr mapped_type& operator[](InKey&& key) noexcept
	{
		static_assert(std::is_convertible_v<InKey, key_type>, "Input key type should be convertible to the map's key type!");

		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		auto find_it = this->find_impl(key_val);
		if (find_it < container_obj.end() && find_it->first == key_val)
		{
			return find_it->second;
		}
		else
		{
			auto insert_it = container_obj.emplace(find_it, std::forward<InKey>(key), mapped_type{});
			return insert_it->second;
		}
	}

	template<typename InKey>
	mapped_type& at_ref(InKey&& key) noexcept
	{
		static_assert(std::is_convertible_v<InKey, key_type>, "Input key type should be convertible to the map's key type!");

		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
#if AOL_DEBUG_ON
		const InKey& key_val = key;
		auto it = this->find_impl(key_val);
		assert(it < container_obj.end() && it->first == key && "Invalid key!");
		return it->second;
#else
		return this->find(std::forward<InKey>(key))->second;
#endif // !NDEBUG
	}

	template<typename InKey>
	const mapped_type& at_ref(InKey&& key) const noexcept
	{
		static_assert(std::is_convertible_v<InKey, key_type>, "Input key type should be convertible to the map's key type!");

		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
#if AOL_DEBUG_ON
		const InKey& key_val = key;
		auto it = this->find_impl(key_val);
		assert(it < container_obj.end() && it->first == key_val && "Invalid key!");
		return it->second;
#else
		return this->find(std::forward<InKey>(key))->second;
#endif // !NDEBUG
	}

	template<typename InKey>
	mapped_type* at_ptr(InKey&& key) noexcept
	{
		static_assert(std::is_convertible_v<InKey, key_type>, "Input key type should be convertible to the map's key type!");

		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		auto it = this->find_impl(key_val);
		return it < container_obj.end() && it->first == key_val ? std::addressof(it->second) : nullptr;
	}

	template<typename InKey>
	const mapped_type* at_ptr(InKey&& key) const noexcept
	{
		static_assert(std::is_convertible_v<InKey, key_type>, "Input key type should be convertible to the map's key type!");

		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		auto it = this->find_impl(key_val);
		return it < container_obj.end() && it->first == key_val ? std::addressof(it->second) : nullptr;
	}

	template<typename InKey>
	constexpr auto find(InKey&& key) noexcept
	{
		const InKey& key_val = key;
		auto it = this->find_impl(key_val);
		if (it == this->end())
		{
			return this->end();
		}
		return it->first == key_val ? it : this->end();
	}

	template<typename InKey>
	constexpr auto find(InKey&& key) const noexcept
	{
		const InKey& key_val = key;
		auto it = this->find_impl(key_val);
		if (it == this->cend())
		{
			return this->cend();
		}
		return it->first == key_val ? it : this->cend();
	}

	template<typename InKey>
	constexpr auto find_impl(InKey&& key) noexcept
	{
		static_assert(std::is_convertible_v<InKey, key_type>, "Input key type should be convertible to the map's key type!");

		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		return AoL::FindLowerBound(container_obj.begin(), container_obj.end(), key_val);
	}

	template<typename InKey>
	constexpr auto find_impl(InKey&& key) const noexcept
	{
		static_assert(std::is_convertible_v<InKey, key_type>, "Input key type should be convertible to the map's key type!");

		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		return AoL::FindLowerBound(container_obj.begin(), container_obj.end(), key_val);
	}

	template<typename InKey>
	constexpr bool contains(InKey&& key) const noexcept
	{
		static_assert(std::is_convertible_v<InKey, key_type>, "Input key type should be convertible to the map's key type!");

		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		const InKey& key_val = key;
		auto it = this->find_impl(key_val);
		return it < container_obj.end() && it->first == key_val;
	}

	AOL_ATTRIB_NO_DISCARD constexpr void clear() noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.clear();
	}

	AOL_ATTRIB_NO_DISCARD constexpr P* data() noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.data();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const P* data() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.data();
	}

	AOL_ATTRIB_NO_DISCARD constexpr bool empty() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.empty();
	}

	AOL_ATTRIB_NO_DISCARD constexpr size_type size() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.size();
	}

	AOL_ATTRIB_NO_DISCARD constexpr iterator begin() noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.begin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_iterator begin() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.cbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_iterator cbegin() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.cbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr iterator end() noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.end();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_iterator end() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.cend();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_iterator cend() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.cend();
	}

	AOL_ATTRIB_NO_DISCARD constexpr reverse_iterator rbegin() noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.rbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_reverse_iterator rbegin() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.crbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_reverse_iterator crbegin() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.crbegin();
	}

	AOL_ATTRIB_NO_DISCARD constexpr reverse_iterator rend() noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.rend();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_reverse_iterator rend() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.crend();
	}

	AOL_ATTRIB_NO_DISCARD constexpr const_reverse_iterator crend() const noexcept
	{
		assert(!build_flag && "Building haven't finished yet! Call build_end() first!");
		return container_obj.crend();
	}
};

} // AoL::Internal namespace


#endif // AOL_HEADER_INTERNAL_CONTAINERS_KEY_ORDERED_MAP_H