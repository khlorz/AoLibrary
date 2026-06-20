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

void BM_RANDOM_COINFLIP2(benchmark::State& state)
{
	AoL::Rand::PoolBit1 pool(gen);

	for (auto _ : state)
	{
		AoL::Rand::FlipCoin(gen, pool);
	}
}

void BM_RANDOM_COINFLIP3(benchmark::State& state)
{
	AoL::Rand::PoolBit16 pool(gen);

	for (auto _ : state)
	{
		AoL::Rand::RollChance(5000, gen, pool);
	}
}

}

BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP1);
BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP2);
BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP3);
BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP4);

#endif // AOL_BENCHMARK_FLAG(RANDOM)
#endif // BENCHMARK_CONFIG_ON