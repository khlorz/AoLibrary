#pragma once

#include "config.h"

#include "types.h"
#include "allocators.h"

#include <string>

namespace AoL
{

/**
* Basic string of the library
* 
* - It's also just a 'using' wrapper for std::basic_string<T, Traits, Allocator>
* 
* - For this string, the allocator can be changed depending on use case
* 
* @tparam A allocator type (default = Internal::DefaultStringAllocator)
*/
using String = std::basic_string<char, std::char_traits<char>, Internal::DefaultStringAllocator>;

/**
* String but for pool allocators
*
* - Specialize string used with pool/memory resources
*
* @tparam A allocator type (default = Internal::DefaultStringPoolAllocator)
*/
using StringPool = std::basic_string<char, std::char_traits<char>, Internal::DefaultStringPoolAllocator>;

}