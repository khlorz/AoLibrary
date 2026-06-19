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
// Do nothing
#endif

namespace AoL
{

/**
* @details The 64 bit random number generator
*/
using RandGen64
#if defined(AOL_USE_XOSHIRO_NESSAN_RNG)
= xso::rng64;
#elif defined(AOL_USE_PCG_CPP_RNG)
= pcg64;
#elif defined(AOL_USE_STD_RNG)
= std::mt19937_64;
#else
#error "No custom random number generator!"
#endif

/**
* @details The 32 bit random number generator
*/
using RandGen32
#if defined(AOL_USE_XOSHIRO_NESSAN_RNG)
= xso::rng32;
#elif defined(AOL_USE_PCG_CPP_RNG)
= pcg32;
#elif defined(AOL_USE_STD_RNG)
= std::mt19937;
#else
#error "No custom random number generator!"
#endif

/**
* @details The default RNG generator
*
* - By the default, if the config.h is not modified, we use xoshiro implementation
*
* - The library can also use pcg-cpp implementation as well
*/
using RandGen = RandGen64;

} // AoL namespace

#endif // AOL_INTERNAL_RANDOM_H