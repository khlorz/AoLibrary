#pragma once

/***************************************************************************************
* AoLibrary types
****************************************************************************************
* - These fundamental types typedef for ease of library use
* - If using the library, better to use this types
***************************************************************************************/

#include <cstddef>

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

using Char		= char;
using Int		= int;
using SizeT		= std::size_t;
using PtrSize	= decltype(sizeof(void*));
using PtrDiff	= std::ptrdiff_t;

}