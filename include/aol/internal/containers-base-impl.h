#pragma once

/*************************************************
* [Containers] Base implementations
*************************************************/

namespace AoL::Internal
{

struct AOL_EMPTY_BASE_OPTIMIZATION ContainerTag_None {};
struct AOL_EMPTY_BASE_OPTIMIZATION ContainerTag_KeyOrderMap {};
struct AOL_EMPTY_BASE_OPTIMIZATION ContainerTag_KeyOrderSet {};
struct AOL_EMPTY_BASE_OPTIMIZATION ContainerTag_Vector {};

/*************************************************
* Iterators for the ContainerBase
* - HasDerivedIteratorImpl checks if the Derived
*   has its own iterator implementation
* - IteratorSelector is used for lazy type
*   instantation for ContainerBase iterator
*************************************************/

template<typename D>
concept HasDerivedIteratorImpl = requires
{
	D::iterator_impl;
	D::const_iterator_impl;
	D::reverse_iterator_impl;
	D::const_reverse_iterator_impl;
};

template<typename D, typename C, bool UseDerived>
struct IteratorSelector
{
	using type = typename C::iterator;
};

template<typename D, typename C>
struct IteratorSelector<D, C, true>
{
	using type = typename D::iterator_impl;
};

template<typename D, typename C, bool UseDerived>
struct ConstIteratorSelector
{
	using type = typename C::const_iterator;
};

template<typename D, typename C>
struct ConstIteratorSelector<D, C, true>
{
	using type = typename D::const_iterator_impl;
};

template<typename D, typename C, bool UseDerived>
struct ReverseIteratorSelector
{
	using type = typename C::reverse_iterator;
};

template<typename D, typename C>
struct ReverseIteratorSelector<D, C, true>
{
	using type = typename D::reverse_iterator_impl;
};

template<typename D, typename C, bool UseDerived>
struct ConstReverseIteratorSelector
{
	using type = typename C::const_reverse_iterator;
};

template<typename D, typename C>
struct ConstReverseIteratorSelector<D, C, true>
{
	using type = typename D::const_reverse_iterator_impl;
};


/*************************************************
* ContainerBase key and mapped type
* - This is used to easily create which type are
*   are used for key and mapped type depending
*   on the container tag
*************************************************/

template<typename T, typename Tag>
struct ContainerKeyType
{
	using type = SizeT;
};

template<typename T>
struct ContainerKeyType<T, ContainerTag_KeyOrderSet>
{
	using type = T;
};

template<typename T>
struct ContainerKeyType<T, ContainerTag_KeyOrderMap>
{
	using type = typename T::first_type;
};

template<typename T, typename Tag>
struct ContainerMappedType
{
	using type = void;
};

template<typename T>
struct ContainerMappedType<T, ContainerTag_KeyOrderMap>
{
	using type = typename T::second_type;
};


/*************************************************
* ContainerBase allocator
* - For non-stl and rot-container, allocator will
*   be void, which is invalid
*************************************************/

template<typename C>
using ContainerAllocatorType = std::conditional_t<Traits::IsSTLContainer<C> || Traits::IsRotContainer<C>, typename C::allocator_type, void>;


/*************************************************
* ContainerBase main implementation
*************************************************/

/**
* @details AoLibrary's main container base implementation
* 
* - Most of AoLibrary's container will always be derived from this
* 
* - This is a CRTP class so it can be a bit of a boilerplate
* 
* - Do not create an object of this class!
* 
* @tparam D derived class for CRTP purposes
* @tparam T element type
* @tparam C container adapted (usually std::vector)
* @tparam Tag container tag (i.e ContainerTag_XXXX)
*/
template<typename D, typename T, typename C, typename Tag>
struct ContainerBase
{
public:
	using container_tag = Tag;
	using container_type = C;

	using iterator = IteratorSelector<D, C, HasDerivedIteratorImpl<D>>::type;
	using const_iterator = ConstIteratorSelector<D, C, HasDerivedIteratorImpl<D>>::type;
	using reverse_iterator = ReverseIteratorSelector<D, C, HasDerivedIteratorImpl<D>>::type;
	using const_reverse_iterator = ConstReverseIteratorSelector<D, C, HasDerivedIteratorImpl<D>>::type;

protected:
	// We use protected ctors and dtor to prevent any creation of this class manually
	// This way, the derived classes can still use them and still not visible to the user

	ContainerBase() noexcept = default;
	ContainerBase(const ContainerBase& other) noexcept = default;
	ContainerBase& operator = (const ContainerBase& other) noexcept = default;
	ContainerBase(ContainerBase&& other) noexcept = default;
	ContainerBase& operator = (ContainerBase&& other) = default;
	~ContainerBase() noexcept = default;

	explicit ContainerBase(SizeT initial_capacity) noexcept :
		container_obj( initial_capacity )
	{
	}

	explicit ContainerBase(const ContainerAllocatorType<C>& allocator) noexcept :
		container_obj( allocator )
	{
	}

	explicit ContainerBase(const container_type& other_data) noexcept :
		container_obj( other_data )
	{
	}

	explicit ContainerBase(container_type&& other_data) noexcept :
		container_obj( std::move(other_data) )
	{
	}

	template<typename It> requires std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<It>::iterator_category>
	explicit ContainerBase(It it_start, It it_end) noexcept :
		container_obj( it_start, it_end )
	{
	}

public:
	void clear() noexcept
	{
		container_obj.clear();
	}

	void reserve(SizeT new_capacity) noexcept
	{
		container_obj.reserve(new_capacity);
	}

	constexpr iterator begin() noexcept
	{
		return container_obj.begin();
	}

	constexpr const_iterator begin() const noexcept
	{
		return container_obj.cbegin();
	}

	constexpr iterator end() noexcept
	{
		return container_obj.end();
	}

	constexpr const_iterator end() const noexcept
	{
		return container_obj.cend();
	}

	constexpr const_iterator cbegin() const noexcept
	{
		return container_obj.cbegin();
	}

	constexpr const_iterator cend() const noexcept
	{
		return container_obj.cend();
	}

	constexpr reverse_iterator rbegin() noexcept
	{
		return container_obj.rbegin();
	}

	constexpr reverse_iterator rend() noexcept
	{
		return container_obj.rend();
	}

	constexpr const_reverse_iterator rbegin() const noexcept
	{
		return container_obj.rbegin();
	}

	constexpr const_reverse_iterator rend() const noexcept
	{
		return container_obj.rend();
	}

	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return container_obj.crbegin();
	}

	constexpr const_reverse_iterator crend() const noexcept
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

/**
* @details AoLibrary's main container base implementation
*
* - This is the invalid template, whenever ContainerTag is not given
*/
template<typename D, typename T, typename C>
struct ContainerBase<D, T, C, ContainerTag_None>
{
	static_assert(Traits::AssertFalse<ContainerTag_None>, "Invalid container tag!");
};

} // AoL::Internal namespace


// containers-base-impl.h EOF