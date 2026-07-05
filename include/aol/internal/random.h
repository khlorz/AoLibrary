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

#include "types.h"
#include "traits.h"
#include "hashes.h"

#include <random>
#include <chrono>
#include <thread>
#include <iterator>

#if defined(AOL_USE_XOSHIRO_NESSAN_RNG)
#include "xoshiro/xoshiro.h"
#elif defined(AOL_USE_PCG_CPP_RNG)
#include "pcg-cpp/pcg_random.hpp"
#elif defined(AOL_USE_STD_RNG)
// By default, <random> is included
#else
// Do nothing
#endif

#include "random-generators-impl.h"
#include "random-rng-pool-impl.h"
#include "random-globals.h"
#include "random-rng-rolls-impl.h"
#include "random-algorithms-impl.h"

#endif // AOL_INTERNAL_RANDOM_H