/***************************************************************************************
* RNG algorithm implementations
***************************************************************************************/

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

} // AoL namespace