/***************************************************************************************
* AoLibrary's random
****************************************************************************************
* - Contain's the libraries random related things
* - Planned to contain anything related to randomness
* - Also contain's the stl's random, aliased to the library for consistency
***************************************************************************************/
#ifndef AOL_HEADER_RANDOMS_H
#define AOL_HEADER_RANDOMS_H

#include "configs.h"

#include "types.h"
#include "traits.h"
#include "hashes.h"
#include "internal/containers.h"
#include "algorithms.h"

#include <random>
#include <chrono>
#include <thread>
#include <bit>

#if defined(AOL_CONFIG_FLAG_USE_XOSHIRO_NESSAN_RNG)
#include "xoshiro/xoshiro.h"
#elif defined(AOL_CONFIG_FLAG_USE_PCG_CPP_RNG)
#include "pcg-cpp/pcg_random.hpp"
#elif defined(AOL_CONFIG_FLAG_USE_STD_RNG)
// By default, <random> is included
#else
// Do nothing
#endif

#include "internal/randoms/generators.h"
#include "internal/randoms/pool.h"
#include "internal/randoms/rolls.h"
#include "internal/randoms/algorithms.h"

#endif // AOL_HEADER_RANDOMS_H