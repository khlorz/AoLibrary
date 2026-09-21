/***************************************************************************************
* AoLibrary types
****************************************************************************************
* - These fundamental types typedef for ease of library use
* - If using the library, better to use this types
***************************************************************************************/
#ifndef AOL_HEADER_TYPES_H
#define AOL_HEADER_TYPES_H


#include "aol/internal/macros/defines.h"

#include <cstddef>

#if AOL_COMPILER_MSVC
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
#if AOL_COMPILER_CLANG || AOL_COMPILER_GCC
= unsigned __int128;
#elif AOL_COMPILER_MSVC
= std::_Unsigned128;
#else
#error "No unsigned int128 implementation available!"
#endif
using I128
#if AOL_COMPILER_CLANG || AOL_COMPILER_GCC
= __int128;
#elif AOL_COMPILER_MSVC
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

#endif // AOL_HEADER_TYPES_H