/********************************************************************
* Random benchmarks
********************************************************************/

#ifdef BENCHMARK_CONFIG_ON

#include "config.h"
#if AOL_BENCHMARK_FLAG(RANDOM)

#include "all_benchmarks.h"

#include "aol/aol.h"

namespace AoL::Benchmark
{

static AoL::Rand::Gen64 gen(0);

void BM_RANDOM_COINFLIP1(benchmark::State& state)
{
	AoL::Rand::PoolBit1 pool(gen);

	for (auto _ : state)
	{
		pool.Next(gen);
	}
}

}

BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP1);

#endif // AOL_BENCHMARK_FLAG(RANDOM)
#endif // BENCHMARK_CONFIG_ON