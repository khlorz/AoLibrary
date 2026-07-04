/***************************************************************************************
* AoLibrary types
****************************************************************************************
* - These fundamental types typedef for ease of library use
* - If using the library, better to use this types
***************************************************************************************/
#ifndef AOL_INTERNAL_TYPES_H
#define AOL_INTERNAL_TYPES_H

#include <cstddef>

#if defined(_MSC_VER)
#include <__msvc_int128.hpp>
#endif

namespace AoL
{

using U8	= unsigned char;
using I8	= signed char;
using U16	= unsigned short;
using I16	= short;
using U32	= unsigned int;
using I32	= int;
using U64	= unsigned long long;
using I64	= long long;
using U128
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
= unsigned __int128;
#elif defined(_MSC_VER)
= std::_Unsigned128;
#else
#error "No unsigned int128 implementation available!"
#endif
using I128
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
= __int128;
#elif defined(_MSC_VER)
= std::_Signed128;
#else
#error "No signed int128 implementation available!"
#endif

using Void		= void;
using Char		= char;
using Int		= int;
using SizeT		= std::size_t;
using PtrSize	= decltype(sizeof(Void*));
using PtrDiff	= std::ptrdiff_t;

} // AoL namespace

#endif // AOL_INTERNAL_TYPES_H