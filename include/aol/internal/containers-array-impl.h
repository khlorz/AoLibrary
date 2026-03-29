#pragma once

/*************************************************
* [Containers] Array implementations
*************************************************/

namespace AoL
{

/****************************************
* General Array
****************************************/

namespace Internal
{

template<
	typename T,
	SizeT Size
>
struct ArrayIterator
{
	static_assert(Size != 0, "Array size should always be greater than zero!");

	using iterator_concept = std::contiguous_iterator_tag;
	using iterator_category = std::random_access_iterator_tag;

	using value_type = T;
	using difference_type = PtrDiff;
    using size_type = SizeT;

	using pointer = T*;
	using reference = T&;

#if defined(NDEBUG)
    pointer ptr;

    constexpr ArrayIterator() noexcept :
        ptr{ nullptr }
    {}

    constexpr explicit ArrayIterator(pointer arr_ptr, size_type offset = 0) noexcept :
        ptr{ arr_ptr + offset }
    {}

    AOL_NO_DISCARD constexpr reference operator*() const noexcept
    {
        return *ptr;
    }

    AOL_NO_DISCARD constexpr pointer operator->() const noexcept
    {
        return ptr;
    }

    constexpr ArrayIterator& operator++() noexcept
    {
        ++ptr;
        return *this;
    }

    constexpr ArrayIterator operator++(int) noexcept
    {
        ArrayIterator tmp = *this;
        ++ptr;
        return tmp;
    }

    constexpr ArrayIterator& operator--() noexcept
    {
        --ptr;
        return *this;
    }

    constexpr ArrayIterator operator--(int) noexcept
    {
        ArrayIterator tmp = *this;
        --ptr;
        return tmp;
    }

    constexpr ArrayIterator& operator+=(const difference_type offset) noexcept
    {
        ptr += offset;
        return *this;
    }

    constexpr ArrayIterator& operator-=(const difference_type offset) noexcept
    {
        ptr -= offset;
        return *this;
    }

    AOL_NO_DISCARD constexpr difference_type operator-(const ArrayIterator& other) const noexcept
    {
        return ptr - other.ptr;
    }

    AOL_NO_DISCARD constexpr reference operator[](const difference_type offset) const noexcept
    {
        return ptr[offset];
    }

    AOL_NO_DISCARD constexpr bool operator==(const ArrayIterator& other) const noexcept
    {
        return ptr == other.ptr;
    }

    AOL_NO_DISCARD constexpr std::strong_ordering operator<=>(const ArrayIterator& other) const noexcept
    {
        return ptr <=> other.ptr;
    }

#else
    pointer ptr; // beginning of array
    size_type idx; // offset into array

    constexpr ArrayIterator() noexcept :
        ptr{nullptr},
        idx{0}
    {}

    constexpr explicit ArrayIterator(pointer arr_ptr, size_type offset = 0) noexcept :
        ptr(arr_ptr),
        idx(offset)
    {
        assert(idx <= Size && "Invalid offset! Offset is way beyond the array size!");
    }

    AOL_NO_DISCARD constexpr reference operator*() const noexcept
    {
        return *operator->();
    }

    AOL_NO_DISCARD constexpr pointer operator->() const noexcept
    {
        assert(ptr && "Invalid operation! Cannot dereference nullptr array iterator!");
        assert(idx < Size && "Invalid operation! Cannot dereference out of range array iterator!");
        return ptr + idx;
    }

    constexpr ArrayIterator& operator++() noexcept
    {
        assert(ptr && "Invalid operation! cannot increment nullptr array iterator!");
        assert(idx < Size && "Invalid operation! Cannot increment array iterator past end!");
        ++idx;
        return *this;
    }

    constexpr ArrayIterator operator++(int) noexcept
    {
        ArrayIterator tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr ArrayIterator& operator--() noexcept
    {
        assert(ptr && "Invalid operation! Cannot decrement nullptr array iterator!");
        assert(idx != 0 && "Invalid operation! Cannot decrement array iterator before begin!");
        --idx;
        return *this;
    }

    constexpr ArrayIterator operator--(int) noexcept
    {
        ArrayIterator tmp = *this;
        --*this;
        return tmp;
    }

    constexpr ArrayIterator& operator+=(const difference_type offset) noexcept
    {
        AssertValidOffset(offset);
        idx += static_cast<size_type>(offset);
        return *this;
    }

    constexpr ArrayIterator& operator-=(const difference_type offset) noexcept
    {
        return *this += -offset;
    }

    AOL_NO_DISCARD constexpr difference_type operator-(const ArrayIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return static_cast<difference_type>(idx - other.idx);
    }

    AOL_NO_DISCARD constexpr reference operator[](const difference_type offset) const noexcept
    {
        return *(*this + offset);
    }

    AOL_NO_DISCARD constexpr bool operator==(const ArrayIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return idx == other.idx;
    }

    AOL_NO_DISCARD constexpr std::strong_ordering operator<=>(const ArrayIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return idx <=> other.idx;
    }

    constexpr void AssertValidOffset(const difference_type offset) const noexcept
    {
        if (offset != 0) {
            assert(ptr && "Invalid operation! Cannot seek nullptr array iterator!");
        }

        if (offset < 0) {
            assert(idx >= size_type{ 0 } - static_cast<size_type>(offset) && "Invalid operation! Cannot seek array iterator before begin!");
        }

        if (offset > 0) {
            assert(Size - idx >= static_cast<size_type>(offset) && "Invalid operation! Cannot seek array iterator after end!");
        }
    }

    constexpr void AssertCompatibility(const ArrayIterator& other) const noexcept
    {
        assert(ptr == other.ptr && "Invalid operation! Array iterators incompatible!");
    }

public:
#endif

    AOL_NO_DISCARD constexpr ArrayIterator operator+(const difference_type offset) const noexcept
    {
        ArrayIterator tmp = *this;
        tmp += offset;
        return tmp;
    }

    AOL_NO_DISCARD constexpr ArrayIterator operator-(const difference_type offset) const noexcept
    {
        ArrayIterator tmp = *this;
        tmp -= offset;
        return tmp;
    }

    AOL_NO_DISCARD friend constexpr ArrayIterator operator+(const difference_type offset, ArrayIterator next) noexcept
    {
        next += offset;
        return next;
    }
};

template<
	SizeT S
>
struct AOL_EMPTY_BASE_OPTIMIZATION NamedArraySize
{
	static constexpr SizeT ArrSize = S;
};

template<
	typename Derived,
	typename T,
    SizeT S
>
struct AOL_EMPTY_BASE_OPTIMIZATION NamedArrayBase : public NamedArraySize<S>
{
	static_assert(std::is_trivial_v<T>, "NamedArray2 requires T to be trivial");
	static_assert(std::is_standard_layout_v<T>, "NamedArray2 requires T to be standard-layout");

	using value_type = T;
	using size_type = SizeT;
	using reference = T&;
	using const_reference = Traits::ConstRefOrCopyType<T>;

	using iterator = Internal::ArrayIterator<T, S>;
	using const_iterator = Internal::ArrayIterator<const T, S>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	AOL_NO_DISCARD constexpr reference operator [] (SizeT idx) noexcept
	{
		assert(idx < S && "Invalid array access!");
		return static_cast<Derived*>(this)->data_arr[idx];
	}

    AOL_NO_DISCARD constexpr const_reference operator [] (SizeT idx) const noexcept
	{
		assert(idx < S && "Invalid array access!");
		return static_cast<Derived*>(this)->data_arr[idx];
	}

    AOL_NO_DISCARD constexpr auto begin() noexcept
    {
        return iterator{ static_cast<Derived*>(this)->data_arr };
    }

    AOL_NO_DISCARD constexpr auto begin() const noexcept
    {
        return const_iterator{ static_cast<Derived*>(this)->data_arr };
    }

    AOL_NO_DISCARD constexpr auto cbegin() const noexcept
    {
        return const_iterator{ static_cast<Derived*>(this)->data_arr };
    }

    AOL_NO_DISCARD constexpr auto end() noexcept
    {
        return iterator{ static_cast<Derived*>(this)->data_arr, S };
    }

    AOL_NO_DISCARD constexpr auto end() const noexcept
    {
        return const_iterator{ static_cast<Derived*>(this)->data_arr, S };
    }

    AOL_NO_DISCARD constexpr auto cend() const noexcept
    {
        return const_iterator{ static_cast<Derived*>(this)->data_arr, S };
    }

    AOL_NO_DISCARD constexpr auto rbegin() noexcept
    {
        return reverse_iterator{ this->end() };
    }

    AOL_NO_DISCARD constexpr auto rbegin() const noexcept
    {
        return const_reverse_iterator{ this->cend };
    }

    AOL_NO_DISCARD constexpr auto crbegin() const noexcept
    {
        return const_reverse_iterator{ this->cend };
    }

    AOL_NO_DISCARD constexpr auto rend() noexcept
    {
        return reverse_iterator{ this->begin() };
    }

    AOL_NO_DISCARD constexpr auto rend() const noexcept
    {
        return const_reverse_iterator{ this->cbegin() };
    }

    AOL_NO_DISCARD constexpr auto crend() const noexcept
    {
        return const_reverse_iterator{ this->cbegin() };
    }

    AOL_NO_DISCARD constexpr auto size() const noexcept
    {
        return S;
    }
};

} // Internal namespace

/**
* Array of two elements that can be access through x and y, respectively
*
* - This is basically a union of an array and two objects of the same types
*
* @tparam T Element type
*/
template<
	typename T
>
struct NamedArray2 : public Internal::NamedArrayBase<NamedArray2<T>, T, 2>
{
private:
    using Base = Internal::NamedArrayBase<NamedArray2<T>, T, 2>;

public:
	union
	{
		T data_arr[Base::ArrSize];
		struct
		{
			T x;
			T y;
		};
	};

	constexpr NamedArray2() noexcept(std::is_nothrow_default_constructible_v<T>)
		: data_arr{}
	{
	}

    explicit constexpr NamedArray2(Traits::ConstRefOrCopyType<T> value) noexcept(std::is_nothrow_copy_constructible_v<T>) :
        data_arr{ value, value }
    {
    }

    explicit constexpr NamedArray2(Traits::ConstRefOrCopyType<T> x_, Traits::ConstRefOrCopyType<T> y_) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: data_arr{ x_, y_ }
	{
	}
};

/**
* Array of three elements that can be access through x, y, and z, respectively
*
* - This is basically a union of an array and three objects of the same types
*
* @tparam T Element type
*/
template<
	typename T
>
struct NamedArray3 : public Internal::NamedArrayBase<NamedArray3<T>, T, 3>
{
private:
    using Base = Internal::NamedArrayBase<NamedArray3<T>, T, 3>;

public:
    union
    {
	    T data_arr[Base::ArrSize];
	    struct
	    {
		    T x;
		    T y;
		    T z;
	    };
    };

	constexpr NamedArray3() noexcept(std::is_nothrow_default_constructible_v<T>)
		: data_arr{}
    {
	}

    explicit constexpr NamedArray3(Traits::ConstRefOrCopyType<T> value) noexcept(std::is_nothrow_copy_constructible_v<T>) :
        data_arr{ value, value, value }
    {
    }

    explicit constexpr NamedArray3(Traits::ConstRefOrCopyType<T> x_, Traits::ConstRefOrCopyType<T> y_, Traits::ConstRefOrCopyType<T> z_) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: data_arr{ x_, y_, z_ }
    {
	}
};

/**
* Array of four elements that can be access through x, y, z, and w, respectively
*
* - This is basically a union of an array and four objects of the same types
*
* @tparam T Element type
*/
template<
	typename T
>
struct NamedArray4 : public Internal::NamedArrayBase<NamedArray4<T>, T, 4>
{
private:
    using Base = Internal::NamedArrayBase<NamedArray4<T>, T, 4>;

public:
    union
    {
	    T data_arr[Base::ArrSize];
	    struct
	    {
		    T x;
		    T y;
		    T z;
		    T w;
	    };
    };

	constexpr NamedArray4() noexcept(std::is_nothrow_default_constructible_v<T>)
		: data_arr{}
	{
	}

    explicit constexpr NamedArray4(Traits::ConstRefOrCopyType<T> value) noexcept(std::is_nothrow_copy_constructible_v<T>) :
        data_arr{ value, value, value, value }
    {
    }

    explicit constexpr NamedArray4(Traits::ConstRefOrCopyType<T> x_, Traits::ConstRefOrCopyType<T> y_, Traits::ConstRefOrCopyType<T> z_, Traits::ConstRefOrCopyType<T> w_) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: data_arr{ x_, y_, z_, w_ }
	{
	}
};


/****************************************
* Circular Array
****************************************/

namespace Internal
{

/**
* @details ArrayCircularEx iterator
*
* - Iterate made solely for ArrayCircular
*/
template<
	typename C
>
struct ArrayCircularExIterator
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

    constexpr ArrayCircularExIterator() noexcept :
        container{ nullptr },
        idx{ 0 }
    {
    }

    constexpr explicit ArrayCircularExIterator(C* c_ptr, size_type offset = 0) noexcept :
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
        return std::addressof(this->operator*());
    }

    constexpr ArrayCircularExIterator& operator++() noexcept
    {
        assert(container && "Invalid operation! cannot increment nullptr array iterator!");
        assert(idx < container->size() && "Invalid operation! Cannot increment array iterator past end!");
        ++idx;
        return *this;
    }

    constexpr ArrayCircularExIterator operator++(int) noexcept
    {
        ArrayCircularExIterator tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr ArrayCircularExIterator& operator--() noexcept
    {
        assert(container && "Invalid operation! Cannot decrement nullptr array iterator!");
        assert(idx != 0 && "Invalid operation! Cannot decrement array iterator before begin!");
        --idx;
        return *this;
    }

    constexpr ArrayCircularExIterator operator--(int) noexcept
    {
        ArrayCircularExIterator tmp = *this;
        --*this;
        return tmp;
    }

    constexpr ArrayCircularExIterator& operator+=(const difference_type offset) noexcept
    {
        AssertValidOffset(offset);
        idx += static_cast<size_type>(offset);
        return *this;
    }

    constexpr ArrayCircularExIterator& operator-=(const difference_type offset) noexcept
    {
        return *this += -offset;
    }

    AOL_NO_DISCARD constexpr difference_type operator-(const ArrayCircularExIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return static_cast<difference_type>(idx - other.idx);
    }

    AOL_NO_DISCARD constexpr reference operator[](const difference_type offset) const noexcept
    {
        return *(*this + offset);
    }

    AOL_NO_DISCARD constexpr bool operator==(const ArrayCircularExIterator& other) const noexcept
    {
        AssertCompatibility(other);
        return idx == other.idx;
    }

    AOL_NO_DISCARD constexpr std::strong_ordering operator<=>(const ArrayCircularExIterator& other) const noexcept
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

    constexpr void AssertCompatibility(const ArrayCircularExIterator& other) const noexcept
    {
        assert(container == other.container && "Invalid operation! Array iterators incompatible!");
    }
#else
    constexpr void AssertValidOffset(const difference_type offset) const noexcept
    {
        // empty function
    }

    constexpr void AssertCompatibility(const ArrayCircularExIterator& other) const noexcept
    {
        // empty function
    }
#endif

public:
    AOL_NO_DISCARD constexpr ArrayCircularExIterator operator+(const difference_type offset) const noexcept
    {
        ArrayCircularExIterator tmp = *this;
        tmp += offset;
        return tmp;
    }

    AOL_NO_DISCARD constexpr ArrayCircularExIterator operator-(const difference_type offset) const noexcept
    {
        ArrayCircularExIterator tmp = *this;
        tmp -= offset;
        return tmp;
    }

    AOL_NO_DISCARD friend constexpr ArrayCircularExIterator operator+(const difference_type offset, ArrayCircularExIterator next) noexcept
    {
        next += offset;
        return next;
    }
};

template<
    typename T,
    SizeT S
>
struct ArrayCircularEx
{
    static_assert(std::has_single_bit(S), "Invalid size! Must be a power of two!");

    using container_type = std::array<T, S>;

    using value_type = container_type::value_type;
    using size_type = SizeT;

    using pointer = container_type::pointer;
    using const_pointer = container_type::const_pointer;
    using reference = container_type::reference;
    using const_reference = container_type::const_reference;

    using iterator = Internal::ArrayCircularExIterator<ArrayCircularEx<T, S>>;
    using const_iterator = ArrayCircularExIterator<const ArrayCircularEx<T, S>>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    container_type container_obj;
    size_type mask;
    size_type head;
    size_type item_count;

    ArrayCircularEx() noexcept :
        container_obj{ },
        mask{ S - 1 },
        head{ 0 },
        item_count{ 0 }
    {
    }

    ArrayCircularEx(const ArrayCircularEx& other) noexcept = default;
    ArrayCircularEx& operator = (const ArrayCircularEx& other) noexcept = default;

    ArrayCircularEx(ArrayCircularEx&& other) noexcept :
        container_obj{ std::move(other.container_obj) },
        mask{ other.mask },
        head{ other.head },
        item_count{ other.item_count }
    {
        other.head = other.item_count = other.mask = static_cast<size_type>(0);
    }

    ArrayCircularEx& operator = (ArrayCircularEx&& other) noexcept
    {
        container_obj = std::move(other.container_obj);
        mask = other.mask;
        head = other.head;
        item_count = other.item_count;
        other.head = other.item_count = other.mask = static_cast<size_type>(0);
        return *this;
    }

    /**
    * @details Checks if the container is already full
    *
    * - Note that it will always be full if head > 0
    *
    * @returns bool true if full
    */
    AOL_NO_DISCARD constexpr bool full() const noexcept
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
    AOL_NO_DISCARD constexpr auto capacity() const noexcept
    {
        return container_obj.size();
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

        container_obj[(head + item_count) & mask] = std::forward<U>(new_item);
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

        container_obj[(head + item_count) & mask] = T(std::forward<Args>(args)...);
        if (item_count == this->capacity())
        {
            head = (head + 1) & mask;
        }
        else
        {
            item_count++;
        }
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
    }

    /**
    * @details Access element
    *
    * - Asserts if idx is greater than or equal to item count of the vector
    */
    AOL_NO_DISCARD constexpr T& operator[](size_t idx) noexcept
    {
        assert(idx < item_count && "Invalid operation! Input idx out of range!");
        return container_obj[(head + idx) & mask];
    }

    /**
    * @details Access element
    *
    * - Asserts if idx is greater than or equal item to count of the vector
    */
    AOL_NO_DISCARD constexpr Traits::ConstRefOrCopyType<T> operator[](size_t idx) const noexcept
    {
        assert(idx < item_count && "Invalid operation! Input idx out of range!");
        return container_obj[(head + idx) & mask];
    }

    /**
    * @details Access the front element
    *
    * - The same as data[0]
    */
    AOL_NO_DISCARD constexpr T& front() noexcept
    {
        assert(item_count > 0);
        return container_obj[head];
    }

    /**
    * @details Access the front element
    *
    * - The same as data[item_count - 1]
    */
    AOL_NO_DISCARD constexpr T& back() noexcept
    {
        assert(item_count > 0);
        return container_obj[(head + item_count - 1) & mask];
    }

    /**
    * @details Access the front element
    *
    * - The same as data[0]
    */
    AOL_NO_DISCARD constexpr T& front() const noexcept
    {
        assert(item_count > 0);
        return container_obj[head];
    }

    /**
    * @details Access the front element
    *
    * - The same as data[item_count - 1]
    */
    AOL_NO_DISCARD constexpr T& back() const noexcept
    {
        assert(item_count > 0);
        return container_obj[(head + item_count - 1) & mask];
    }

    AOL_NO_DISCARD constexpr auto begin() noexcept
    {
        return iterator(this, 0);
    }

    AOL_NO_DISCARD constexpr auto begin() const noexcept
    {
        return iterator(this, 0);
    }

    AOL_NO_DISCARD constexpr auto cbegin() const noexcept
    {
        return const_iterator(this, 0);
    }

    AOL_NO_DISCARD constexpr auto end() noexcept
    {
        return iterator(this, item_count);
    }

    AOL_NO_DISCARD constexpr auto end() const noexcept
    {
        return iterator(this, item_count);
    }

    AOL_NO_DISCARD constexpr auto cend() const noexcept
    {
        return const_iterator(this, item_count);
    }

    AOL_NO_DISCARD constexpr auto rbegin() noexcept
    {
        return reverse_iterator(this->end());
    }

    AOL_NO_DISCARD constexpr auto rbegin() const noexcept
    {
        return const_reverse_iterator(this->cend());
    }

    AOL_NO_DISCARD constexpr auto crbegin() const noexcept
    {
        return const_reverse_iterator(this->cend());
    }

    AOL_NO_DISCARD constexpr auto rend() noexcept
    {
        return reverse_iterator(this->begin());
    }

    AOL_NO_DISCARD constexpr auto rend() const noexcept
    {
        return const_reverse_iterator(this->cbegin());
    }

    AOL_NO_DISCARD constexpr auto crend() const noexcept
    {
        return const_reverse_iterator(this->cbegin());
    }

    AOL_NO_DISCARD constexpr bool empty() const noexcept
    {
        return item_count == 0;
    }

    AOL_NO_DISCARD constexpr auto size() const noexcept
    {
        return item_count;
    }
};

} // Internal namespace

} // AoL namespace


// containers-array-impl.h EOF