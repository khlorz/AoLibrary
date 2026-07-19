/***************************************************************************************
* AoLibrary containers
****************************************************************************************
* - The library's main container types
* - It is better to use these if you include the library for better ease of use
* - Some custom containers and functions are provided as well
* - Additional containers/functions will be added in the future as the library grows
***************************************************************************************/
#ifndef AOL_INTERNAL_CONTAINERS_H
#define AOL_INTERNAL_CONTAINERS_H

#include "configs.h"

#include "macros.h"
#include "aol/allocators.h"
#include "hashes.h"
#include "types.h"
#include "traits.h"
#include "math.h"
#include "aol/algorithms.h"

#include <iterator>		// iterator/const_iterators
#include <compare>		// <=> operator
#include <cassert>		// assert macro
#include <utility>		// std::pair
#include <ranges>		// std::views


namespace AoL
{




} // AoL namespace


/*************************************************
* Implementation includes
*************************************************/

#include "containers-subrange-impl.h"
#include "containers-partitions-impl.h"

#endif // AOL_INTERNAL_CONTAINERS_H