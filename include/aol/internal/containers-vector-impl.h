#pragma once

/*************************************************
* [Container] Vector implementations
*************************************************/

namespace AoL::Internal
{

/**
* @details Circular vector for circular purposes like overwriting the oldest and such
* 
* @tparam T element type
* @tparam A allocator type
*/
template<
	typename T,
	typename A
>
struct VectorCircularEx : ContainerBase<VectorCircularEx<T,A>, T, Vector<T,A>, ContainerTag_Vector>
{
private:
	using Base = ContainerBase<VectorCircularEx<T, A>, T, Vector<T, A>, ContainerTag_Vector>;

	using Base::container_obj;

public:
	using container_type = Vector<T,A>;

	using value_type = container_type::value_type;
	using allocator_type = container_type::allocator_type;

	using pointer = container_type::pointer;
	using const_pointer = container_type::const_pointer;
	using reference = container_type::reference;
	using const_reference = container_type::const_reference;

	SizeT mask;
	SizeT head;
	SizeT item_count;

	VectorCircularEx() noexcept :
		Base{ },
		mask{0},
		head{0},
		item_count{0}
	{}

	VectorCircularEx(const VectorCircularEx& other) noexcept = default;
	VectorCircularEx& operator = (const VectorCircularEx& other) noexcept = default;

	VectorCircularEx(VectorCircularEx&& other) noexcept :
		Base{ std::move(other.container_obj) },
		mask{ other.mask },
		head{ other.head },
		item_count{ other.item_count }
	{
		other.head = other.item_count = other.mask = static_cast<SizeT>(0);
	}

	VectorCircularEx& operator = (VectorCircularEx&& other) noexcept
	{
		container_obj = std::move(other.container_obj);
		mask = other.mask;
		head = other.head;
		item_count = other.item_count;
		other.head = other.item_count = other.mask = static_cast<SizeT>(0);
		return *this;
	}

	explicit VectorCircularEx(SizeT item_limit) noexcept :
		Base{ item_limit },
		mask{ item_limit - 1 },
		head{ 0 },
		item_count{ 0 }
	{
		assert(std::has_single_bit(item_limit) && "Invalid limit! Must be power of 2!");
	}

	AOL_NO_DISCARD constexpr bool full() const noexcept
	{
		return item_count == this->capacity();
	}

	AOL_NO_DISCARD constexpr bool empty() const noexcept
	{
		return item_count == 0;
	}

	AOL_NO_DISCARD constexpr auto size() const noexcept
	{
		return item_count;
	}

	AOL_NO_DISCARD constexpr auto capacity() const noexcept
	{
		return container_obj.size();
	}

	void push_back(T&& new_item) noexcept
	{
		assert(mask > 0 && "No assigned item limit yet! Cannot add items!");
		
		container_obj[(head + item_count) & mask] = std::forward<T>(new_item);
		if (item_count == this->capacity())
		{
			head = (head + 1) & mask;
		}
		else
		{
			item_count++;
		}
	}

	T& operator[](size_t idx) noexcept
	{
		assert(idx < item_count && "Invalid operation! Input idx out of range!");
		return container_obj[(head + idx) & mask];
	}

	Traits::ConstRefOrCopyType<T> operator[](size_t idx) const noexcept
	{
		assert(idx < item_count && "Invalid operation! Input idx out of range!");
		return container_obj[(head + idx) & mask];
	}

	T& front()
	{
		assert(item_count > 0);
		return container_obj[head];
	}

	T& back()
	{
		assert(item_count > 0);
		return container_obj[(head + item_count - 1) & mask];
	}
};

} // AoL::Internal namespace


// containers-vector-impl.h EOF