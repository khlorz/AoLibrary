#pragma once

/*************************************************
* [Container] Vector implementations
*************************************************/

namespace AoL::Internal
{

/**
* @details VectorCircularEx iterator
*
* - Iterate made solely for VectorCircular
*/
template<
	typename C
>
struct VectorCircularExIterator
{
	using container_type = std::remove_const_t<C>;
    using container_ptr = C*;

	using value_type = typename container_type::value_type;
	using allocator_type = typename container_type::allocator_type;
	using difference_type = PtrDiff;

	using iterator_concept = std::random_access_iterator_tag;
	using iterator_category = std::random_access_iterator_tag;

	using pointer = std::conditional_t<std::is_const_v<C>, typename container_type::const_pointer, typename container_type::pointer>;
	using reference = std::conditional_t<std::is_const_v<C>, typename container_type::const_reference, typename container_type::reference>;

    container_ptr container; // beginning of array
    size_t idx; // offset into array

    constexpr VectorCircularExIterator() noexcept :
        container{ nullptr },
        idx{ 0 }
    {
    }

    constexpr explicit VectorCircularExIterator(C* c_ptr, size_t offset = 0) noexcept :
        container(c_ptr),
        idx(offset)
    {
        assert(idx <= container->size() && "Invalid offset! Offset is way beyond the array size!");
    }

    AOL_NO_DISCARD constexpr reference operator*() const noexcept
    {
        assert(container && "Invalid operation! Cannot dereference nullptr array iterator!");
        assert(idx < container->size() && "Invalid operation! Cannot dereference out of range array iterator!");
        return (*container)[idx];
    }

    AOL_NO_DISCARD constexpr pointer operator->() const noexcept
    {
        return &this->operator*();
    }

    constexpr VectorCircularExIterator& operator++() noexcept
    {
        assert(container && "Invalid operation! cannot increment nullptr array iterator!");
        assert(idx < container->size() && "Invalid operation! Cannot increment array iterator past end!");
        ++idx;
        return *this;
    }

    constexpr VectorCircularExIterator operator++(int) noexcept
    {
        VectorCircularExIterator tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr VectorCircularExIterator& operator--() noexcept
    {
        assert(container && "Invalid operation! Cannot decrement nullptr array iterator!");
        assert(idx != 0 && "Invalid operation! Cannot decrement array iterator before begin!");
        --idx;
        return *this;
    }

    constexpr VectorCircularExIterator operator--(int) noexcept
    {
        VectorCircularExIterator tmp = *this;
        --*this;
        return tmp;
    }

    constexpr VectorCircularExIterator& operator+=(const difference_type offset) noexcept
    {
        AssertValidOffset(offset);
        idx += static_cast<size_t>(offset);
        return *this;
    }

    constexpr VectorCircularExIterator& operator-=(const difference_type offset) noexcept
    {
        return *this += -offset;
    }

    AOL_NO_DISCARD constexpr difference_type operator-(const VectorCircularExIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return static_cast<difference_type>(idx - other.idx);
    }

    AOL_NO_DISCARD constexpr reference operator[](const difference_type offset) const noexcept
    {
        return *(*this + offset);
    }

    AOL_NO_DISCARD constexpr bool operator==(const VectorCircularExIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return idx == other.idx;
    }

    AOL_NO_DISCARD constexpr std::strong_ordering operator<=>(const VectorCircularExIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return idx <=> other.idx;
    }

private:
#ifndef NDEBUG
    constexpr void AssertValidOffset(const difference_type offset) const noexcept
    {
        if (offset != 0) {
            assert(container && "Invalid operation! Cannot seek nullptr array iterator!");
        }

        if (offset < 0) {
            assert(idx >= size_t{ 0 } - static_cast<size_t>(offset) && "Invalid operation! Cannot seek array iterator before begin!");
        }

        if (offset > 0) {
            assert(container->size() - idx >= static_cast<size_t>(offset) && "Invalid operation! Cannot seek array iterator after end!");
        }
    }

    constexpr void AssertCompatibility(const VectorCircularExIterator& other) const noexcept
    {
        assert(container == other.container && "Invalid operation! Array iterators incompatible!");
    }
#else
    constexpr void AssertValidOffset(const difference_type offset) const noexcept
    {
        // empty function
    }

    constexpr void AssertCompatibility(const VectorCircularExIterator& other) const noexcept
    {
        // empty function
    }
#endif

public:
    AOL_NO_DISCARD constexpr VectorCircularExIterator operator+(const difference_type offset) const noexcept
    {
        VectorCircularExIterator tmp = *this;
        tmp += offset;
        return tmp;
    }

    AOL_NO_DISCARD constexpr VectorCircularExIterator operator-(const difference_type offset) const noexcept
    {
        VectorCircularExIterator tmp = *this;
        tmp -= offset;
        return tmp;
    }

    AOL_NO_DISCARD friend constexpr VectorCircularExIterator operator+(const difference_type offset, VectorCircularExIterator next) noexcept
    {
        next += offset;
        return next;
    }
};

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

    using iterator = VectorCircularExIterator<VectorCircularEx<T, A>>;
    using const_iterator = VectorCircularExIterator<const VectorCircularEx<T, A>>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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

	constexpr void push_back(T&& new_item) noexcept
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

    constexpr void increase_capacity(SizeT new_item_limit) noexcept
    {
        assert(new_item_limit > 0 && std::has_single_bit(new_item_limit) && new_item_limit > this->capacity() && "Invalid new limit! Must be power of 2 and larger than current capacity!");

        if (head > 0)
        {
            SizeT tail_count = std::min(item_count, this->capacity() - head);
            for (SizeT i = 0; i < tail_count; ++i)
            {
                std::swap(container_obj[i], container_obj[head + i]);
            }
        }
        container_obj.resize(new_item_limit);
        mask = new_item_limit - 1;
        head = 0;
    }

    AOL_NO_DISCARD constexpr T& operator[](size_t idx) noexcept
	{
		assert(idx < item_count && "Invalid operation! Input idx out of range!");
		return container_obj[(head + idx) & mask];
	}

    AOL_NO_DISCARD constexpr Traits::ConstRefOrCopyType<T> operator[](size_t idx) const noexcept
	{
		assert(idx < item_count && "Invalid operation! Input idx out of range!");
		return container_obj[(head + idx) & mask];
	}

    AOL_NO_DISCARD constexpr T& front()
	{
		assert(item_count > 0);
		return container_obj[head];
	}

    AOL_NO_DISCARD constexpr T& back()
	{
		assert(item_count > 0);
		return container_obj[(head + item_count - 1) & mask];
	}

    AOL_NO_DISCARD constexpr auto begin_impl() noexcept
    {
        return iterator(this, 0);
    }

    AOL_NO_DISCARD constexpr auto cbegin_impl() const noexcept
    {
        return const_iterator(this, 0);
    }

    AOL_NO_DISCARD constexpr auto end_impl() noexcept
    {
        return iterator(this, item_count);
    }

    AOL_NO_DISCARD constexpr auto cend_impl() const noexcept
    {
        return const_iterator(this, item_count);
    }

    AOL_NO_DISCARD constexpr auto rbegin_impl() noexcept
    {
        return reverse_iterator(this, 0);
    }

    AOL_NO_DISCARD constexpr auto crbegin_impl() const noexcept
    {
        return const_reverse_iterator(this, 0);
    }

    AOL_NO_DISCARD constexpr auto rend_impl() noexcept
    {
        return reverse_iterator(this, item_count);
    }

    AOL_NO_DISCARD constexpr auto crend_impl() const noexcept
    {
        return const_reverse_iterator(this, item_count);
    }
};

} // AoL::Internal namespace


// containers-vector-impl.h EOF