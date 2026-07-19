/********************************************************************
* RollRange Distribution Uniformity tests
********************************************************************/

#include "pch.h"

#include "aol/randoms.h"

namespace
{

using AoLRng = AoL::Rand::DefaultGen;
using Pool32 = AoL::Rand::PoolBit<AoL::U64, 32>;

constexpr int many_trials = 200'000;

}

/*********************************************************************************************
* RollRange - Distribution Uniformity Tests
*********************************************************************************************/

TEST(RollRange_Uniformity, UnsignedSmallRange)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	AoL::U32 const min = 0, max = 9;
	int const range_size = static_cast<int>(max - min + 1);
	std::vector<int> histogram(range_size, 0);

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::U32>(min, max, rng, pool);
		histogram[static_cast<int>(v - min)]++;
	}

	double const expected = static_cast<double>(many_trials) / range_size;
	for (int count : histogram)
	{
		EXPECT_NEAR(static_cast<double>(count) / expected, 1.0, 0.05);
	}
}

TEST(RollRange_Uniformity, SignedRangeAcrossZero)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	AoL::I32 const min = -50, max = 50;
	int const range_size = static_cast<int>(max - min + 1);
	std::vector<int> histogram(range_size, 0);

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::I32>(min, max, rng, pool);
		histogram[static_cast<int>(v - min)]++;
	}

	double const expected = static_cast<double>(many_trials) / range_size;
	for (int count : histogram)
	{
		EXPECT_NEAR(static_cast<double>(count) / expected, 1.0, 0.08);
	}
}

TEST(RollRange_Uniformity, FloatUnitInterval)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int const bucket_count = 100;
	std::vector<int> buckets(bucket_count, 0);

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<double>(0.0, 1.0, rng, pool);
		int bucket = static_cast<int>(v * bucket_count);
		if (bucket >= bucket_count)
	{
		bucket = bucket_count - 1;
	}
		buckets[bucket]++;
	}

	double const expected = static_cast<double>(many_trials) / bucket_count;
	for (int count : buckets)
	{
		EXPECT_NEAR(static_cast<double>(count) / expected, 1.0, 0.12);
	}
}
