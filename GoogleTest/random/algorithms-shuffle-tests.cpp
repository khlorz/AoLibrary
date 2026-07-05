/********************************************************************
* Shuffle + PartialShuffle tests
********************************************************************/

#include "pch.h"
#include <numeric>

namespace
{

using AoLRng = AoL::Rand::DefaultGen;
using Pool32 = AoL::Rand::PoolBit<AoL::U64, 32>;

constexpr int many_trials = 200'000;

template<typename T>
bool IsPermutation(std::vector<T> const& a, std::vector<T> const& b)
{
	if (a.size() != b.size()) { return false; }
	auto sa = a, sb = b;
	std::sort(sa.begin(), sa.end());
	std::sort(sb.begin(), sb.end());
	return sa == sb;
}

}

/*********************************************************************************************
* Shuffle - edge cases
*********************************************************************************************/

TEST(Shuffle, EmptyRangeIsNoOp)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::vector<int> v;
	AoL::Shuffle(v.begin(), v.end(), rng, pool);
	EXPECT_TRUE(v.empty());
}

TEST(Shuffle, SingleElementIsNoOp)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::vector<int> v = { 42 };
	AoL::Shuffle(v.begin(), v.end(), rng, pool);
	EXPECT_EQ(v[0], 42);
}

/*********************************************************************************************
* Shuffle - two elements
*********************************************************************************************/

TEST(Shuffle, TwoElementsBothPermutationsPossible)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int ab_count = 0, ba_count = 0;

	for (int i = 0; i < many_trials; ++i)
	{
		std::array<int, 2> v = { 1, 2 };
		AoL::Shuffle(v.begin(), v.end(), rng, pool);
		if (v[0] == 1 && v[1] == 2) { ++ab_count; }
		else if (v[0] == 2 && v[1] == 1) { ++ba_count; }
	}

	EXPECT_GT(ab_count, 0);
	EXPECT_GT(ba_count, 0);
	double ratio = static_cast<double>(ab_count) / many_trials;
	EXPECT_NEAR(ratio, 0.5, 0.02);
}

/*********************************************************************************************
* Shuffle - correctness
*********************************************************************************************/

TEST(Shuffle, ElementsPreservedAfterShuffle)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int trial = 0; trial < 50; ++trial)
	{
		std::vector<int> v(100);
		std::iota(v.begin(), v.end(), 0);
		auto original = v;

		AoL::Shuffle(v.begin(), v.end(), rng, pool);

		EXPECT_TRUE(IsPermutation(v, original));
	}
}

TEST(Shuffle, DeterministicWithSameSeed)
{
	std::vector<int> const input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::vector<int> result_a, result_b;

	{
		auto rng = AoLRng(8888);
		Pool32 pool;
		auto v = input;
		AoL::Shuffle(v.begin(), v.end(), rng, pool);
		result_a = v;
	}

	{
		auto rng = AoLRng(8888);
		Pool32 pool;
		auto v = input;
		AoL::Shuffle(v.begin(), v.end(), rng, pool);
		result_b = v;
	}

	EXPECT_EQ(result_a, result_b);
}

TEST(Shuffle, DifferentSeedDifferentResult)
{
	auto rng1 = AoLRng(1111);
	auto rng2 = AoLRng(2222);
	Pool32 pool1, pool2;

	std::vector<int> v1 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::vector<int> v2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	AoL::Shuffle(v1.begin(), v1.end(), rng1, pool1);
	AoL::Shuffle(v2.begin(), v2.end(), rng2, pool2);

	bool same = (v1 == v2);
	EXPECT_FALSE(same) << "Different seeds produced identical shuffle";
}

TEST(Shuffle, LargeArrayDoesNotCorrupt)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::vector<int> v(10000);
	std::iota(v.begin(), v.end(), 0);
	auto original = v;

	AoL::Shuffle(v.begin(), v.end(), rng, pool);

	EXPECT_TRUE(IsPermutation(v, original));
}

TEST(Shuffle, WorksWithStdArray)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::array<int, 5> v = { 10, 20, 30, 40, 50 };
	auto original = v;

	AoL::Shuffle(v.begin(), v.end(), rng, pool);

	int matches = 0;
	for (size_t i = 0; i < v.size(); ++i)
	{
		if (v[i] == original[i]) { ++matches; }
	}
	EXPECT_LT(matches, 5) << "All elements stayed in place (highly unlikely)";

	std::sort(v.begin(), v.end());
	std::sort(original.begin(), original.end());
	EXPECT_EQ(v, original);
}

/*********************************************************************************************
* Shuffle - uniformity
*********************************************************************************************/

TEST(Shuffle, EachPositionGetsEachElementApproximatelyUniformly)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<int, 5> const tokens = { 0, 1, 2, 3, 4 };
	int counts[5][5] = {};

	for (int trial = 0; trial < many_trials; ++trial)
	{
		auto v = tokens;
		AoL::Shuffle(v.begin(), v.end(), rng, pool);
		for (int pos = 0; pos < 5; ++pos)
		{
			++counts[pos][v[pos]];
		}
	}

	double expected = static_cast<double>(many_trials) / 5;
	for (int pos = 0; pos < 5; ++pos)
	{
		for (int val = 0; val < 5; ++val)
		{
			EXPECT_NEAR(static_cast<double>(counts[pos][val]) / expected, 1.0, 0.08);
		}
	}
}

/*********************************************************************************************
* PartialShuffle - edge cases
*********************************************************************************************/

TEST(PartialShuffle, KZeroIsNoOp)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::vector<int> v = { 1, 2, 3, 4, 5 };
	auto original = v;
	AoL::PartialShuffle(v.begin(), v.begin(), v.end(), rng, pool);
	EXPECT_EQ(v, original);
}

TEST(PartialShuffle, KEqualsNIsFullShuffle)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::vector<int> v(10);
	std::iota(v.begin(), v.end(), 0);
	auto original = v;

	AoL::PartialShuffle(v.begin(), v.end(), v.end(), rng, pool);

	EXPECT_TRUE(IsPermutation(v, original));
}

TEST(PartialShuffle, EmptyRangeIsNoOp)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::vector<int> v;
	AoL::PartialShuffle(v.begin(), v.begin(), v.end(), rng, pool);
	EXPECT_TRUE(v.empty());
}

TEST(PartialShuffle, SingleElementIsNoOp)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::vector<int> v = { 99 };
	AoL::PartialShuffle(v.begin(), v.end(), v.end(), rng, pool);
	EXPECT_EQ(v[0], 99);
}

/*********************************************************************************************
* PartialShuffle - correctness
*********************************************************************************************/

TEST(PartialShuffle, FirstKAreRandomSubset)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int const n = 20, k = 5;

	std::vector<int> full_set(n);
	std::iota(full_set.begin(), full_set.end(), 0);

	for (int trial = 0; trial < 100; ++trial)
	{
		auto v = full_set;
		AoL::PartialShuffle(v.begin(), v.begin() + k, v.end(), rng, pool);

		std::vector<int> first_k(v.begin(), v.begin() + k);
		std::vector<int> last_nk(v.begin() + k, v.end());

		auto combined = first_k;
		combined.insert(combined.end(), last_nk.begin(), last_nk.end());
		std::sort(combined.begin(), combined.end());

		EXPECT_EQ(combined, full_set);
	}
}

TEST(PartialShuffle, KOnePicksRandomElement)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<int, 5> const base = { 10, 20, 30, 40, 50 };
	int first_pos_counts[5] = {};

	for (int trial = 0; trial < many_trials; ++trial)
	{
		auto v = base;
		AoL::PartialShuffle(v.begin(), v.begin() + 1, v.end(), rng, pool);
		for (int j = 0; j < 5; ++j)
		{
			if (v[0] == base[j]) { ++first_pos_counts[j]; }
		}
	}

	for (int c : first_pos_counts)
	{
		double ratio = static_cast<double>(c) / many_trials;
		EXPECT_NEAR(ratio, 0.2, 0.02);
	}
}

TEST(PartialShuffle, DeterministicWithSameSeed)
{
	std::vector<int> const input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	std::vector<int> result_a, result_b;

	{
		auto rng = AoLRng(7777);
		Pool32 pool;
		auto v = input;
		AoL::PartialShuffle(v.begin(), v.begin() + 4, v.end(), rng, pool);
		result_a = v;
	}

	{
		auto rng = AoLRng(7777);
		Pool32 pool;
		auto v = input;
		AoL::PartialShuffle(v.begin(), v.begin() + 4, v.end(), rng, pool);
		result_b = v;
	}

	EXPECT_EQ(result_a, result_b);
}

TEST(PartialShuffle, RemainingElementsAreComplementOfFirstK)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::vector<int> const base = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	for (int trial = 0; trial < 50; ++trial)
	{
		auto v = base;
		AoL::PartialShuffle(v.begin(), v.begin() + 3, v.end(), rng, pool);

		std::vector<int> first_3(v.begin(), v.begin() + 3);
		std::vector<int> last_7(v.begin() + 3, v.end());

		auto combined = first_3;
		combined.insert(combined.end(), last_7.begin(), last_7.end());
		std::sort(combined.begin(), combined.end());

		EXPECT_EQ(combined, base);
	}
}
