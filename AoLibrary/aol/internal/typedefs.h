#pragma once

/***************************************************************************************
* AoLibrary typedefs
****************************************************************************************
* - Compared to types, these are not fundamental types typedefs
* - This is more on typedefs that are commonly used throughout the codebase
* - e.g Optional<String> -> StringOpt
***************************************************************************************/

#include "types.h"
#include "dynamic_types.h"
#include "strings.h"

namespace AoL
{

using StringOpt = Optional<String>;
using IntOpt	= Optional<Int>;
using SizeTOpt	= Optional<SizeT>;

}