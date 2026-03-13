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
* - HasXXXImpl regarding iterators
* - XXX_impl methods are needed to use the derived
*   methods, otherwise, it will default to the
*   begin(), end(), etc. of the underlying
*	container
*************************************************/

template<typename D>
concept HasBeginImpl = requires(D d)
{
	d.begin_impl();
};

template<typename D>
concept HasCBeginImpl = requires(D d)
{
	d.cbegin_impl();
};

template<typename D>
concept HasRBeginImpl = requires(D d)
{
	d.rbegin_impl();
};

template<typename D>
concept HasCRBeginImpl = requires(D d)
{
	d.crbegin_impl();
};

template<typename D>
concept HasEndImpl = requires(D d)
{
	d.end_impl();
};

template<typename D>
concept HasCEndImpl = requires(D d)
{
	d.cend_impl();
};

template<typename D>
concept HasREndImpl = requires(D d)
{
	d.rend_impl();
};

template<typename D>
concept HasCREndImpl = requires(D d)
{
	d.crend_impl();
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
	
	AOL_NO_DISCARD constexpr auto begin() noexcept
	{
		if constexpr (HasBeginImpl<D>)
		{
			return static_cast<D*>(this)->begin_impl();
		}
		else
		{
			return container_obj.begin();
		}
	}

	AOL_NO_DISCARD constexpr auto begin() const noexcept
	{
		if constexpr (HasCBeginImpl<D>)
		{
			return static_cast<const D*>(this)->cbegin_impl();
		}
		else
		{
			return container_obj.cbegin();
		}
	}

	AOL_NO_DISCARD constexpr auto end() noexcept
	{
		if constexpr (HasEndImpl<D>)
		{
			return static_cast<D*>(this)->end_impl();
		}
		else
		{
			return container_obj.end();
		}
	}

	AOL_NO_DISCARD constexpr auto end() const noexcept
	{
		if constexpr (HasCEndImpl<D>)
		{
			return static_cast<const D*>(this)->cend_impl();
		}
		else
		{
			return container_obj.cend();
		}
	}

	AOL_NO_DISCARD constexpr auto cbegin() const noexcept
	{
		if constexpr (HasCBeginImpl<D>)
		{
			return static_cast<const D*>(this)->cbegin_impl();
		}
		else
		{
			return container_obj.cbegin();
		}
	}

	AOL_NO_DISCARD constexpr auto cend() const noexcept
	{
		if constexpr (HasCEndImpl<D>)
		{
			return static_cast<const D*>(this)->cend_impl();
		}
		else
		{
			return container_obj.cend();
		}
	}

	AOL_NO_DISCARD constexpr auto rbegin() noexcept
	{
		if constexpr (HasRBeginImpl<D>)
		{
			return static_cast<D*>(this)->rbegin_impl();
		}
		else
		{
			return container_obj.rbegin();
		}
	}

	AOL_NO_DISCARD constexpr auto rend() noexcept
	{
		if constexpr (HasREndImpl<D>)
		{
			return static_cast<D*>(this)->rend_impl();
		}
		else
		{
			return container_obj.rend();
		}
	}

	AOL_NO_DISCARD constexpr auto rbegin() const noexcept
	{
		if constexpr (HasCRBeginImpl<D>)
		{
			return static_cast<const D*>(this)->crbegin_impl();
		}
		else
		{
			return container_obj.crbegin();
		}
	}

	AOL_NO_DISCARD constexpr auto rend() const noexcept
	{
		if constexpr (HasCREndImpl<D>)
		{
			return static_cast<const D*>(this)->crend_impl();
		}
		else
		{
			return container_obj.crend();
		}
	}

	AOL_NO_DISCARD constexpr auto crbegin() const noexcept
	{
		if constexpr (HasCRBeginImpl<D>)
		{
			return static_cast<const D*>(this)->crbegin_impl();
		}
		else
		{
			return container_obj.crbegin();
		}
	}

	AOL_NO_DISCARD constexpr auto crend() const noexcept
	{
		if constexpr (HasCREndImpl<D>)
		{
			return static_cast<const D*>(this)->crend_impl();
		}
		else
		{
			return container_obj.crend();
		}
	}

	AOL_NO_DISCARD constexpr auto size() const noexcept
	{
		return container_obj.size();
	}

	AOL_NO_DISCARD constexpr auto empty() const noexcept
	{
		return container_obj.empty();
	}

	AOL_NO_DISCARD constexpr auto data() noexcept
	{
		return container_obj.data();
	}

	AOL_NO_DISCARD constexpr auto data() const noexcept
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