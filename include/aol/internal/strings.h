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
#include "absl/strings/str_format.h"
#include "absl/strings/match.h"
#include "absl/strings/str_split.h"

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


/***************************************
* Declarations / usings
***************************************/

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
* @details String but for pool allocators
*
* - Specialize string used with pool/memory resources
*
* @tparam A allocator type (default = Internal::DefaultStringPoolAllocator)
*/
using StringPool = std::basic_string<char, std::char_traits<char>, Internal::DefaultStringPoolAllocator>;

/**
* @details View object for string values
* 
* - This is a wrapper for std::string
* 
* - Usable on std::strings, c-strings, and custom types that abides by std::string standard
*/
using StringView = std::string_view;


/***************************************
* Definitions
***************************************/

inline constexpr auto StrNoPos = AoL::String::npos;


/***************************************
* Definitions
***************************************/

/*
* @details AoL sprintf
* 
* - This is a wrapper for stb's sprintf implementation
* 
* - Best used with c-strings
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
* - Best used with c-strings
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
* - If you want/need readability, use this function
*
* - However, for things that need fast string output, use StrConcat
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
* - Create a string from an abritrary number of values (e.g. c-strings, std::string, int, double, etc.)
* 
* - For things that need fast string output, use this
* 
* - If you want readability, use StrFormat instead
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

/**
* @details AoL string appending
* 
* - This is a wrapper for Abseil's StrAppend
* 
* - Append the given string an abritrary number of values (e.g. c-strings, std::string, int, double, etc.)
* 
* - Use this to append the strings effectively
* 
* @param str The output string to be appended
* @param ts The values to be appended to the output string
*/
template<typename... Ts>
constexpr auto StrAppend(AoL::String& str, Ts&&... ts) -> void
{
	return absl::StrAppend(&str, std::forward<Ts>(ts)...);
}

/**
* @details AoL string appending
*
* - This is a wrapper for Abseil's StrAppendFormat
*
* - Append the given string an abritrary number of values with a given format
*
* - Use this to append the strings effectively
*
* @param str The output string to be appended
* @param fmt The format of the appending string
* @param ts The values to be appended to the output string
* @returns AoL::String: The resulting string. Used for chain appending purposes.
*/
template<typename... Ts>
constexpr auto StrAppendF(AoL::String& str, const char* fmt, Ts&&... ts) -> AoL::String&
{
	return absl::StrAppendFormat(&str, fmt, std::forward<Ts>(ts)...);
}

/**
* @details AoL string searching
*
* - This is a wrapper for Abseil's StrContain
*
* - Finds the given value from the given string
*
* -- Depending on the implementation, absl StrContains can be faster than the given find() of std::string
*
* @param haystack  source string
* @param needle find value
* @param offset starting offset for the haystack
* @returns SizeT: found position
*/
constexpr auto StrFind(AoL::StringView haystack, AoL::StringView needle, SizeT offset = 0) noexcept -> SizeT
{
	return haystack.find(needle, offset);
}

/**
* @details AoL string searching
*
* - This is a wrapper for Abseil's StrContain
*
* - Finds the given value from the given string
*
* -- Depending on the implementation, absl StrContains can be faster than the given find() of std::string
*
* @param haystack  source string
* @param needle find value
* @param offset starting offset for the haystack
* @returns SizeT: found position
*/
constexpr auto StrFind(AoL::StringView haystack, char needle, SizeT offset = 0) noexcept -> SizeT
{
	return haystack.find(needle, offset);
}

/**
* @details AoL string searching
*
* - This is a wrapper for Abseil's StrContain
*
* - Finds the given value from the given string
*
* -- Depending on the implementation, absl StrContains can be faster than the given find() of std::string
*
* @param haystack  source string
* @param needle find value
* @param offset starting offset for the haystack
* @param count needle size
* @returns SizeT: found position
*/
constexpr auto StrFind(AoL::StringView haystack, const char* const needle, SizeT offset, SizeT count) noexcept -> SizeT
{
	return haystack.find(needle, offset, count);
}

/**
* @details AoL string searching
*
* - This is a wrapper for Abseil's StrContain
*
* - Query on whether the value exists on the string
*
* -- Depending on the implementation, absl StrContains can be faster than the given find() of std::string
*
* @param haystack  source string
* @param needle find value
* @param offset starting offset for the haystack
* @returns bool: true = found, false = not found 
*/
constexpr auto StrContains(AoL::StringView haystack, AoL::StringView needle, SizeT offset = 0) noexcept -> bool
{
	return haystack.find(needle, offset) != haystack.npos;
}

/**
* @details AoL string searching
*
* - This is a wrapper for Abseil's StrContain
*
* - Query on whether the value exists on the string
*
* -- Depending on the implementation, absl StrContains can be faster than the given find() of std::string
*
* @param haystack  source string
* @param needle find value
* @param offset starting offset for the haystack
* @returns bool: true = found, false = not found
*/
constexpr auto StrContains(AoL::StringView haystack, char needle, SizeT offset = 0) noexcept -> bool
{
	return haystack.find(needle, offset) != haystack.npos;
}

/**
* @details AoL string searching
*
* - This is a wrapper for Abseil's StrContain
*
* - Query on whether the value exists on the string
*
* -- Depending on the implementation, absl StrContains can be faster than the given find() of std::string
*
* @param haystack source string
* @param needle find value
* @param offset starting offset for the haystack
* @param count needle size
* @returns bool: true = found, false = not found
*/
constexpr auto StrContains(AoL::StringView haystack, const char* const needle, SizeT offset, SizeT count) noexcept -> bool
{
	return haystack.find(needle, offset, count) != haystack.npos;
}

/**
* @details AoL to_string conversion
* 
* - This is a wrapper for Abseil's StrCat
* 
* - From my benchmarks, Abseil's StrCat and fmt's to_string are just the same except for double, where Abseil is 2x faster
* 
* @param value to be converted value
* @tparam T value type (int, short, char, float, double, boolean)
* @returns AoL::String: resulting string
*/
template<typename T>
constexpr auto ToString(Traits::ConstRefOrCopyType<T> value) noexcept -> AoL::String
{
	return absl::StrCat(value);
}


} // AoL namespace