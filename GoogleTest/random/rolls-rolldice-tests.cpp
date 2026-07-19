/********************************************************************
* RollDice tests
********************************************************************/

#include "pch.h"

#include "aol/randoms.h"

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
* RollDice
*********************************************************************************************/

TEST(RollDice, RuntimeSidesProducesValuesInOneToMaxSidesInclusive)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<bool, 6> seen{};
	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollDice(Cs(6), rng, pool);
		ASSERT_GE(v, 1u);
		ASSERT_LE(v, 6u);
		seen[v - 1] = true;
	}
	for (bool b : seen)
	{
		EXPECT_TRUE(b);
	}
}

TEST(RollDice_NoPool, RuntimeSidesProducesValuesInOneToMaxSidesInclusive)
{
	auto rng = AoLRng(12345);

	std::array<bool, 6> seen{};
	for (int i = 0; i < many_trials; ++i)
	{
		auto v = AoL::Rand::RollDice(Cs(6), rng);
		ASSERT_GE(v, 1u);
		ASSERT_LE(v, 6u);
		seen[v - 1] = true;
	}
	for (bool b : seen)
	{
		EXPECT_TRUE(b);
	}
}
