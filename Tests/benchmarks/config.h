#pragma once

/********************************************************************
* Benchmark configuration
********************************************************************/

// Change to 1 if you want to run all tests
#define AOL_BENCHMARK_ALL_BENCHMARK_ENABLED_FLAG 0

// Change to 1 if you want to disable all tests
#define AOL_BENCHMARK_ALL_BENCHMARK_DISABLED_FLAG 0

#if (AOL_BENCHMARK_ALL_BENCHMARK_ENABLED_FLAG == 1) && (AOL_BENCHMARK_ALL_BENCHMARK_DISABLED_FLAG == 1)
#error "Both flags cannot be on at the same time!"
#endif

#define AOL_BENCHMARK_CONTAINERS_BENCHMARK_FLAG 0
#define AOL_BENCHMARK_RANDOM_BENCHMARK_FLAG 1

#define AOL_BENCHMARK_FLAG(flag_name) !AOL_BENCHMARK_ALL_BENCHMARK_DISABLED_FLAG && (AOL_BENCHMARK_ALL_BENCHMARK_ENABLED_FLAG || AOL_BENCHMARK_##flag_name##_BENCHMARK_FLAG)
