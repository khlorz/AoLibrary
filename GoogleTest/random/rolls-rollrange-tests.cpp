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
