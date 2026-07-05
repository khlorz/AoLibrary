#pragma once

/*************************************************
* [Container] Cyclic Buffer implementations
*************************************************/

namespace AoL::Internal
{

template<
    typename C
>
struct CyclicBufferIterator
{
    using container_type = std::remove_const_t<C>;
    using container_ptr = C*;

    using value_type = typename container_type::value_type;
    using difference_type = PtrDiff;
    using size_type = SizeT;

    using iterator_concept = std::random_access_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;

    using pointer = std::conditional_t<std::is_const_v<C>, typename container_type::const_pointer, typename container_type::pointer>;
    using reference = std::conditional_t<std::is_const_v<C>, typename container_type::const_reference, typename container_type::reference>;

    container_ptr container; // beginning of array
    size_type idx; // offset into array

    constexpr CyclicBufferIterator() noexcept :
        container{ nullptr },
        idx{ 0 }
    {
    }

    constexpr explicit CyclicBufferIterator(C* c_ptr, size_type offset = 0) noexcept :
        container(c_ptr),
        idx(offset)
    {
        assert(idx <= container->size() && "Invalid offset! Offset is way beyond the array size!");
    }

    AOL_ATTRIB_NO_DISCARD constexpr reference operator*() const noexcept
    {
        assert(container && "Invalid operation! Cannot dereference nullptr array iterator!");
        assert(idx < container->size() && "Invalid operation! Cannot dereference out of range array iterator!");
        return (*container)[idx];
    }

    AOL_ATTRIB_NO_DISCARD constexpr pointer operator->() const noexcept
    {
        return std::addressof(this->operator*());
    }

    constexpr CyclicBufferIterator& operator++() noexcept
    {
        assert(container && "Invalid operation! cannot increment nullptr array iterator!");
        assert(idx < container->size() && "Invalid operation! Cannot increment array iterator past end!");
        ++idx;
        return *this;
    }

    constexpr CyclicBufferIterator operator++(int) noexcept
    {
        CyclicBufferIterator tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr CyclicBufferIterator& operator--() noexcept
    {
        assert(container && "Invalid operation! Cannot decrement nullptr array iterator!");
        assert(idx != 0 && "Invalid operation! Cannot decrement array iterator before begin!");
        --idx;
        return *this;
    }

    constexpr CyclicBufferIterator operator--(int) noexcept
    {
        CyclicBufferIterator tmp = *this;
        --*this;
        return tmp;
    }

    constexpr CyclicBufferIterator& operator+=(const difference_type offset) noexcept
    {
        AssertValidOffset(offset);
        idx += static_cast<size_type>(offset);
        return *this;
    }

    constexpr CyclicBufferIterator& operator-=(const difference_type offset) noexcept
    {
        return *this += -offset;
    }

    AOL_ATTRIB_NO_DISCARD constexpr difference_type operator-(const CyclicBufferIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return static_cast<difference_type>(idx - other.idx);
    }

    AOL_ATTRIB_NO_DISCARD constexpr reference operator[](const difference_type offset) const noexcept
    {
        return *(*this + offset);
    }

    AOL_ATTRIB_NO_DISCARD constexpr bool operator==(const CyclicBufferIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return idx == other.idx;
    }

    AOL_ATTRIB_NO_DISCARD constexpr std::strong_ordering operator<=>(const CyclicBufferIterator& other) const noexcept
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
            assert(idx >= size_type{ 0 } - static_cast<size_type>(offset) && "Invalid operation! Cannot seek array iterator before begin!");
        }

        if (offset > 0) {
            assert(container->size() - idx >= static_cast<size_type>(offset) && "Invalid operation! Cannot seek array iterator after end!");
        }
    }

    constexpr void AssertCompatibility(const CyclicBufferIterator& other) const noexcept
    {
        assert(container == other.container && "Invalid operation! Array iterators incompatible!");
    }
#else
    constexpr void AssertValidOffset(const difference_type offset) const noexcept
    {
        // empty function
    }

    constexpr void AssertCompatibility(const CyclicBufferIterator& other) const noexcept
    {
        // empty function
    }
#endif

public:
    AOL_ATTRIB_NO_DISCARD constexpr CyclicBufferIterator operator+(const difference_type offset) const noexcept
    {
        CyclicBufferIterator tmp = *this;
        tmp += offset;
        return tmp;
    }

    AOL_ATTRIB_NO_DISCARD constexpr CyclicBufferIterator operator-(const difference_type offset) const noexcept
    {
        CyclicBufferIterator tmp = *this;
        tmp -= offset;
        return tmp;
    }

    AOL_ATTRIB_NO_DISCARD friend constexpr CyclicBufferIterator operator+(const difference_type offset, CyclicBufferIterator next) noexcept
    {
        next += offset;
        return next;
    }
};

template<
    typename D,
    typename T,
    typename A,
    SizeT S
>
struct CyclicBufferBase
{
    static_assert(S == 0 || std::has_single_bit(S), "Fixed size must be a power of two!");

    using container_type = std::conditional_t<S == 0, AoL::Vector<T, A>, AoL::Array<T, S>>;

    using value_type = container_type::value_type;
    using size_type = SizeT;

    using pointer = container_type::pointer;
    using const_pointer = container_type::const_pointer;
    using reference = container_type::reference;
    using const_reference = container_type::const_reference;

    using iterator = CyclicBufferIterator<CyclicBufferBase<D, T, A, S>>;
    using const_iterator = CyclicBufferIterator<const CyclicBufferBase<D, T, A, S>>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    container_type container_obj;
    size_type mask;
    size_type head;
    size_type item_count;

    CyclicBufferBase() noexcept :
        container_obj{ },
        mask{ S > 0 ? S - 1 : 0},
        head{ 0 },
        item_count{ 0 }
    {}

    CyclicBufferBase(const CyclicBufferBase& other) noexcept = default;
    CyclicBufferBase& operator = (const CyclicBufferBase& other) noexcept = default;

    CyclicBufferBase(CyclicBufferBase&& other) noexcept :
        container_obj{ std::move(other.container_obj) },
        mask{ other.mask },
        head{ other.head },
        item_count{ other.item_count }
    {
        other.head = other.item_count = other.mask = static_cast<SizeT>(0);
    }

    CyclicBufferBase& operator = (CyclicBufferBase&& other) noexcept
    {
        container_obj = std::move(other.container_obj);
        mask = other.mask;
        head = other.head;
        item_count = other.item_count;
        other.head = other.item_count = other.mask = static_cast<SizeT>(0);
        return *this;
    }

    /**
    * @details Checks if the container is already full
    *
    * - Note that it will always be full if head > 0
    *
    * @returns bool true if full
    */
    AOL_ATTRIB_NO_DISCARD constexpr bool full() const noexcept
    {
        return item_count == this->capacity();
    }

    /**
    * @details Query for the maximum of items the vector can have
    *
    * - As a circular buffer, the capacity does not increase
    *
    * @returns SizeT maximum allowable items
    */
    AOL_ATTRIB_NO_DISCARD constexpr auto capacity() const noexcept
    {
        return container_obj.size();
    }

    /**
    * @details Checks if the container is empty
    *
    * @returns bool true if empty, otherwise false
    */
    AOL_ATTRIB_NO_DISCARD constexpr bool empty() const noexcept
    {
        return item_count == 0;
    }

    /**
    * @details Query for the current size of the buffer
    *
    * - If the size is already equal to capacity, the size will not increase anymore
    *
    * @returns SizeT current size
    */
    AOL_ATTRIB_NO_DISCARD constexpr auto size() const noexcept
    {
        return item_count;
    }

    /**
    * @details Add an element to the back
    *
    * - If the size is already at capacity, the new item will overwrite the oldest element
    *
    * @param new_item item to be added
    */
    template<typename U>
    constexpr void push_back(U&& new_item) noexcept
    {
        assert(mask > 0 && "No assigned item limit yet! Cannot add items!");

        static_cast<D*>(this)->push_back_impl(std::forward<U>(new_item));
        if (item_count == this->capacity())
        {
            head = (head + 1) & mask;
        }
        else
        {
            item_count++;
        }
    }

    /**
    * @details Add an element to the back
    *
    * - If the size is already at capacity, the new item will overwrite the oldest element
    *
    * @param args type construction arguments
    */
    template<typename...Args>
    constexpr reference emplace_back(Args&&...args) noexcept
    {
        assert(mask > 0 && "No assigned item limit yet! Cannot add items!");

        static_cast<D*>(this)->emplace_back_impl(std::forward<Args>(args)...);
        if (item_count == this->capacity())
        {
            head = (head + 1) & mask;
        }
        else
        {
            item_count++;
        }

        return (*this)[item_count - 1];
    }

    /*
    * @details Remove the front element
    *
    * - This process does not actually remove the element
    *   but just moves the head forward and decreases the item count
    */
    constexpr void pop_front() noexcept
    {
        assert(item_count > 0 && "Invalid operation! Cannot pop an empty container!");

        head = (head + 1) & mask;
        item_count--;
    }

    /**
    * @details Remove the back element
    *
    * - This process does not actually remove the element
    *   but just decreases the item count
    */
    constexpr void pop_back() noexcept
    {
        assert(item_count > 0 && "Invalid operation! Cannot pop an empty container!");

        item_count--;
    }

    /**
    * @details Clears the container
    *
    * - Like the description says, clears the container and its containing elements
    */
    constexpr void clear() noexcept
    {
        head = item_count = 0;
        if constexpr (requires (D d) { d.clear_impl(); })
        {
            static_cast<D*>(this)->clear_impl();
        }
    }

    /**
    * @details Access element
    *
    * - Asserts if idx is greater than or equal to item count of the vector
    */
    AOL_ATTRIB_NO_DISCARD constexpr T& operator[](size_t idx) noexcept
    {
        assert(idx < item_count && "Invalid operation! Input idx out of range!");
        return container_obj[(head + idx) & mask];
    }

    /**
    * @details Access element
    *
    * - Asserts if idx is greater than or equal item to count of the vector
    */
    AOL_ATTRIB_NO_DISCARD constexpr Traits::ConstRefOrCopyType<T> operator[](size_t idx) const noexcept
    {
        assert(idx < item_count && "Invalid operation! Input idx out of range!");
        return container_obj[(head + idx) & mask];
    }

    /**
    * @details Access the front element
    *
    * - The same as data[0]
    */
    AOL_ATTRIB_NO_DISCARD constexpr T& front() noexcept
    {
        assert(item_count > 0);
        return container_obj[head];
    }

    /**
    * @details Access the front element
    *
    * - The same as data[item_count - 1]
    */
    AOL_ATTRIB_NO_DISCARD constexpr T& back() noexcept
    {
        assert(item_count > 0);
        return container_obj[(head + item_count - 1) & mask];
    }

    /**
    * @details Access the front element
    *
    * - The same as data[0]
    */
    AOL_ATTRIB_NO_DISCARD constexpr T& front() const noexcept
    {
        assert(item_count > 0);
        return container_obj[head];
    }

    /**
    * @details Access the front element
    *
    * - The same as data[item_count - 1]
    */
    AOL_ATTRIB_NO_DISCARD constexpr T& back() const noexcept
    {
        assert(item_count > 0);
        return container_obj[(head + item_count - 1) & mask];
    }

    AOL_ATTRIB_NO_DISCARD constexpr T* data() noexcept
    {
        return container_obj.data();
    }

    AOL_ATTRIB_NO_DISCARD constexpr const T* data() const noexcept
    {
        return container_obj.data();
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto begin() noexcept
    {
        return iterator(this, 0);
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto begin() const noexcept
    {
        return iterator(this, 0);
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto cbegin() const noexcept
    {
        return const_iterator(this, 0);
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto end() noexcept
    {
        return iterator(this, item_count);
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto end() const noexcept
    {
        return iterator(this, item_count);
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto cend() const noexcept
    {
        return const_iterator(this, item_count);
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto rbegin() noexcept
    {
        return reverse_iterator(this->end());
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto rbegin() const noexcept
    {
        return const_reverse_iterator(this->cend());
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto crbegin() const noexcept
    {
        return const_reverse_iterator(this->cend());
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto rend() noexcept
    {
        return reverse_iterator(this->begin());
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto rend() const noexcept
    {
        return const_reverse_iterator(this->cbegin());
    }

    AOL_ATTRIB_NO_DISCARD constexpr auto crend() const noexcept
    {
        return const_reverse_iterator(this->cbegin());
    }
};

template<
    typename T,
    SizeT S
>
struct CyclicBufferFixed : CyclicBufferBase<CyclicBufferFixed<T, S>, T, void, S>
{
private:
    using Base = CyclicBufferBase<CyclicBufferFixed<T, S>, T, void, S>;

    friend Base;

public:
    static_assert(S > 0, "Size must be greater than 0!");

    using Base::Base;
    using Base::container_obj;
    using Base::mask;
    using Base::head;
    using Base::item_count;

private:
    template<typename U>
    constexpr void push_back_impl(U&& new_item) noexcept
    {
        container_obj[(head + item_count) & mask] = std::forward<U>(new_item);
    }

    template<typename... Args>
    constexpr void emplace_back_impl(Args&&... args) noexcept
    {
        container_obj[(head + item_count) & mask] = T(std::forward<Args>(args)...);
    }
};

template<
    typename T,
    typename A = DefaultAllocator<T>
>
struct CyclicBufferDynamic : CyclicBufferBase<CyclicBufferDynamic<T, A>, T, A, 0>
{
private:
    using Base = CyclicBufferBase<CyclicBufferDynamic<T, A>, T, A, 0>;

    friend Base;

public:
    using allocator_type = typename Base::container_type::allocator_type;

    using Base::Base;
    using Base::container_obj;
    using Base::mask;
    using Base::head;
    using Base::item_count;

    /**
    * @details Construct the circular vector
    *
    * - Note that item_limit must be a power of 2
    *
    * - This is done to optimize the random element access by using AND-bit instead of module
    *
    * @param item_limit maximum capacity of the vector in power of 2
    */
    explicit CyclicBufferDynamic(SizeT item_limit) noexcept :
        Base{ }
    {
        assert(std::has_single_bit(item_limit) && "Invalid limit! Must be power of 2!");

        mask = item_limit - 1;
        container_obj.reserve(item_limit);
    }

    /**
    * @details Increase the current capacity of the vector
    *
    * - If the vector has already circled, or overwritten at least one element
    *   The head will be reset at 0, otherwise
    * 
    * - The new item limit must be value of power of 2 and not less than the current capacity
    * 
    * @param new_item_limit new limit of the circular vector
    */
    constexpr void increase_capacity(SizeT new_item_limit) noexcept
    {
        assert(new_item_limit > 0 && "Invalid new limit! Must be greater than zero!");
        assert(std::has_single_bit(new_item_limit) && "Invalid new limit! Must be power of 2!");
        assert(new_item_limit > this->capacity() && "Invalid new limit! Must be larger than current capacity!");

        if (head > 0)
        {
            std::rotate(container_obj.begin(), container_obj.begin() + head, container_obj.end());
        }
        container_obj.reserve(new_item_limit);
        mask = new_item_limit - 1;
        head = 0;
    }

    /**
    * @details Decrease the current capacity of the vector
    * 
    * - Like increase_capacity but it decreases
    * 
    * - This will only retain the earliest elements up to the new limit
    * 
    * @param new_item_limit new limit of the circular vector
    */
    constexpr void decrease_capacity(SizeT new_item_limit) noexcept
    {
        assert(new_item_limit > 0 && "Invalid new limit! Must be greater than zero!");
        assert(std::has_single_bit(new_item_limit) && "Invalid new limit! Must be power of 2!");
        assert(new_item_limit < this->capacity() && "Invalid new limit! Must be smaller than current capacity!");

        if (head > 0)
        {
            std::rotate(container_obj.begin(), container_obj.begin() + head, container_obj.end());
        }
        if (new_item_limit < item_count)
        {
            item_count = new_item_limit;
        }
        container_obj.resize(new_item_limit);
        mask = new_item_limit - 1;
        head = 0;
    }

private:
    template<typename U>
    constexpr void push_back_impl(U&& new_item) noexcept
    {
        if (container_obj.size() == mask + 1)
        {
            container_obj[(head + item_count) & mask] = std::forward<U>(new_item);
        }
        else
        {
            container_obj.push_back(std::forward<U>(new_item));
        }
    }

    template<typename... Args>
    constexpr void emplace_back_impl(Args&&... args) noexcept
    {
        if (container_obj.size() == mask + 1)
        {
            container_obj[(head + item_count) & mask] = T(std::forward<Args>(args)...);
        }
        else
        {
            container_obj.emplace_back(std::forward<Args>(args)...);
        }
    }

    constexpr void clear_impl() noexcept
    {
        container_obj.clear();
    }
};

} // AoL::Internal namespace