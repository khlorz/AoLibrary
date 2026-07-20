/***************************************************************************************
* Random number generator implementations
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_RANDOMS_GENERATORS_H
#define AOL_HEADER_INTERNAL_RANDOMS_GENERATORS_H


#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/traits.h"
#include "aol/types.h"

#if defined(AOL_CONFIG_FLAG_USE_XOSHIRO_NESSAN_RNG)
#include "xoshiro/xoshiro.h"
#elif defined(AOL_CONFIG_FLAG_USE_PCG_CPP_RNG)
#include "pcg-cpp/pcg_random.hpp"
#elif defined(AOL_CONFIG_FLAG_USE_STD_RNG)
#include <random>
#else
// Do nothing
#endif


namespace AoL
{

namespace Rand
{

/**
* @details The 64 bit random number generator
*/
using Gen64
#if defined(AOL_CONFIG_FLAG_USE_XOSHIRO_NESSAN_RNG)
= xso::rng64;
#elif defined(AOL_CONFIG_FLAG_USE_PCG_CPP_RNG)
= pcg64;
#elif defined(AOL_CONFIG_FLAG_USE_STD_RNG)
= std::mt19937_64;
#else
#error "No custom random number generator!"
#endif

/**
* @details The 32 bit random number generator
*/
using Gen32
#if defined(AOL_CONFIG_FLAG_USE_XOSHIRO_NESSAN_RNG)
= xso::rng32;
#elif defined(AOL_CONFIG_FLAG_USE_PCG_CPP_RNG)
= pcg32;
#elif defined(AOL_CONFIG_FLAG_USE_STD_RNG)
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
using DefaultGen = Gen64;

} // Rand namespace

} // AoL namespace


#endif // AOL_HEADER_INTERNAL_RANDOMS_GENERATORS_H