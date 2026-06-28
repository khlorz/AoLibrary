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
template<typename RNGSizeT, AoL::U64 BitSize>
struct PoolBit
{
	using RNGSizeType = RNGSizeT;
	using OutputType = std::conditional_t<BitSize <= 8, AoL::U8,
					   std::conditional_t<BitSize <= 16, AoL::U16,
					   std::conditional_t<BitSize <= 32, AoL::U32, AoL::U64>>>;

	static constexpr const AoL::U64 RNGBitSize = sizeof(RNGSizeType) * 8;
	static constexpr const AoL::U64 OutputBitSize = BitSize;

	static_assert(RNGBitSize == 64, "PoolBit currently assumes a 64-bit RNG word type!");
	static_assert(OutputBitSize >= 1 && OutputBitSize <= (RNGBitSize / 8), "BitSize must be between one and half the total bit size of the RNGSizeType!");
	static_assert(RNGBitSize % OutputBitSize == 0, "BitSize must divide RNGBitSize evenly!");

	static constexpr const RNGSizeType mask = (static_cast<RNGSizeType>(1) << OutputBitSize) - 1;
	static constexpr const AoL::U8 max_count = static_cast<AoL::U8>(RNGBitSize / OutputBitSize);

	RNGSizeType rand_pool;
	AoL::U8 counter;

	PoolBit() :
		rand_pool{ static_cast<RNGSizeType>(0) },
		counter{ max_count }
	{}

	template<typename RNG>
	PoolBit(RNG& rng) :
		rand_pool{ rng() },
		counter{ 0 }
	{
		static_assert(std::is_same_v<std::remove_cvref_t<decltype(rng())>, RNGSizeType>, "RNG::operator() must return RNGSizeType!");
	}

	template<typename RNG>
	OutputType Next(RNG& rng) noexcept
	{
		static_assert(std::is_same_v<std::remove_cvref_t<decltype(rng())>, RNGSizeType>, "RNG::operator() must return RNGSizeType!");

		if (counter == max_count) [[unlikely]]
		{
			rand_pool = rng();
			counter = 0;
		}

		const OutputType ret_val = static_cast<OutputType>(rand_pool & mask);
		rand_pool >>= OutputBitSize;
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
	using RNGSizeType = I;

	static constexpr const RNGSizeType sentinel_mask = static_cast<RNGSizeType>(1) << (sizeof(RNGSizeType) * 8 - 1);
	static constexpr const AoL::U64 OutputBitSize = 1;

	RNGSizeType rand_pool;

	PoolBit() :
		rand_pool{ sentinel_mask }
	{}

	template<typename RNG>
	PoolBit(RNG& rng) :
		rand_pool{ rng() | sentinel_mask }
	{
		static_assert(std::is_same_v<std::remove_cvref_t<decltype(rng())>, RNGSizeType>, "RNG::operator() must return RNGSizeType!");
	}

	template<typename RNG>
	bool Next(RNG& rng) noexcept
	{
		static_assert(std::is_same_v<std::remove_cvref_t<decltype(rng())>, RNGSizeType>, "RNG::operator() must return RNGSizeType!");

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