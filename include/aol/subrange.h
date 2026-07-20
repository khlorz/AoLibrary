/***************************************************************************************
* AoLibrary Subrange
****************************************************************************************
* - The library's Subrange
***************************************************************************************/
#ifndef AOL_HEADER_SUBRANGE_H
#define AOL_HEADER_SUBRANGE_H


#include "configs.h"
#include "macros.h"
#include "traits.h"
#include "types.h"

#include "internal/containers/subrange.h"

#if defined(AOL_CONFIG_FLAG_USE_STD_SUBRANGE)
#include <ranges>
#endif


namespace AoL
{

#if defined(AOL_CONFIG_FLAG_USE_STD_SUBRANGE)
template<
	typename It,
	typename Sentinel = It,
	std::ranges::subrange_kind Kind = std::sized_sentinel_for<Se, It> ? std::ranges::subrange_kind::sized : std::ranges::subrange_kind::unsized
>
using Subrange = std::ranges::subrange<It, Sentinel, Kind>;
#else
template<
	typename It
>
using Subrange = Internal::SubrangeEx<It>;
#endif

}


#endif // AOL_HEADER_SUBRANGE_H