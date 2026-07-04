/********************************************************************
* Random benchmarks
********************************************************************/

#ifdef BENCHMARK_CONFIG_ON

#include "config.h"
#if AOL_BENCHMARK_FLAG(RANDOM)

#include "all_benchmarks.h"

#include "aol/aol.h"

#define AOL_BENCHMARK_COINFLIP_BENCHMARK_FLAG 1
#define AOL_BENCHMARK_ROLLRANGE_BENCHMARK_FLAG 0

namespace AoL::Benchmark
{

static AoL::Rand::Gen64 gen(0);

#if AOL_BENCHMARK_COINFLIP_BENCHMARK_FLAG
static void BM_RANDOM_COINFLIP1(benchmark::State& state)
{
	AoL::Rand::PoolBit64_1 pool(gen);

	for (auto _ : state)
	{
		pool.Next(gen);
	}
}

static void BM_RANDOM_COINFLIP2(benchmark::State& state)
{
	AoL::Rand::PoolBit64_1 pool(gen);

	for (auto _ : state)
	{
		AoL::Rand::FlipCoin(gen, pool);
	}
}

static void BM_RANDOM_COINFLIP3(benchmark::State& state)
{
	for (auto _ : state)
	{
		AoL::Rand::RollChance(5000, gen);
	}
}

static void BM_RANDOM_COINFLIP4(benchmark::State& state)
{
	for (auto _ : state)
	{
		AoL::Rand::FlipCoin(gen);
	}
}

static void BM_RANDOM_COINFLIP5(benchmark::State& state)
{
	for (auto _ : state)
	{
		AoL::Rand::FlipCoin();
	}
}

static void BM_RANDOM_RollInt(benchmark::State& state)
{
	AoL::Rand::PoolBit64_16 pool(gen);

	for (auto _ : state)
	{
		AoL::Rand::RollChance(2525, gen, pool);
	}
}

static void BM_RANDOM_RollFloat(benchmark::State& state)
{
	AoL::Rand::PoolBit64_16 pool(gen);

	for (auto _ : state)
	{
		AoL::Rand::RollChance(0.2525f, gen, pool);
	}
}

static void BM_RANDOM_RollInt64(benchmark::State& state)
{
	for (auto _ : state)
	{
		AoL::Rand::RollChance(2525, gen);
	}
}
#endif
#if AOL_BENCHMARK_ROLLRANGE_BENCHMARK_FLAG
static void BM_RANDOM_ROLLRANGE_STL(benchmark::State& state)
{
	AoL::SizeT min = 1;
	AoL::SizeT max = 21542;
	std::uniform_int_distribution dist(min, max);

	for (auto _ : state)
	{
		dist(gen);
	}
}

static void BM_RANDOM_ROLLRANGE_CUSTOM1(benchmark::State& state)
{
	AoL::Rand::PoolBit16 pool(gen);

	auto min = AoL::Rand::RollRange(unsigned short(0), unsigned short(std::numeric_limits<unsigned short>::max() - 10), gen, pool);
	auto max = AoL::Rand::RollRange(unsigned short(min + 1), unsigned short(std::numeric_limits<unsigned short>::max() - 1), gen, pool);

	for (auto _ : state)
	{
		AoL::Rand::RollRange(min, max, gen, pool);
	}
}

static void BM_RANDOM_ROLLRANGE_CUSTOM2(benchmark::State& state)
{
	AoL::Rand::PoolBit16 pool(gen);
	
	auto min = AoL::Rand::RollRange((short)SHRT_MIN, short(0), gen, pool);
	auto max = AoL::Rand::RollRange(short(min + 1), (short)SHRT_MAX, gen, pool);
	
	for (auto _ : state)
	{
		AoL::Rand::RollRange(min, max, gen, pool);
	}
}

static void BM_RANDOM_ROLLRANGE_CUSTOM3(benchmark::State& state)
{
	AoL::Rand::PoolBit16 pool(gen);

	float min = AoL::Rand::RollRange(float(-FLT_MIN), float(0), gen, pool);
	float max = 0;
	do
	{
		max = AoL::Rand::RollRange(float(1), float(FLT_MAX), gen, pool);
	} while (max < min);

	for (auto _ : state)
	{
		AoL::Rand::RollRange(min, max, gen, pool);
	}
}

static void BM_RANDOM_ROLLRANGE_CUSTOM4(benchmark::State& state)
{
	AoL::Rand::PoolBit16 pool(gen);

	constexpr AoL::U16 min = 1;
	constexpr AoL::U16 max = 21542;

	for (auto _ : state)
	{
		AoL::Rand::RollRange<min, max>(gen, pool);
	}
}

static void BM_RANDOM_ROLLRANGE_CUSTOM5(benchmark::State& state)
{
	AoL::Rand::PoolBit16 pool(gen);

	constexpr AoL::I16 min = -3111;
	constexpr AoL::I16 max = 3222;

	for (auto _ : state)
	{
		AoL::Rand::RollRange<min, max>(gen, pool);
	}
}

static void BM_RANDOM_ROLLRANGE_CUSTOM6(benchmark::State& state)
{
	AoL::Rand::PoolBit16 pool(gen);

	constexpr float min = -1.0f;
	constexpr float max = 1.0f;

	for (auto _ : state)
	{
		AoL::Rand::RollRange<min, max>(gen, pool);
	}
}
#endif

}

#if AOL_BENCHMARK_COINFLIP_BENCHMARK_FLAG
BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP1);
BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP2);
BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP3);
BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP4);
BENCHMARK(AoL::Benchmark::BM_RANDOM_COINFLIP5);
BENCHMARK(AoL::Benchmark::BM_RANDOM_RollInt);
BENCHMARK(AoL::Benchmark::BM_RANDOM_RollInt64);
BENCHMARK(AoL::Benchmark::BM_RANDOM_RollFloat);
#endif
#if AOL_BENCHMARK_ROLLRANGE_BENCHMARK_FLAG
BENCHMARK(AoL::Benchmark::BM_RANDOM_ROLLRANGE_STL);
BENCHMARK(AoL::Benchmark::BM_RANDOM_ROLLRANGE_CUSTOM1);
BENCHMARK(AoL::Benchmark::BM_RANDOM_ROLLRANGE_CUSTOM2);
BENCHMARK(AoL::Benchmark::BM_RANDOM_ROLLRANGE_CUSTOM3);
BENCHMARK(AoL::Benchmark::BM_RANDOM_ROLLRANGE_CUSTOM4);
BENCHMARK(AoL::Benchmark::BM_RANDOM_ROLLRANGE_CUSTOM5);
BENCHMARK(AoL::Benchmark::BM_RANDOM_ROLLRANGE_CUSTOM6);
#endif

#endif // AOL_BENCHMARK_FLAG(RANDOM)
#endif // BENCHMARK_CONFIG_ON