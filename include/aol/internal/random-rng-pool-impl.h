/***************************************************************************************
* RNG Pooling implementations
***************************************************************************************/

namespace AoL
{

namespace Rand
{

namespace Internal
{

template<AoL::U64 BitSize>
struct PoolBit_Constants
{
private:
	using SizeType = std::conditional_t<BitSize <= 8, AoL::U8,
					 std::conditional_t<BitSize <= 16, AoL::U16,
					 std::conditional_t<BitSize <= 32, AoL::U32, AoL::U64>>>;

public:
	static constexpr SizeType min = std::numeric_limits<SizeType>::min();
	static constexpr SizeType max = std::numeric_limits<SizeType>::max();
};

}

template<typename RNGSizeT, AoL::U64 BitSize>
struct PoolBit;

/**
* @details RNG Pooling
*
* - For pooling the RNG bits to be efficient with its bits
*
* @tparam I RNG size type
* @tparam BitSize bit size for pooling the RNG (i.e. 2, 4, 8, etc.)
*/
template<typename RNGSizeT, AoL::U64 BitSize>
struct AOL_EMPTY_BASE_OPTIMIZATION PoolBit : public Internal::PoolBit_Constants<BitSize>
{
	using RNGSizeType = RNGSizeT;
	using OutputType = std::conditional_t<BitSize <= 8, AoL::U8,
					   std::conditional_t<BitSize <= 16, AoL::U16,
					   std::conditional_t<BitSize <= 32, AoL::U32, AoL::U64>>>;

	static constexpr const AoL::U64 RNGBitSize = sizeof(RNGSizeType) * 8;
	static constexpr const AoL::U64 OutputBitSize = BitSize;

	static_assert(OutputBitSize >= 1 && OutputBitSize <= (RNGBitSize / 2), "BitSize must be between one and half the total bit size of the RNGSizeType!");
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
template<typename RNGSizeT>
struct AOL_EMPTY_BASE_OPTIMIZATION PoolBit<RNGSizeT, 1> : public Internal::PoolBit_Constants<1>
{
	using RNGSizeType = RNGSizeT;
	using OutputType = bool;

	static constexpr const AoL::U64 RNGBitSize = sizeof(RNGSizeType) * 8;
	static constexpr const AoL::U64 OutputBitSize = 1;
	static constexpr const RNGSizeType sentinel_mask = static_cast<RNGSizeType>(1) << (sizeof(RNGSizeType) * 8 - 1);

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

/**************************************************************************
* Helper types for PoolBit
**************************************************************************/

/****************************************
* For 64-bit RNGs
****************************************/

using PoolBit64_1 = PoolBit<AoL::U64, 1>;
using PoolBit64_2 = PoolBit<AoL::U64, 2>;
using PoolBit64_4 = PoolBit<AoL::U64, 4>;
using PoolBit64_8 = PoolBit<AoL::U64, 8>;
using PoolBit64_16 = PoolBit<AoL::U64, 16>;
using PoolBit64_32 = PoolBit<AoL::U64, 32>;

/****************************************
* For 32-bit RNGs
****************************************/

using PoolBit32_1 = PoolBit<AoL::U32, 1>;
using PoolBit32_2 = PoolBit<AoL::U32, 2>;
using PoolBit32_4 = PoolBit<AoL::U32, 4>;
using PoolBit32_8 = PoolBit<AoL::U32, 8>;
using PoolBit32_16 = PoolBit<AoL::U32, 16>;

} // Rand namespace

} // AoL namespace

// EOF