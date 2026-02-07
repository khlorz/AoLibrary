#pragma once

/***************************************************************************************
* AoLibrary math
****************************************************************************************
* - Some math functions used by the library
* - Currently small, but will be adding more as I do more project
***************************************************************************************/

#include "config.h"

#include "types.h"
#include "traits.h"

namespace AoL
{

template<typename T>
constexpr T AddWithGuard(T value, T addend) noexcept
{
	if constexpr (std::is_floating_point_v<T>) {
		static_assert(Traits::AssertFalse<T>, "Type is float. Add them normally.");
	}
	else if constexpr (std::is_signed_v<T>) {
		if ((addend > 0) && (value > std::numeric_limits<T>::max() - addend))
			return std::numeric_limits<T>::max();
		if ((addend < 0) && (value < std::numeric_limits<T>::min() - addend))
			return std::numeric_limits<T>::min();
		return value + addend;
	}
	else if constexpr (std::is_unsigned_v<T>) {
		return value > std::numeric_limits<T>::max() - addend ? std::numeric_limits<T>::max() : value + addend;
	}
	else {
		static_assert(Traits::AssertFalse<T>, "Invalid type for addition!");
	}
}

template<typename T>
constexpr void AddInPlaceWithGuard(T& value, T addend) noexcept
{
	if constexpr (std::is_floating_point_v<T>) {
		static_assert(Traits::AssertFalse<T>, "Type is float. Add them normally.");
	}
	else if constexpr (std::is_signed_v<T>) {
		if ((addend > 0) && (value > std::numeric_limits<T>::max() - addend))
			value = std::numeric_limits<T>::max();
		if ((addend < 0) && (value < std::numeric_limits<T>::min() - addend))
			value = std::numeric_limits<T>::min();
		value += addend;
	}
	else if constexpr (std::is_unsigned_v<T>) {
		value = value > std::numeric_limits<T>::max() - addend ? std::numeric_limits<T>::max() : value + addend;
	}
	else {
		static_assert(Traits::AssertFalse<T>, "Invalid type for addition!");
	}
}

template<typename T>
constexpr T SubtractWithGuard(T value, T subtrahend) noexcept
{
	if constexpr (std::is_floating_point_v<T>) {
		static_assert(Traits::AssertFalse<T>, "Type is float. Subtract them normally.");
	}
	else if constexpr (std::is_signed_v<T>) {
		if ((subtrahend > 0) && (value < std::numeric_limits<T>::min() + subtrahend))
			return std::numeric_limits<T>::min();
		if ((subtrahend < 0) && (value > std::numeric_limits<T>::max() + subtrahend))
			return std::numeric_limits<T>::max();
		return value - subtrahend;
	}
	else if constexpr (std::is_unsigned_v<T>) {
		return value < subtrahend ? 0 : value - subtrahend;
	}
	else {
		static_assert(Traits::AssertFalse<T>, "Invalid type for subtraction!");
	}
}

template<typename T>
constexpr void SubtractInPlaceWithGuard(T& value, T subtrahend) noexcept
{
	if constexpr (std::is_floating_point_v<T>) {
		static_assert(Traits::AssertFalse<T>, "Type is float. Subtract them normally.");
	}
	else if constexpr (std::is_signed_v<T>) {
		if ((subtrahend > 0) && (value < std::numeric_limits<T>::min() + subtrahend))
			value = std::numeric_limits<T>::min();
		if ((subtrahend < 0) && (value > std::numeric_limits<T>::max() + subtrahend))
			value = std::numeric_limits<T>::max();
		value -= subtrahend;
	}
	else if constexpr (std::is_unsigned_v<T>) {
		value = value < subtrahend ? 0 : value - subtrahend;
	}
	else {
		static_assert(Traits::AssertFalse<T>, "Invalid type for subtraction!");
	}
}

} // namespace AoL