/***************************************************************************************
* AoLibrary typedefs
****************************************************************************************
* - Compared to types, these are not fundamental types typedefs
* - This is more on typedefs that are commonly used throughout the codebase
* - e.g Optional<String> -> StringOpt
***************************************************************************************/
#ifndef AOL_INTERNAL_TYPEDEFS_H
#define AOL_INTERNAL_TYPEDEFS_H

#include "types.h"
#include "dynamic_types.h"
#include "strings.h"

namespace AoL
{

using StringOpt = Optional<String>;
using IntOpt	= Optional<Int>;
using SizeTOpt	= Optional<SizeT>;

} // AoL namespace

#endif // AOL_INTERNAL_TYPEDEFS_H