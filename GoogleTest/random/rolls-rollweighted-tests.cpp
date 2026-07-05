/********************************************************************
* RollWeighted tests
********************************************************************/

#include "pch.h"

namespace
{

using AoLRng = AoL::Rand::DefaultGen;
using Pool32 = AoL::Rand::PoolBit<AoL::U64, 32>;

constexpr int many_trials = 200'000;

}

/*********************************************************************************************
* RollWeighted - single element
*********************************************************************************************/

TEST(RollWeighted, SingleElementAlwaysReturnsZero)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::array<AoL::U32, 1> weights = { 5 };
	for (int i = 0; i < 100; ++i)
	{
		EXPECT_EQ(AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool), 0u);
	}
}

TEST(RollWeighted, SingleElementLargeWeight)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::array<AoL::U32, 1> weights = { 4'000'000'000u };
	for (int i = 0; i < 100; ++i)
	{
		EXPECT_EQ(AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool), 0u);
	}
}

/*********************************************************************************************
* RollWeighted - two elements
*********************************************************************************************/

TEST(RollWeighted, FirstWeightZeroAlwaysReturnsOne)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::array<AoL::U32, 2> weights = { 0, 1 };
	for (int i = 0; i < 100; ++i)
	{
		EXPECT_EQ(AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool), 1u);
	}
}

TEST(RollWeighted, SecondWeightZeroAlwaysReturnsZero)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::array<AoL::U32, 2> weights = { 1, 0 };
	for (int i = 0; i < 100; ++i)
	{
		EXPECT_EQ(AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool), 0u);
	}
}

TEST(RollWeighted, EqualWeightsAreUniform)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<AoL::U32, 2> weights = { 1, 1 };
	int counts[2] = {};

	for (int i = 0; i < many_trials; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		ASSERT_LT(idx, 2u);
		++counts[idx];
	}

	double ratio = static_cast<double>(counts[0]) / many_trials;
	EXPECT_NEAR(ratio, 0.5, 0.01);
}

TEST(RollWeighted, UnequalWeightsOneToThree)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<AoL::U32, 2> weights = { 1, 3 };
	int counts[2] = {};

	for (int i = 0; i < many_trials; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		ASSERT_LT(idx, 2u);
		++counts[idx];
	}

	double ratio = static_cast<double>(counts[0]) / many_trials;
	EXPECT_NEAR(ratio, 0.25, 0.015);
}

TEST(RollWeighted, UnequalWeightsThreeToOne)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<AoL::U32, 2> weights = { 3, 1 };
	int counts[2] = {};

	for (int i = 0; i < many_trials; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		ASSERT_LT(idx, 2u);
		++counts[idx];
	}

	double ratio = static_cast<double>(counts[0]) / many_trials;
	EXPECT_NEAR(ratio, 0.75, 0.015);
}

TEST(RollWeighted, UnequalWeightsLargeNumbers)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<AoL::U32, 2> weights = { 1000, 3000 };
	int counts[2] = {};

	for (int i = 0; i < many_trials; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		ASSERT_LT(idx, 2u);
		++counts[idx];
	}

	double ratio = static_cast<double>(counts[0]) / many_trials;
	EXPECT_NEAR(ratio, 0.25, 0.015);
}

/*********************************************************************************************
* RollWeighted - multiple elements
*********************************************************************************************/

TEST(RollWeighted, EqualWeightsMultipleElements)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<AoL::U32, 5> weights = { 1, 1, 1, 1, 1 };
	int counts[5] = {};

	for (int i = 0; i < many_trials; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		ASSERT_LT(idx, 5u);
		++counts[idx];
	}

	for (int c : counts)
	{
		double ratio = static_cast<double>(c) / many_trials;
		EXPECT_NEAR(ratio, 0.2, 0.02);
	}
}

TEST(RollWeighted, SkewedDistributionFiveElements)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<AoL::U32, 5> weights = { 10, 20, 30, 40, 50 };
	int counts[5] = {};
	int total_weight = 150;

	for (int i = 0; i < many_trials; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		ASSERT_LT(idx, 5u);
		++counts[idx];
	}

	for (int j = 0; j < 5; ++j)
	{
		double expected_pct = static_cast<double>(weights[j]) / total_weight;
		double ratio = static_cast<double>(counts[j]) / many_trials;
		EXPECT_NEAR(ratio, expected_pct, 0.02);
	}
}

TEST(RollWeighted, ManyWeightsSmokeTest)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::vector<AoL::U32> weights(100, 1);
	int total_weight = 100;

	int counts[100] = {};

	for (int i = 0; i < many_trials; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		ASSERT_LT(idx, 100u);
		++counts[idx];
	}

	for (int j = 0; j < 100; ++j)
	{
		double ratio = static_cast<double>(counts[j]) / many_trials;
		double expected = 1.0 / 100;
		EXPECT_NEAR(ratio, expected, 0.02);
	}
}

TEST(RollWeighted, ZeroWeightsMixedWithPositive)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<AoL::U32, 5> weights = { 0, 0, 5, 0, 0 };
	for (int i = 0; i < 100; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		EXPECT_EQ(idx, 2u);
	}
}

TEST(RollWeighted, SinglePositiveAmongManyZeros)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::array<AoL::U16, 4> weights = { 0, 0, 0, 7 };
	for (int i = 0; i < 100; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		EXPECT_EQ(idx, 3u);
	}
}



TEST(RollWeighted, SmallWeightTypeU8)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<AoL::U8, 3> weights = { 1, 2, 3 };
	int counts[3] = {};

	for (int i = 0; i < many_trials; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
		ASSERT_LT(idx, 3u);
		++counts[idx];
	}

	double const expected[] = { 1.0 / 6, 2.0 / 6, 3.0 / 6 };
	for (int j = 0; j < 3; ++j)
	{
		double ratio = static_cast<double>(counts[j]) / many_trials;
		EXPECT_NEAR(ratio, expected[j], 0.02);
	}
}

TEST(RollWeighted, MatchingReturnTypeAoLSizeT)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::array<AoL::U32, 1> weights = { 1 };
	auto result = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool);
	EXPECT_TRUE((std::is_same_v<decltype(result), AoL::SizeT>));
}

TEST(RollWeighted, DeterministicSeedRepeatableResults)
{
	std::array<AoL::U32, 4> weights = { 1, 2, 3, 4 };
	std::vector<AoL::SizeT> results_a, results_b;

	{
		auto rng = AoLRng(9999);
		Pool32 pool;
		for (int i = 0; i < 100; ++i)
		{
			results_a.push_back(AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool));
		}
	}

	{
		auto rng = AoLRng(9999);
		Pool32 pool;
		for (int i = 0; i < 100; ++i)
		{
			results_b.push_back(AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng, pool));
		}
	}

	EXPECT_EQ(results_a, results_b);
}

/*********************************************************************************************
* RollWeighted no-pool
*********************************************************************************************/

TEST(RollWeighted_NoPool, SingleElementAlwaysReturnsZero)
{
	auto rng = AoLRng(42);

	std::array<AoL::U32, 1> weights = { 5 };
	for (int i = 0; i < 100; ++i)
	{
		EXPECT_EQ(AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng), 0u);
	}
}

TEST(RollWeighted_NoPool, EqualWeightsAreUniform)
{
	auto rng = AoLRng(12345);

	std::array<AoL::U32, 2> weights = { 1, 1 };
	int counts[2] = {};

	for (int i = 0; i < many_trials; ++i)
	{
		auto idx = AoL::Rand::RollWeighted(weights.begin(), weights.end(), rng);
		ASSERT_LT(idx, 2u);
		++counts[idx];
	}

	double ratio = static_cast<double>(counts[0]) / many_trials;
	EXPECT_NEAR(ratio, 0.5, 0.01);
}
