/********************************************************************
* RollIndex tests
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
* RollIndex
*********************************************************************************************/

TEST(RollIndex, RuntimeMaxSizeProducesValuesInZeroToMaxSizeInclusive)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<bool, 11> seen{};
	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollIndex(Cs(10), rng, pool);
		ASSERT_LE(v, 10u);
		seen[v] = true;
	}
	for (bool b : seen)
	{
		EXPECT_TRUE(b);
	}
}

TEST(RollIndex_NoPool, RuntimeMaxSizeProducesValuesInZeroToMaxSizeInclusive)
{
	auto rng = AoLRng(12345);

	std::array<bool, 11> seen{};
	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollIndex(Cs(10), rng);
		ASSERT_LE(v, 10u);
		seen[v] = true;
	}
	for (bool b : seen)
	{
		EXPECT_TRUE(b);
	}
}
