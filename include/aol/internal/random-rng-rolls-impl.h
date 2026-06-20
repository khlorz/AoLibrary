/***************************************************************************************
* RNG rolls implementations
***************************************************************************************/

namespace AoL
{

namespace Rand
{

namespace Internal
{

template<AoL::SizeT ChanceScale, AoL::SizeT BitSize>
consteval auto MakeThresholdTable() noexcept
{
	static_assert(ChanceScale % 10 == 0, "Must be divisible by 10!");
	static_assert(BitSize >= 2 && BitSize <= 32, "BitSize must be between 2 and 32!");
	static_assert(64 % BitSize == 0, "BitSize must divide 64 evenly!");

	using table_value_t = std::conditional_t<BitSize <= 8, AoL::U8,
						  std::conditional_t<BitSize <= 16, AoL::U16,
						  std::conditional_t<BitSize <= 32, AoL::U32, AoL::U64>>>;

	using wide_table_value_t = std::conditional_t<BitSize <= 8, AoL::U16,
							   std::conditional_t<BitSize <= 16, AoL::U32, AoL::U64>>;

	std::array<table_value_t, ChanceScale + 1> table{};

	constexpr AoL::U64 max_rng_value = (1ull << BitSize) - 1ull;

	for (AoL::SizeT i = 0; i <= ChanceScale; ++i)
	{
		table[i] = static_cast<table_value_t>((static_cast<wide_table_value_t>(i) * (static_cast<wide_table_value_t>(max_rng_value) + 1)) / ChanceScale);
	}

	return table;
}

template<AoL::SizeT ChanceScale, AoL::SizeT BitSize>
constexpr const auto& GetThresholdTable() noexcept
{
	static constexpr auto threshold_table = MakeThresholdTable<ChanceScale, BitSize>();
	return threshold_table;
}

} // Internal namespace

/**
* @details Rolls a chance-based RNG check using a precomputed threshold table
* 
* - Draws one chunk from `pool` and compares it against a threshold derived from ChanceScale
* 
* - 'chance' is expressed on a scale of ChanceScale
* 
* - On a ChanceScale of 1000, 50% would be 500. On 10000, 50.50% would be 5050
* 
* - The higher the ChanceScale, the more strict the threshold will be
*
* @tparam ChanceScale the denominator that defines the granularity of the chance scale
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
*
* @param chance the chance value with respect to the ChanceScale
* @param rng the random number generator object
* @param pool the pool object
*
* @return true if the roll succeeded
*/
template<AoL::SizeT ChanceScale = 10000, typename RNG, typename Pool>
constexpr bool RollChance(AoL::SizeT chance, RNG& rng, Pool& pool) noexcept
{
	const auto& threshold_table = Internal::GetThresholdTable<ChanceScale, Pool::pool_bit_size>();
	return pool.Next(rng) < threshold_table[chance];
}

/**
* @details Rolls a chance-based RNG check using a precomputed threshold table
*
* - A convenience function for users who just wants a simple roll
*
* - Notably slower than if you pass your own pool object, but it's just for convenience, not for hot paths
*
* @tparam ChanceScale the denominator that defines the granularity of the chance scale
* @param chance the chance value with respect to the ChanceScale
* @return true if the roll succeeded
*/
template<AoL::SizeT ChanceScale = 10000, typename RNG>
constexpr bool RollChance(AoL::SizeT chance, RNG& rng) noexcept
{
	return RollChance<ChanceScale>(chance, rng, Internal::DefaultPool<16>(rng));
}

/**
* @details Rolls a chance-based RNG check using a precomputed threshold table
* 
* - A convenience function for users who just wants a simple roll
* 
* - Notably slower than if you pass your own rng and pool object, but it's just for convenience, not for hot paths
*
* @tparam ChanceScale the denominator that defines the granularity of the chance scale
* @param chance the chance value with respect to the ChanceScale
* @return true if the roll succeeded
*/
template<AoL::SizeT ChanceScale = 10000>
constexpr bool RollChance(AoL::SizeT chance) noexcept
{
	return RollChance<ChanceScale>(chance, Internal::DefaultRNG(), Internal::DefaultPool<16, DefaultGen>());
}

/**
* @details Flip a flippin coin
* 
* - 50% chance to land on heads or tails
*
* @tparam RNG the random number generator type
* 
* @param rng the random number generator object
* @param pool the pool object
*
* @return true if the roll succeeded
*/
template<typename RNG>
constexpr bool FlipCoin(RNG& rng, PoolBit1<decltype(std::declval<RNG&>()())>& pool) noexcept
{
	return pool.Next(rng);
}

/**
* @details Flip a flippin coin
*
* - 50% chance to land on heads or tails
*
* - Notably slower than if you pass your own pool object, but it's just for convenience, not for hot paths
*
* @return true if the roll succeeded
*/
template<typename RNG>
constexpr bool FlipCoin(RNG& rng) noexcept
{
	return Internal::DefaultPool<1>(rng).Next(rng);
}

/**
* @details Flip a flippin coin
*
* - 50% chance to land on heads or tails
* 
* - Notably slower than if you pass your own rng and pool object, but it's just for convenience, not for hot paths
*
* @return true if the roll succeeded
*/
inline bool FlipCoin() noexcept
{
	return Internal::DefaultPool<1, DefaultGen>().Next(Internal::DefaultRNG());
}

} // Rand namespace

} // AoL namespace