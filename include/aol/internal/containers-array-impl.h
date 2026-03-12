#pragma once

/*************************************************
* [Containers] Array implementations
*************************************************/

namespace AoL
{

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
union NamedArray2
{
private:
	static constexpr SizeT ArrSize = 2;

public:
	static_assert(std::is_trivial_v<T>, "NamedArray2 requires T to be trivial");
	static_assert(std::is_standard_layout_v<T>, "NamedArray2 requires T to be standard-layout");

	T data_arr[ArrSize];
	struct
	{
		T x;
		T y;
	};

	constexpr NamedArray2() noexcept(std::is_nothrow_default_constructible_v<T>)
		: data_arr{}
	{
	}

	constexpr NamedArray2(const T& x_, const T& y_) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: data_arr{ x_, y_ }
	{
	}

	T& operator [] (SizeT idx) noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}

	Traits::ConstRefOrCopyType<T> operator [] (SizeT idx) const noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
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
union NamedArray3
{
private:
	static constexpr SizeT ArrSize = 3;

public:
	static_assert(std::is_trivial_v<T>, "NamedArray3 requires T to be trivial");
	static_assert(std::is_standard_layout_v<T>, "NamedArray3 requires T to be standard-layout");

	T data_arr[ArrSize];
	struct
	{
		T x;
		T y;
		T z;
	};

	constexpr NamedArray3() noexcept(std::is_nothrow_default_constructible_v<T>)
		: data_arr{} {
	}

	constexpr NamedArray3(Traits::ConstRefOrCopyType<T> x_, Traits::ConstRefOrCopyType<T> y_, Traits::ConstRefOrCopyType<T> z_) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: data_arr{ x_, y_, z_ } {
	}

	T& operator [] (SizeT idx) noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}

	Traits::ConstRefOrCopyType<T> operator [] (SizeT idx) const noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
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
union NamedArray4
{
private:
	static constexpr SizeT ArrSize = 4;

public:
	static_assert(std::is_trivial_v<T>, "NamedArray4 requires T to be trivial");
	static_assert(std::is_standard_layout_v<T>, "NamedArray4 requires T to be standard-layout");

	T data_arr[ArrSize];
	struct
	{
		T x;
		T y;
		T z;
		T w;
	};

	constexpr NamedArray4() noexcept(std::is_nothrow_default_constructible_v<T>)
		: data_arr{}
	{
	}

	constexpr NamedArray4(Traits::ConstRefOrCopyType<T> x_, Traits::ConstRefOrCopyType<T> y_, Traits::ConstRefOrCopyType<T> z_, Traits::ConstRefOrCopyType<T> w_) noexcept(std::is_nothrow_copy_constructible_v<T>)
		: data_arr{ x_, y_, z_, w_ }
	{
	}

	T& operator [] (SizeT idx) noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}

	Traits::ConstRefOrCopyType<T> operator [] (SizeT idx) const noexcept
	{
		assert(idx < ArrSize);
		return data_arr[idx];
	}
};

} // AoL namespace


// containers-array-impl.h EOF