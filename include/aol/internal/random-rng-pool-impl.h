/***************************************************************************************
* RNG Pooling implementations
***************************************************************************************/

namespace AoL
{

namespace Rand
{

/**
* @details RNG Pooling
*
* - For pooling the RNG bits to be efficient with its bits
*
* @tparam I RNG size type
* @tparam BitSize bit size for pooling the RNG (i.e. 2, 4, 8, etc.)
*/
template<typename I, AoL::U64 BitSize>
struct PoolBit
{
	using rng_size_type = I;
	using output_type = std::conditional_t<BitSize <= 8, AoL::U8,
						std::conditional_t<BitSize <= 16, AoL::U16,
						std::conditional_t<BitSize <= 32, AoL::U32, AoL::U64>>>;

	static_assert(sizeof(rng_size_type) * 8 == 64, "PoolBit currently assumes a 64-bit RNG word type!");
	static_assert(BitSize >= 1 && BitSize <= 32, "BitSize must be between 1 and 32!");
	static_assert(64 % BitSize == 0, "BitSize must divide 64 evenly!");

	static constexpr const rng_size_type mask = (static_cast<rng_size_type>(1) << BitSize) - 1;
	static constexpr const AoL::U8 max_count = static_cast<AoL::U8>(64 / BitSize);

	rng_size_type rand_pool;
	AoL::U8 counter;

	PoolBit() :
		rand_pool{ static_cast<rng_size_type>(0) },
		counter{ max_count }
	{}

	template<typename RNG>
	PoolBit(RNG& rng) :
		rand_pool{ rng() },
		counter{ 0 }
	{
		static_assert(std::is_same_v<std::remove_cvref_t<decltype(rng())>, rng_size_type>, "RNG::operator() must return rng_size_type!");
	}

	template<typename RNG>
	output_type Next(RNG& rng) noexcept
	{
		static_assert(std::is_same_v<std::remove_cvref_t<decltype(rng())>, rng_size_type>, "RNG::operator() must return rng_size_type!");

		if (counter == max_count) [[unlikely]]
		{
			rand_pool = rng();
			counter = 0;
		}

		const output_type ret_val = static_cast<output_type>(rand_pool & mask);
		rand_pool >>= BitSize;
		++counter;
		return ret_val;
	}
};

/**
* @details RNG Pooling for 1 bit
*
* - Specialize specifically for 1 bit pooling
*
* @tparam I RNG size type
*/
template<typename I>
struct PoolBit<I, 1>
{
	using rng_size_type = I;

	static constexpr const rng_size_type sentinel_mask = static_cast<rng_size_type>(1) << (sizeof(rng_size_type) * 8 - 1);

	rng_size_type rand_pool;

	PoolBit() :
		rand_pool{ sentinel_mask }
	{}

	template<typename RNG>
	PoolBit(RNG& rng) :
		rand_pool{ rng() | sentinel_mask }
	{
		static_assert(std::is_same_v<std::remove_cvref_t<decltype(rng())>, rng_size_type>, "RNG::operator() must return rng_size_type!");
	}

	template<typename RNG>
	bool Next(RNG& rng) noexcept
	{
		static_assert(std::is_same_v<std::remove_cvref_t<decltype(rng())>, rng_size_type>, "RNG::operator() must return rng_size_type!");

		if (rand_pool == 1) [[unlikely]]
		{
			rand_pool = rng() | sentinel_mask;
		}

		const bool ret_val = rand_pool & 1;
		rand_pool >>= 1;
		return ret_val;
	}
};

template<typename I = AoL::U64>
using PoolBit1 = PoolBit<I, 1>;
template<typename I = AoL::U64>
using PoolBit2 = PoolBit<I, 2>;
template<typename I = AoL::U64>
using PoolBit4 = PoolBit<I, 4>;
template<typename I = AoL::U64>
using PoolBit8 = PoolBit<I, 8>;
template<typename I = AoL::U64>
using PoolBit16 = PoolBit<I, 16>;
template<typename I = AoL::U64>
using PoolBit32 = PoolBit<I, 32>;

} // Rand namespace

} // AoL namespace

// EOF