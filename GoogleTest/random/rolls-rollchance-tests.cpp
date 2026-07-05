/********************************************************************
* RollChance + FlipCoin tests
********************************************************************/

#include "pch.h"

namespace
{

using AoLRng = AoL::Rand::DefaultGen;
using Pool32 = AoL::Rand::PoolBit<AoL::U64, 32>;

constexpr int many_trials = 200'000;

AoL::SizeT Cs(auto x)
{
	return static_cast<AoL::SizeT>(x);
}

}

/*********************************************************************************************
* RollChance
*********************************************************************************************/

TEST(RollChance, ZeroChanceAlwaysFails)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int i = 0; i < 1000; ++i)
	{
		EXPECT_FALSE((AoL::Rand::RollChance<10000>(Cs(0), rng, pool)));
	}
}

TEST(RollChance, FullChanceAlwaysSucceeds)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int i = 0; i < 1000; ++i)
	{
		EXPECT_TRUE((AoL::Rand::RollChance<10000>(Cs(10000), rng, pool)));
	}
}

TEST(RollChance, FiftyPercentIsRoughlyUniform)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int successes = 0;
	for (int i = 0; i < many_trials; ++i)
	{
		if (AoL::Rand::RollChance<10000>(Cs(5000), rng, pool))
		{
			++successes;
		}
	}

	double ratio = static_cast<double>(successes) / many_trials;
	EXPECT_NEAR(ratio, 0.5, 0.01);
}

TEST(RollChance, LowChanceRoughlyMatchesProbability)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int successes = 0;
	for (int i = 0; i < many_trials; ++i)
	{
		if (AoL::Rand::RollChance<10000>(Cs(500), rng, pool))
		{
			++successes;
		}
	}

	double ratio = static_cast<double>(successes) / many_trials;
	EXPECT_NEAR(ratio, 0.05, 0.01);
}

TEST(RollChance, AlternativeChanceScale)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int successes = 0;
	for (int i = 0; i < many_trials; ++i)
	{
		if (AoL::Rand::RollChance<100>(Cs(25), rng, pool))
		{
			++successes;
		}
	}

	double ratio = static_cast<double>(successes) / many_trials;
	EXPECT_NEAR(ratio, 0.25, 0.01);
}

TEST(RollChance_Float, ZeroAndFull)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	EXPECT_FALSE((AoL::Rand::RollChance<10000>(0.0f, rng, pool)));
	EXPECT_TRUE((AoL::Rand::RollChance<10000>(1.0f, rng, pool)));
}

/*********************************************************************************************
* FlipCoin
*********************************************************************************************/

TEST(FlipCoin, NoArgConvenienceOverloadRunsAndIsRoughlyUniform)
{
	auto rng = AoLRng(12345);
	int heads = 0;
	for (int i = 0; i < many_trials; ++i)
	{
		if (AoL::Rand::FlipCoin(rng))
		{
			++heads;
		}
	}

	double ratio = static_cast<double>(heads) / many_trials;
	EXPECT_NEAR(ratio, 0.5, 0.02);
}
