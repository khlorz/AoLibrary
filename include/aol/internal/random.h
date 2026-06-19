/***************************************************************************************
* AoLibrary's random
****************************************************************************************
* - Contain's the libraries random related things
* - Planned to contain anything related to randomness
* - Also contain's the stl's random, aliased to the library for consistency
***************************************************************************************/
#ifndef AOL_INTERNAL_RANDOM_H
#define AOL_INTERNAL_RANDOM_H

#include "config.h"

#include <random>

#if defined(AOL_USE_XOSHIRO_NESSAN_RNG)
#include "xoshiro/xoshiro.h"
#elif defined(AOL_USE_PCG_CPP_RNG)
#include "pcg-cpp/pcg_random.hpp"
#elif defined(AOL_USE_STD_RNG)
// By default, <random> is included
#else
#error "No custom RNG!"
#endif

namespace AoL
{

} // AoL namespace

#endif // AOL_INTERNAL_RANDOM_H