/***************************************************************************************
* RNG algorithm implementations
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_RANDOMS_ALGORITHMS_H
#define AOL_HEADER_INTERNAL_RANDOMS_ALGORITHMS_H


#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/traits.h"
#include "aol/types.h"


namespace AoL
{

namespace Rand
{

} // Rand namespace

/***************************************************
* Fisher-Yates shuffle
***************************************************/

/**
* @details Shuffles a range using the Fisher-Yates algorithm.
*
* - Produces a uniformly random permutation in O(n) time with O(1) extra space.
*
* - Empty and single-element ranges are no-ops.
*
* @tparam It the random access iterator type
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
* @param begin iterator to the beginning of the range
* @param end iterator to the end of the range
* @param rng the random number generator object
* @param pool the pool object
*/
template<std::random_access_iterator It, typename RNG, typename Pool>
constexpr void Shuffle(It begin, It end, RNG& rng, Pool& pool) noexcept
{
	auto n = static_cast<AoL::SizeT>(end - begin);

	if (n < 2)
	{
		return;
	}

	for (AoL::SizeT i = n - 1; i > 0; --i)
	{
		auto j = AoL::Rand::RollRange(static_cast<AoL::SizeT>(0), i, rng, pool);
		std::swap(begin[i], begin[j]);
	}
}

/***************************************************
* Partial shuffle
***************************************************/

/**
* @details Shuffles only the first k elements of a range using partial Fisher-Yates.
*
* - After the call, the first k positions contain a uniformly random selection of k
*   elements from the full range, in random order. The remaining n-k elements are
*   the leftovers (not fully shuffled).
*
* - Useful for "pick N and leave the rest" scenarios: card hands, team drafts.
*
* - When mid == end, equivalent to a full Shuffle.
*
* - Empty and single-element ranges are no-ops.
*
* @tparam It the random access iterator type
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
* @param begin iterator to the beginning of the range
* @param mid iterator to the (k+1)th element; the first k = mid-begin elements are shuffled
* @param end iterator to the end of the range
* @param rng the random number generator object
* @param pool the pool object
*/
template<std::random_access_iterator It, typename RNG, typename Pool>
constexpr void PartialShuffle(It begin, It mid, It end, RNG& rng, Pool& pool) noexcept
{
	auto n = static_cast<AoL::SizeT>(end - begin);
	auto k = static_cast<AoL::SizeT>(mid - begin);

	if (n < 2 || k == 0)
	{
		return;
	}

	for (AoL::SizeT i = 0; i < k; ++i)
	{
		auto j = AoL::Rand::RollRange(i, n - 1, rng, pool);
		std::swap(begin[i], begin[j]);
	}
}

/***************************************************
* Random partition
***************************************************/

/**
* @details Unstable random partition of a range.
*
* - Each element independently moves to the left side with 50% probability.
*
* - Does not preserve relative order within each side (see StableRandomPartition).
*
* - Returns the partition point: iterator to the first element in the right side.
*
* @tparam It the forward iterator type
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
* @param begin iterator to the beginning of the range
* @param end iterator to the end of the range
* @param rng the random number generator object
* @param pool the pool object
* @return an iterator to the first element that went right (partition point)
*/
template<std::forward_iterator It, typename RNG, typename Pool>
constexpr It RandomPartition(It begin, It end, RNG& rng, Pool& pool) noexcept
{
	auto mid = begin;

	for (auto it = begin; it != end; ++it)
	{
		if (pool.Next(rng) & static_cast<typename Pool::OutputType>(1))
		{
			std::swap(*mid, *it);
			++mid;
		}
	}

	return mid;
}

/***************************************************
* Stable random partition
***************************************************/

/**
* @details Stable random partition of a range.
*
* - Each element independently moves to the left side with 50% probability.
*
* - Preserves relative order within each side.
*
* - Uses divide-and-conquer with rotate: O(n log n) time, O(log n) stack space.
*
* @tparam It the forward iterator type
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
* @param begin iterator to the beginning of the range
* @param end iterator to the end of the range
* @param rng the random number generator object
* @param pool the pool object
* @return an iterator to the first element that went right (partition point)
*/
template<std::forward_iterator It, typename RNG, typename Pool>
constexpr It StableRandomPartition(It begin, It end, RNG& rng, Pool& pool) noexcept
{
	auto dist = static_cast<AoL::SizeT>(std::distance(begin, end));

	if (dist == 0) { return begin; }

	if (dist == 1)
	{
		bool goes_left = (pool.Next(rng) & static_cast<typename Pool::OutputType>(1));
		return goes_left ? end : begin;
	}

	auto mid = begin;
	std::advance(mid, static_cast<AoL::PtrDiff>(dist / 2));

	auto left_false = StableRandomPartition(begin, mid, rng, pool);
	auto right_false = StableRandomPartition(mid, end, rng, pool);

	return std::rotate(left_false, mid, right_false);
}

} // AoL namespace


#endif // AOL_HEADER_INTERNAL_RANDOMS_ALGORITHMS_H