/********************************************************************
* RandomPartition + StableRandomPartition tests
********************************************************************/

#include "pch.h"

#include "aol/randoms.h"

#include <numeric>
#include <list>

namespace
{

using AoLRng = AoL::Rand::DefaultGen;
using Pool32 = AoL::Rand::PoolBit<AoL::U64, 32>;

constexpr int many_trials = 200'000;

}

/*********************************************************************************************
* RandomPartition - edge cases
*********************************************************************************************/

TEST(RandomPartition, EmptyRangeReturnsBegin)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::vector<int> v;
	auto mid = AoL::RandomPartition(v.begin(), v.end(), rng, pool);
	EXPECT_EQ(mid, v.begin());
}

TEST(RandomPartition, SingleElementCanGoLeftOrRight)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int left_count = 0, right_count = 0;

	for (int i = 0; i < many_trials; ++i)
	{
		std::array<int, 1> v = { 42 };
		auto mid = AoL::RandomPartition(v.begin(), v.end(), rng, pool);
		if (mid == v.end()) { ++left_count; }
		else { ++right_count; }
	}

	double ratio = static_cast<double>(left_count) / many_trials;
	EXPECT_NEAR(ratio, 0.5, 0.02);
}

TEST(RandomPartition, SingleElementPreservesValue)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::array<int, 1> v = { 99 };
	auto mid = AoL::RandomPartition(v.begin(), v.end(), rng, pool);
	EXPECT_EQ(v[0], 99);
	EXPECT_TRUE(mid == v.begin() || mid == v.end());
}

/*********************************************************************************************
* RandomPartition - correctness
*********************************************************************************************/

TEST(RandomPartition, ElementsPreservedAfterPartition)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int trial = 0; trial < 50; ++trial)
	{
		std::vector<int> v(20);
		std::iota(v.begin(), v.end(), 0);
		auto original = v;

		auto mid = AoL::RandomPartition(v.begin(), v.end(), rng, pool);

		std::vector<int> left(v.begin(), mid);
		std::vector<int> right(mid, v.end());

		auto combined = left;
		combined.insert(combined.end(), right.begin(), right.end());

		std::sort(combined.begin(), combined.end());
		EXPECT_EQ(combined, original);
	}
}

TEST(RandomPartition, PartitionPointIsValid)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int trial = 0; trial < 100; ++trial)
	{
		std::vector<int> v(15);
		std::iota(v.begin(), v.end(), 0);

		auto mid = AoL::RandomPartition(v.begin(), v.end(), rng, pool);

		EXPECT_GE(mid, v.begin());
		EXPECT_LE(mid, v.end());
	}
}

TEST(RandomPartition, ApproximatelyFiftyPercentSplit)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int total_left = 0;

	for (int trial = 0; trial < many_trials; ++trial)
	{
		std::array<int, 10> v = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		auto mid = AoL::RandomPartition(v.begin(), v.end(), rng, pool);
		total_left += static_cast<int>(mid - v.begin());
	}

	double avg_left = static_cast<double>(total_left) / many_trials;
	EXPECT_NEAR(avg_left, 5.0, 0.05);
}
TEST(RandomPartition, EachElementHasFiftyPercentChanceLeft)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::array<int, 5> const base = { 10, 20, 30, 40, 50 };
	int left_counts[5] = {};

	for (int trial = 0; trial < many_trials; ++trial)
	{
		auto v = base;
		auto mid = AoL::RandomPartition(v.begin(), v.end(), rng, pool);
		auto mid_idx = static_cast<AoL::SizeT>(mid - v.begin());

		for (AoL::SizeT pos = 0; pos < mid_idx; ++pos)
		{
			for (int j = 0; j < 5; ++j)
			{
				if (v[pos] == base[j]) { ++left_counts[j]; }
			}
		}
	}

	for (int c : left_counts)
	{
		double ratio = static_cast<double>(c) / many_trials;
		EXPECT_NEAR(ratio, 0.5, 0.03);
	}
}

TEST(RandomPartition, DeterministicWithSameSeed)
{
	std::vector<int> result_a, result_b;

	{
		auto rng = AoLRng(3333);
		Pool32 pool;
		std::vector<int> v = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		AoL::RandomPartition(v.begin(), v.end(), rng, pool);
		result_a = v;
	}

	{
		auto rng = AoLRng(3333);
		Pool32 pool;
		std::vector<int> v = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		AoL::RandomPartition(v.begin(), v.end(), rng, pool);
		result_b = v;
	}

	EXPECT_EQ(result_a, result_b);
}

/*********************************************************************************************
* StableRandomPartition - edge cases
*********************************************************************************************/

TEST(StableRandomPartition, EmptyRangeReturnsBegin)
{
	auto rng = AoLRng(42);
	Pool32 pool;

	std::vector<int> v;
	auto mid = AoL::StableRandomPartition(v.begin(), v.end(), rng, pool);
	EXPECT_EQ(mid, v.begin());
}

TEST(StableRandomPartition, SingleElementCanGoLeftOrRight)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int left_count = 0, right_count = 0;

	for (int i = 0; i < many_trials; ++i)
	{
		std::array<int, 1> v = { 42 };
		auto mid = AoL::StableRandomPartition(v.begin(), v.end(), rng, pool);
		if (mid == v.end()) { ++left_count; }
		else { ++right_count; }
	}

	double ratio = static_cast<double>(left_count) / many_trials;
	EXPECT_NEAR(ratio, 0.5, 0.02);
}

/*********************************************************************************************
* StableRandomPartition - correctness
*********************************************************************************************/

TEST(StableRandomPartition, ElementsPreservedAfterPartition)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int trial = 0; trial < 50; ++trial)
	{
		std::vector<int> v(20);
		std::iota(v.begin(), v.end(), 0);
		auto original = v;

		auto mid = AoL::StableRandomPartition(v.begin(), v.end(), rng, pool);

		std::vector<int> left(v.begin(), mid);
		std::vector<int> right(mid, v.end());

		auto combined = left;
		combined.insert(combined.end(), right.begin(), right.end());

		std::sort(combined.begin(), combined.end());
		EXPECT_EQ(combined, original);
	}
}

TEST(StableRandomPartition, PartitionPointIsValid)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int trial = 0; trial < 100; ++trial)
	{
		std::vector<int> v(15);
		std::iota(v.begin(), v.end(), 0);

		auto mid = AoL::StableRandomPartition(v.begin(), v.end(), rng, pool);

		EXPECT_GE(mid, v.begin());
		EXPECT_LE(mid, v.end());
	}
}

TEST(StableRandomPartition, ApproximatelyFiftyPercentSplit)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	int total_left = 0;

	for (int trial = 0; trial < many_trials; ++trial)
	{
		std::array<int, 10> v = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		auto mid = AoL::StableRandomPartition(v.begin(), v.end(), rng, pool);
		total_left += static_cast<int>(mid - v.begin());
	}

	double avg_left = static_cast<double>(total_left) / many_trials;
	EXPECT_NEAR(avg_left, 5.0, 0.05);
}

TEST(StableRandomPartition, PreservesRelativeOrderWithinEachSide)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int trial = 0; trial < 100; ++trial)
	{
		std::vector<int> v(12);
		std::iota(v.begin(), v.end(), 0);

		std::vector<int> original(v);

		auto mid = AoL::StableRandomPartition(v.begin(), v.end(), rng, pool);

		std::vector<int> left(v.begin(), mid);
		std::vector<int> right(mid, v.end());

		std::vector<int> left_orig, right_orig;

		for (size_t pos = 0; pos < original.size(); ++pos)
		{
			if (std::find(left.begin(), left.end(), original[pos]) != left.end())
			{
				left_orig.push_back(original[pos]);
			}
			else
			{
				right_orig.push_back(original[pos]);
			}
		}

		EXPECT_EQ(left, left_orig) << "Left side order not preserved";
		EXPECT_EQ(right, right_orig) << "Right side order not preserved";
	}
}

TEST(StableRandomPartition, DeterministicWithSameSeed)
{
	std::vector<int> result_a, result_b;

	{
		auto rng = AoLRng(3333);
		Pool32 pool;
		std::vector<int> v = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		AoL::StableRandomPartition(v.begin(), v.end(), rng, pool);
		result_a = v;
	}

	{
		auto rng = AoLRng(3333);
		Pool32 pool;
		std::vector<int> v = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		AoL::StableRandomPartition(v.begin(), v.end(), rng, pool);
		result_b = v;
	}

	EXPECT_EQ(result_a, result_b);
}

TEST(StableRandomPartition, OrderPreservedWithDistinctElements)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	for (int trial = 0; trial < 200; ++trial)
	{
		std::vector<int> v(10);
		std::iota(v.begin(), v.end(), 0);

		auto orig = v;
		auto mid = AoL::StableRandomPartition(v.begin(), v.end(), rng, pool);

		std::vector<int> left(v.begin(), mid);
		std::vector<int> right(mid, v.end());

		std::vector<int> expected_left, expected_right;
		for (int x : orig)
		{
			if (std::find(left.begin(), left.end(), x) != left.end())
			{
				expected_left.push_back(x);
			}
			else
			{
				expected_right.push_back(x);
			}
		}

		EXPECT_EQ(left, expected_left);
		EXPECT_EQ(right, expected_right);
	}
}

TEST(StableRandomPartition, WorksWithForwardIteratorList)
{
	auto rng = AoLRng(12345);
	Pool32 pool;

	std::list<int> lst = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };

	std::vector<int> orig(lst.begin(), lst.end());

	auto mid = AoL::StableRandomPartition(lst.begin(), lst.end(), rng, pool);

	std::vector<int> left(lst.begin(), mid);
	std::vector<int> right(mid, lst.end());

	auto combined = left;
	combined.insert(combined.end(), right.begin(), right.end());

	std::sort(combined.begin(), combined.end());
	std::sort(orig.begin(), orig.end());
	EXPECT_EQ(combined, orig);
}
