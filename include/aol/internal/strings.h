#pragma once

/***************************************************************************************
* AoLibrary strings
****************************************************************************************
* - The main string type used by the library
* - This string is different from std::string due to custom allocators
***************************************************************************************/

#include "config.h"

#include "types.h"
#include "allocators.h"
#include "traits.h"

#include "stb/stb_sprintf.h"
#include "fmt/format.h"
#include "absl/strings/str_cat.h"

#include <string>
#include <cstring>
#include <string_view>

namespace AoL
{

namespace Internal
{

template<typename T>
struct ValidFormatArgument : std::false_type {};
template<>
struct ValidFormatArgument<int> : std::true_type {};
template<>
struct ValidFormatArgument<unsigned> : std::true_type {};
template<>
struct ValidFormatArgument<long> : std::true_type {};
template<>
struct ValidFormatArgument<unsigned long> : std::true_type {};
template<>
struct ValidFormatArgument<long long> : std::true_type {};
template<>
struct ValidFormatArgument<unsigned long long> : std::true_type {};
template<>
struct ValidFormatArgument<double> : std::true_type {};
template<>
struct ValidFormatArgument<const char*> : std::true_type {};
template<>
struct ValidFormatArgument<char*> : std::true_type {};
template<>
struct ValidFormatArgument<void*> : std::true_type {};
template<>
struct ValidFormatArgument<const void*> : std::true_type {};

template<typename T>
inline constexpr bool IsValidFormatArgument = ValidFormatArgument<std::decay_t<T>>::value;

}

/**
* @details Basic string of the library
*
* - It's also just a 'using' wrapper for std::basic_string<T, Traits, Allocator>

* - For this string, the allocator can be changed depending on use case
*
* @tparam A allocator type (default = Internal::DefaultStringAllocator)
*/
using String = std::basic_string<char, std::char_traits<char>, Internal::DefaultStringAllocator>;

/**
* @Details String but for pool allocators
*
* - Specialize string used with pool/memory resources
*
* @tparam A allocator type (default = Internal::DefaultStringPoolAllocator)
*/
using StringPool = std::basic_string<char, std::char_traits<char>, Internal::DefaultStringPoolAllocator>;

/*
* @details AoL sprintf
* 
* - This is a wrapper for stb's sprintf implementation
* 
* - Unsafe but faster
* 
* - For safe operations, use StrFormat... functions
* 
* @param buf - Char buffer
* @param fmt - Print format
* @param ts - Format arguments
* @tparam Ts - Format types
* @returns int: End of buffer position
*/
template<typename... Ts>
constexpr auto StrPrintF(char* buf, const char* fmt, Ts&&... ts) noexcept -> int
{
	static_assert(Internal::IsValidFormatArgument<Ts...>, "Invalid format type!");
	return stbsp_sprintf(buf, fmt, std::forward<Ts>(ts)...);
}

/*
* @details AoL snprintf_s
*
* - This is a wrapper for stb's snprintf implementation
*
* - Safe in the sense that it wouldn't write beyond the buffer capacity
*
* - Still unsafe for mismatch format arguments
*
* - For safe operations, use StrFormat... functions
*
* @param buf Char buffer
* @param size Char size, this includes the '\0' or NULL terminator
* @param fmt Print format
* @param ts Format arguments
* @tparam Ts Format types
* @returns int: End of buffer position
*/
template<typename... Ts>
constexpr auto StrPrintF(char* buf, SizeT capacity, const char* fmt, Ts&&... ts) noexcept -> int
{
	static_assert(Internal::IsValidFormatArgument<Ts...>, "Invalid format type!");
	return stbsp_snprintf(buf, capacity, fmt, std::forward<Ts>(ts)...);
}

/*
* @details AoL format
* 
* - This is a wrapper for fmt's format
* 
* - This can be quicker and safer than StrPrintF if dealing with std::strings rather than c strings
* 
* @param fmt Print format
* @param ts Format arguments
* @tparam Ts Format types
* @returns AoL::String: Formatted string
*/
template<typename... Ts>
constexpr auto StrFormat(const char* fmt, Ts&&... ts) -> AoL::String
{
	return fmt::format(fmt, std::forward<Ts>(ts)...);
}

/**
* @details AoL string concatenation
* 
* - This is a wrapper for Abseil's StrCat
* 
* - This can be quicker than StrFormat in some cases
* 
* @param ts Format arguments
* @tparam Ts Fromat types
* @returns AoL::String: Resulting concatenated values
*/
template<typename... Ts>
constexpr auto StrConcat(Ts&&... ts) -> AoL::String
{
	return absl::StrCat(std::forward<Ts>(ts)...);
}


}