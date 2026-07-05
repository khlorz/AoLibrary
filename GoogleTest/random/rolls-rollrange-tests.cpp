/********************************************************************
* RollRange + RollRangeSlow tests
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
* RollRange
*********************************************************************************************/

TEST(RollRange_Unsigned, CompileTimeBoundsRespected)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<static_cast<AoL::U32>(5), static_cast<AoL::U32>(15)>(rng, pool);
		EXPECT_GE(v, 5u);
		EXPECT_LE(v, 15u);
	}
}

TEST(RollRange_Unsigned, RuntimeBoundsRespected)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	AoL::U32 const min = 100, max = 200;
	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::U32>(min, max, rng, pool);
		EXPECT_GE(v, min);
		EXPECT_LE(v, max);
	}
}

TEST(RollRange_Signed, CompileTimeBoundsRespectedAcrossZero)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<static_cast<AoL::I32>(-10), static_cast<AoL::I32>(10)>(rng, pool);
		EXPECT_GE(v, -10);
		EXPECT_LE(v, 10);
	}
}

TEST(RollRange_Signed, RuntimeBoundsRespectedAcrossZero)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	AoL::I32 const min = -500, max = -100;
	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::I32>(min, max, rng, pool);
		EXPECT_GE(v, min);
		EXPECT_LE(v, max);
	}
}

TEST(RollRange_Float, CompileTimeBoundsRespected)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<0.0f, 1.0f>(rng, pool);
		EXPECT_GE(v, 0.0f);
		EXPECT_LT(v, 1.0f);
	}
}

/*********************************************************************************************
* RollRange bias / chi-squared tests
*********************************************************************************************/

namespace
{

double ChiSquaredCritical(int df, double alpha)
{
	if (df == 2)  return alpha < 0.001 ? 13.82 : 9.21;
	// Wilson–Hilferty approximation for larger df
	double const z = alpha < 0.001 ? 3.09 : 2.33;
	double const x = z * sqrt(2.0 * df) + 2.0 * (z * z - 1.0) / 3.0;
	return df + x;
}

double ChiSquared(std::vector<int> const& hist, double expected)
{
	double cs = 0.0;
	for (int c : hist)
	{
		double d = static_cast<double>(c) - expected;
		cs += d * d / expected;
	}
	return cs;
}

}

TEST(RollRange_Bias, SmallRange_0_2)
{
	auto rng = AoLRng(12345);
	Pool32 pool;
	AoL::U32 const min = 0, max = 2;
	int const range = static_cast<int>(max - min + 1);
	std::vector<int> hist(range, 0);

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::U32>(min, max, rng, pool);
		++hist[static_cast<int>(v)];
	}

	double expected = static_cast<double>(many_trials) / range;
	double cs = ChiSquared(hist, expected);
	EXPECT_LT(cs, ChiSquaredCritical(2, 0.001));
}

TEST(RollRange_Bias, MediumRange_half_of_U16)
{
	auto rng = AoLRng(12345);
	Pool32 pool;
	AoL::U32 const min = 0, max = 32767;
	int const range = static_cast<int>(max - min + 1);
	std::vector<int> hist(range, 0);

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::U32>(min, max, rng, pool);
		++hist[static_cast<int>(v)];
	}

	double expected = static_cast<double>(many_trials) / range;
	double cs = ChiSquared(hist, expected);
	EXPECT_LT(cs, ChiSquaredCritical(range - 1, 0.001));
}

TEST(RollRange_Bias, LargeRange_full_U16)
{
	auto rng = AoLRng(12345);
	Pool32 pool;
	AoL::U32 const min = 0, max = 65535;
	int const range = static_cast<int>(max - min + 1);
	std::vector<int> hist(range, 0);

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRange<AoL::U32>(min, max, rng, pool);
		++hist[static_cast<int>(v)];
	}

	double expected = static_cast<double>(many_trials) / range;
	double cs = ChiSquared(hist, expected);
	EXPECT_LT(cs, ChiSquaredCritical(range - 1, 0.001));
}

/*********************************************************************************************
* RollRangeSlow
*********************************************************************************************/

TEST(RollRangeSlow, HandlesFullSpanOfU8WithoutHanging)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<AoL::U8>(
			static_cast<AoL::U8>(0), static_cast<AoL::U8>(255), rng, pool);
		EXPECT_LE(v, 255);
	}
}

TEST(RollRangeSlow_Signed, RegressionCaseDoesNotHang)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	AoL::I16 const min = -32768, max = 12379;
	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollRangeSlow<AoL::I16>(min, max, rng, pool);
		EXPECT_GE(v, min);
		EXPECT_LE(v, max);
	}
}
