#pragma once

/*************************************************
* [Containers] Base implementations
*************************************************/

namespace AoL::Internal
{

template<ContainerTypeTag CT, typename T>
struct ContainerKeyType
{
	using type = SizeT;
};

template<typename T>
struct ContainerKeyType<ContainerTypeTag::KeyOrderMap, T>
{
	using type = typename T::first_type;
};

template<ContainerTypeTag CT, typename T>
struct ContainerValueType
{
	using type = T;
};

template<typename T>
struct ContainerValueType<ContainerTypeTag::KeyOrderMap, T>
{
	using type = typename T::second_type;
};

template<typename C>
using ContainerAllocatorType = std::conditional_t<Traits::IsSTLContainer<C> || Traits::IsRotContainer<C>, typename C::allocator_type, void>;

template<ContainerTypeTag CT, typename T, typename C>
struct ContainerBase
{
	static constexpr ContainerTypeTag internal_type_tag = CT;

	using container_type = typename C;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;

	ContainerBase() noexcept = default;
	ContainerBase(const ContainerBase& other) noexcept = default;
	ContainerBase& operator = (const ContainerBase& other) noexcept = default;
	ContainerBase(ContainerBase&& other) noexcept = default;
	ContainerBase& operator = (ContainerBase&& other) = default;
	~ContainerBase() noexcept = default;

	ContainerBase(SizeT initial_capacity) noexcept :
		container_obj{ }
	{
		container_obj.reserve(initial_capacity);
	}

	ContainerBase(const ContainerAllocatorType<C>& allocator) noexcept :
		container_obj{ allocator }
	{
	}

	ContainerBase(const container_type& other_data) noexcept :
		container_obj{ other_data }
	{
	}

	ContainerBase(container_type&& other_data) noexcept :
		container_obj{ std::move(other_data) }
	{
	}

	template<typename It> requires std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<It>::iterator_category>
	ContainerBase(It it_start, It it_end) noexcept :
		container_obj{ it_start, it_end }
	{
	}

	void clear() noexcept
	{
		container_obj.clear();
	}

	void reserve(SizeT new_capacity) noexcept
	{
		container_obj.reserve(new_capacity);
	}

	constexpr auto begin() noexcept
	{
		return container_obj.begin();
	}

	constexpr auto begin() const noexcept
	{
		return container_obj.cbegin();
	}

	constexpr auto end() noexcept
	{
		return container_obj.end();
	}

	constexpr auto end() const noexcept
	{
		return container_obj.cend();
	}

	constexpr auto cbegin() const noexcept
	{
		return container_obj.cbegin();
	}

	constexpr auto cend() const noexcept
	{
		return container_obj.cend();
	}

	constexpr auto rbegin() noexcept
	{
		return container_obj.rbegin();
	}

	constexpr auto rend() noexcept
	{
		return container_obj.rend();
	}

	constexpr auto rbegin() const noexcept
	{
		return container_obj.rbegin();
	}

	constexpr auto rend() const noexcept
	{
		return container_obj.rend();
	}

	constexpr auto crbegin() const noexcept
	{
		return container_obj.crbegin();
	}

	constexpr auto crend() const noexcept
	{
		return container_obj.crend();
	}

	constexpr auto size() const noexcept
	{
		return container_obj.size();
	}

	constexpr auto empty() const noexcept
	{
		return container_obj.empty();
	}

	constexpr auto data() noexcept
	{
		return container_obj.data();
	}

	constexpr auto data() const noexcept
	{
		return container_obj.data();
	}

protected:
	container_type container_obj;

};

} // AoL::Internal namespace


// containers-base-impl.h EOF