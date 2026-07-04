#pragma once

#include <gtest/gtest.h>
#include <random>
#include <cstdint>
#include <array>

namespace {

// PoolBit::Next / its constructor static_assert that the RNG's operator()
// return type is *exactly* the pool's RNGSizeType, so we use TestRng64 (see
// TestRng.hpp) rather than std::mt19937_64, whose result_type
// (uint_fast64_t) isn't guaranteed to be the same type as AoL::U64.
using AoLRng = AoL::Rand::DefaultGen;

// Pool widths used across tests, all backed by a 64-bit RNG.
using Pool8 = AoL::Rand::PoolBit<AoL::U64, 8>;
using Pool16 = AoL::Rand::PoolBit<AoL::U64, 16>;
using Pool32 = AoL::Rand::PoolBit<AoL::U64, 32>;
using Pool64 = AoL::Rand::PoolBit<AoL::U64, 32>; // BitSize must be <= RNGBitSize/2

constexpr int kManyTrials = 200'000;

// Returns a deterministically-seeded RNG so failures are reproducible.
AoLRng MakeRng(std::uint64_t seed = 12345)
{
	return AoLRng(seed);
}

} // namespace

/*********************************************************************************************
* RollChance (integer chance, explicit pool)
*********************************************************************************************/

TEST(RollChance_Int, ZeroChanceAlwaysFails)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < 1000; ++i)
	{
		EXPECT_FALSE((AoL::Rand::RollChance<10000>(static_cast<AoL::SizeT>(0), rng, pool)));
	}
}

TEST(RollChance_Int, FullChanceAlwaysSucceeds)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < 1000; ++i)
	{
		EXPECT_TRUE((AoL::Rand::RollChance<10000>(static_cast<AoL::SizeT>(10000), rng, pool)));
	}
}

TEST(RollChance_Int, FiftyPercentIsRoughlyUniform)
{
	auto rng = MakeRng();
	Pool32 pool;

	int successes = 0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		if (AoL::Rand::RollChance<10000>(static_cast<AoL::SizeT>(5000), rng, pool))
		{
			++successes;
		}
	}

	const double ratio = static_cast<double>(successes) / kManyTrials;
	EXPECT_NEAR(ratio, 0.5, 0.01);
}

TEST(RollChance_Int, LowChanceRoughlyMatchesProbability)
{
	auto rng = MakeRng();
	Pool32 pool;

	int successes = 0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		// 5% chance on a scale of 10000 -> chance value 500
		if (AoL::Rand::RollChance<10000>(static_cast<AoL::SizeT>(500), rng, pool))
		{
			++successes;
		}
	}

	const double ratio = static_cast<double>(successes) / kManyTrials;
	EXPECT_NEAR(ratio, 0.05, 0.01);
}

TEST(RollChance_Int, SupportsAlternativeChanceScale)
{
	auto rng = MakeRng();
	Pool32 pool;

	int successes = 0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		// ChanceScale of 100 (percentage-style), 25%.
		if (AoL::Rand::RollChance<100>(static_cast<AoL::SizeT>(25), rng, pool))
		{
			++successes;
		}
	}

	const double ratio = static_cast<double>(successes) / kManyTrials;
	EXPECT_NEAR(ratio, 0.25, 0.01);
}

/*********************************************************************************************
* RollChance (floating-point chance, explicit pool)
*********************************************************************************************/

TEST(RollChance_Float, ZeroChanceAlwaysFails)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < 1000; ++i)
	{
		EXPECT_FALSE((AoL::Rand::RollChance<10000>(0.0f, rng, pool)));
	}
}

TEST(RollChance_Float, FullChanceAlwaysSucceeds)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < 1000; ++i)
	{
		EXPECT_TRUE((AoL::Rand::RollChance<10000>(1.0f, rng, pool)));
	}
}

TEST(RollChance_Float, FiftyPercentIsRoughlyUniform)
{
	auto rng = MakeRng();
	Pool32 pool;

	int successes = 0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		if (AoL::Rand::RollChance<10000>(0.5, rng, pool))
		{
			++successes;
		}
	}

	const double ratio = static_cast<double>(successes) / kManyTrials;
	EXPECT_NEAR(ratio, 0.5, 0.01);
}

TEST(RollChance_Float, FractionalPercentageMatchesProbability)
{
	auto rng = MakeRng();
	Pool32 pool;

	int successes = 0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		// 12.5% chance.
		if (AoL::Rand::RollChance<10000>(0.125, rng, pool))
		{
			++successes;
		}
	}

	const double ratio = static_cast<double>(successes) / kManyTrials;
	EXPECT_NEAR(ratio, 0.125, 0.01);
}

/*********************************************************************************************
* RollChance (convenience overload, no explicit pool)
*********************************************************************************************/

TEST(RollChance_Convenience, ZeroChanceAlwaysFails)
{
	auto rng = MakeRng();

	for (int i = 0; i < 1000; ++i)
	{
		EXPECT_FALSE((AoL::Rand::RollChance<10000>(static_cast<AoL::SizeT>(0), rng)));
	}
}

TEST(RollChance_Convenience, FullChanceAlwaysSucceeds)
{
	auto rng = MakeRng();

	for (int i = 0; i < 1000; ++i)
	{
		EXPECT_TRUE((AoL::Rand::RollChance<10000>(static_cast<AoL::SizeT>(10000), rng)));
	}
}

TEST(RollChance_Convenience, FiftyPercentIsRoughlyUniform)
{
	auto rng = MakeRng();

	int successes = 0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		if (AoL::Rand::RollChance<10000>(static_cast<AoL::SizeT>(5000), rng))
		{
			++successes;
		}
	}

	const double ratio = static_cast<double>(successes) / kManyTrials;
	EXPECT_NEAR(ratio, 0.5, 0.01);
}

/*********************************************************************************************
* FlipCoin
*********************************************************************************************/

TEST(FlipCoin, PoolBitVariantIsRoughlyUniform)
{
	auto rng = MakeRng();
	AoL::Rand::PoolBit<AoL::Rand::Internal::RNGReturnType<AoLRng>, 1> pool;

	int heads = 0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		if (AoL::Rand::FlipCoin(rng, pool))
		{
			++heads;
		}
	}

	const double ratio = static_cast<double>(heads) / kManyTrials;
	EXPECT_NEAR(ratio, 0.5, 0.01);
}

TEST(FlipCoin, ConvenienceRngOverloadIsRoughlyUniform)
{
	auto rng = MakeRng();

	int heads = 0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		if (AoL::Rand::FlipCoin(rng))
		{
			++heads;
		}
	}

	const double ratio = static_cast<double>(heads) / kManyTrials;
	EXPECT_NEAR(ratio, 0.5, 0.01);
}

TEST(FlipCoin, NoArgConvenienceOverloadRunsAndIsRoughlyUniform)
{
	int heads = 0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		if (AoL::Rand::FlipCoin())
		{
			++heads;
		}
	}

	const double ratio = static_cast<double>(heads) / kManyTrials;
	EXPECT_NEAR(ratio, 0.5, 0.02); // looser tolerance: uses a shared default RNG
}

/*********************************************************************************************
* RollRange - unsigned integers
*********************************************************************************************/

TEST(RollRange_Unsigned, CompileTimeBoundsRespected)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRange<static_cast<AoL::U32>(5), static_cast<AoL::U32>(15)>(rng, pool);
		EXPECT_GE(v, 5u);
		EXPECT_LE(v, 15u);
	}
}

TEST(RollRange_Unsigned, CompileTimeCoversFullSpanOfSmallRange)
{
	auto rng = MakeRng();
	Pool32 pool;

	std::array<bool, 4> seen{}; // range [10,13]
	for (int i = 0; i < 10000; ++i)
	{
		auto v = AoL::Rand::RollRange<static_cast<AoL::U32>(10), static_cast<AoL::U32>(13)>(rng, pool);
		ASSERT_GE(v, 10u);
		ASSERT_LE(v, 13u);
		seen[v - 10] = true;
	}
	EXPECT_TRUE(seen[0] && seen[1] && seen[2] && seen[3]);
}

TEST(RollRange_Unsigned, RuntimeBoundsRespected)
{
	auto rng = MakeRng();
	Pool32 pool;

	const AoL::U32 min = 100, max = 200;
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::U32>(min, max, rng, pool);
		EXPECT_GE(v, min);
		EXPECT_LE(v, max);
	}
}

TEST(RollRange_Unsigned, RuntimeSingleValueRangeAlwaysReturnsThatValue)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < 100; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::U32>(42u, 42u, rng, pool);
		EXPECT_EQ(v, 42u);
	}
}

/*********************************************************************************************
* RollRange - signed integers
*********************************************************************************************/

TEST(RollRange_Signed, CompileTimeBoundsRespectedAcrossZero)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRange<static_cast<AoL::I32>(-10), static_cast<AoL::I32>(10)>(rng, pool);
		EXPECT_GE(v, -10);
		EXPECT_LE(v, 10);
	}
}

TEST(RollRange_Signed, RuntimeBoundsRespectedAcrossZero)
{
	auto rng = MakeRng();
	Pool32 pool;

	const AoL::I32 min = -500, max = -100;
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::I32>(min, max, rng, pool);
		EXPECT_GE(v, min);
		EXPECT_LE(v, max);
	}
}

TEST(RollRange_Signed, RuntimeCoversFullSpanOfSmallRange)
{
	auto rng = MakeRng();
	Pool32 pool;

	std::array<bool, 5> seen{}; // range [-2, 2]
	for (int i = 0; i < 10000; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::I32>(-2, 2, rng, pool);
		ASSERT_GE(v, -2);
		ASSERT_LE(v, 2);
		seen[v + 2] = true;
	}
	EXPECT_TRUE(seen[0] && seen[1] && seen[2] && seen[3] && seen[4]);
}

/*********************************************************************************************
* RollRange - floating point
*********************************************************************************************/

TEST(RollRange_Float, CompileTimeBoundsRespected)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRange<0.0f, 1.0f>(rng, pool);
		EXPECT_GE(v, 0.0f);
		EXPECT_LT(v, 1.0f); // half-open interval
	}
}

TEST(RollRange_Float, RuntimeBoundsRespectedAndRoughlyUniformMean)
{
	auto rng = MakeRng();
	Pool32 pool;

	double sum = 0.0;
	const double min = -5.0, max = 5.0;
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRange<double>(min, max, rng, pool);
		ASSERT_GE(v, min);
		ASSERT_LT(v, max);
		sum += v;
	}

	const double mean = sum / kManyTrials;
	EXPECT_NEAR(mean, 0.0, 0.1); // expected mean of uniform[-5,5) is 0
}

/*********************************************************************************************
* RollRangeSlow - unsigned integers
*********************************************************************************************/

TEST(RollRangeSlow_Unsigned, CompileTimeBoundsRespected)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<static_cast<AoL::U8>(5), static_cast<AoL::U8>(15)>(rng, pool);
		EXPECT_GE(v, 5);
		EXPECT_LE(v, 15);
	}
}

TEST(RollRangeSlow_Unsigned, RuntimeBoundsRespected)
{
	auto rng = MakeRng();
	Pool32 pool;

	const AoL::U16 min = 1000, max = 2000;
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<AoL::U16>(min, max, rng, pool);
		EXPECT_GE(v, min);
		EXPECT_LE(v, max);
	}
}

TEST(RollRangeSlow_Unsigned, HandlesFullSpanOfNarrowTypeWithoutHanging)
{
	// Exercises the rejection-loop branch: range covers the *entire* domain
	// of an 8-bit value type (0..255), which stresses the wraparound math in
	// the "unlikely" correction path.
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<AoL::U8>(
			static_cast<AoL::U8>(0), static_cast<AoL::U8>(255), rng, pool);
		EXPECT_LE(v, 255); // always true for U8, kept for documentation/clarity
	}
}

TEST(RollRangeSlow_Unsigned, CoversFullSpanOfSmallRange)
{
	auto rng = MakeRng();
	Pool32 pool;

	std::array<bool, 4> seen{}; // range [10,13]
	for (int i = 0; i < 10000; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<AoL::U8>(
			static_cast<AoL::U8>(10), static_cast<AoL::U8>(13), rng, pool);
		ASSERT_GE(v, 10);
		ASSERT_LE(v, 13);
		seen[v - 10] = true;
	}
	EXPECT_TRUE(seen[0] && seen[1] && seen[2] && seen[3]);
}

/*********************************************************************************************
* RollRangeSlow - signed integers
*********************************************************************************************/

TEST(RollRangeSlow_Signed, CompileTimeBoundsRespectedAcrossZero)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<static_cast<AoL::I16>(-10), static_cast<AoL::I16>(10)>(rng, pool);
		EXPECT_GE(v, -10);
		EXPECT_LE(v, 10);
	}
}

TEST(RollRangeSlow_Signed, RuntimeBoundsRespectedAcrossZero)
{
	auto rng = MakeRng();
	Pool32 pool;

	const AoL::I16 min = -100, max = 100;
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<AoL::I16>(min, max, rng, pool);
		EXPECT_GE(v, min);
		EXPECT_LE(v, max);
	}
}

// Regression test for the specific failing case called out in the source
// comments: min=-32768, max=12379 with a 16-bit value_t. Without the
// static_cast<value_t> fix on the wraparound subtraction, integer promotion
// to `int` corrupts range_size and can hang the rejection loop.
TEST(RollRangeSlow_Signed, RegressionCaseFromSourceCommentDoesNotHang)
{
	auto rng = MakeRng();
	Pool32 pool;

	const AoL::I16 min = -32768, max = 12379;
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<AoL::I16>(min, max, rng, pool);
		EXPECT_GE(v, min);
		EXPECT_LE(v, max);
	}
}

TEST(RollRangeSlow_Signed, CompileTimeRegressionCaseDoesNotHang)
{
	auto rng = MakeRng();
	Pool32 pool;

	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<static_cast<AoL::I16>(-32768), static_cast<AoL::I16>(12379)>(rng, pool);
		EXPECT_GE(v, -32768);
		EXPECT_LE(v, 12379);
	}
}

/*********************************************************************************************
* RollDice
*********************************************************************************************/

TEST(RollDice, RuntimeSidesProducesValuesInOneToMaxSidesInclusive)
{
	auto rng = MakeRng();
	Pool32 pool;

	std::array<bool, 6> seen{}; // d6
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollDice(static_cast<AoL::SizeT>(6), rng, pool);
		ASSERT_GE(v, 1u);
		ASSERT_LE(v, 6u);
		seen[v - 1] = true;
	}
	EXPECT_TRUE(seen[0] && seen[1] && seen[2] && seen[3] && seen[4] && seen[5]);
}

TEST(RollDice, CompileTimeSidesProducesValuesInOneToMaxSidesInclusive)
{
	auto rng = MakeRng();
	Pool32 pool;

	std::array<bool, 20> seen{}; // d20
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollDice<static_cast<AoL::SizeT>(20)>(rng, pool);
		ASSERT_GE(v, 1u);
		ASSERT_LE(v, 20u);
		seen[v - 1] = true;
	}
	for (bool b : seen)
	{
		EXPECT_TRUE(b);
	}
}

/*********************************************************************************************
* RollIndex
*********************************************************************************************/

TEST(RollIndex, RuntimeMaxSizeProducesValuesInZeroToMaxSizeInclusive)
{
	auto rng = MakeRng();
	Pool32 pool;

	std::array<bool, 11> seen{}; // [0,10]
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollIndex(static_cast<AoL::SizeT>(10), rng, pool);
		ASSERT_LE(v, 10u);
		seen[v] = true;
	}
	for (bool b : seen)
	{
		EXPECT_TRUE(b);
	}
}

TEST(RollIndex, CompileTimeMaxSizeProducesValuesInZeroToMaxSizeInclusive)
{
	auto rng = MakeRng();
	Pool32 pool;

	std::array<bool, 6> seen{}; // [0,5]
	for (int i = 0; i < kManyTrials; ++i)
	{
		auto v = AoL::Rand::RollIndex<static_cast<AoL::SizeT>(5)>(rng, pool);
		ASSERT_LE(v, 5u);
		seen[v] = true;
	}
	for (bool b : seen)
	{
		EXPECT_TRUE(b);
	}
}