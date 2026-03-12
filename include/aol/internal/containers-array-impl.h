#pragma once

/*************************************************
* [Containers] Array implementations
*************************************************/

namespace AoL
{

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

	using pointer = T*;
	using reference = T&;

#if defined(NDEBUG)
    pointer ptr;

    constexpr ArrayIterator() noexcept :
        ptr{ nullptr }
    {}

    constexpr explicit ArrayIterator(pointer arr_ptr, size_t offset = 0) noexcept :
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
    size_t idx; // offset into array

    constexpr ArrayIterator() noexcept :
        ptr{nullptr},
        idx{0}
    {}

    constexpr explicit ArrayIterator(pointer arr_ptr, size_t offset = 0) noexcept : 
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
        idx += static_cast<size_t>(offset);
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
            assert(idx >= size_t{ 0 } - static_cast<size_t>(offset) && "Invalid operation! Cannot seek array iterator before begin!");
        }

        if (offset > 0) {
            assert(Size - idx >= static_cast<size_t>(offset) && "Invalid operation! Cannot seek array iterator after end!");
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

} // AoL namespace


// containers-array-impl.h EOF