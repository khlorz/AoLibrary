/***************************************************************************************
* RNG rolls implementations
***************************************************************************************/

namespace AoL
{

namespace Rand
{

namespace Internal
{

template<AoL::SizeT TableSize, AoL::SizeT BitSize, bool CompileTime>
struct ThresholdTableImpl;

template<AoL::SizeT TableSize, AoL::SizeT BitSize>
struct ThresholdTableImpl<TableSize, BitSize, true>
{
	static_assert(TableSize >= 10, "TableSize must be more than or equal 10");
	static_assert(TableSize % 10 == 0, "TableSize must be divisible by 10!");
	static_assert(BitSize >= 2 && BitSize <= 32, "BitSize must be between 2 and 32!");
	static_assert(64 % BitSize == 0, "BitSize must divide 64 evenly!");

	using table_value_t = std::conditional_t<BitSize <= 8, AoL::U8,
		std::conditional_t<BitSize <= 16, AoL::U16,
		std::conditional_t<BitSize <= 32, AoL::U32, AoL::U64>>>;

	using wide_table_value_t = std::conditional_t<BitSize <= 8, AoL::U16,
		std::conditional_t<BitSize <= 16, AoL::U32, AoL::U64>>;

	using container_type = AoL::Array<table_value_t, TableSize + 1>;

	static consteval auto MakeTable() noexcept
	{
		container_type table{};

		constexpr AoL::U64 max_rng_value = (1ull << BitSize) - 1ull;

		for (AoL::SizeT i = 0; i <= TableSize; ++i)
		{
			table[i] = static_cast<table_value_t>((static_cast<wide_table_value_t>(i) * (static_cast<wide_table_value_t>(max_rng_value) + 1)) / TableSize);
		}

		return table;
	}

	static constexpr auto table = MakeTable();

	static constexpr table_value_t GetThresholdValue(AoL::SizeT val) noexcept
	{
		return table[val];
	}
};

template<AoL::SizeT TableSize, AoL::SizeT BitSize>
struct ThresholdTableImpl<TableSize, BitSize, false>
{
	static_assert(TableSize >= 10, "TableSize must be more than or equal 10");
	static_assert(TableSize % 10 == 0, "TableSize must be divisible by 10!");
	static_assert(BitSize >= 2 && BitSize <= 32, "BitSize must be between 2 and 32!");
	static_assert(64 % BitSize == 0, "BitSize must divide 64 evenly!");

	using table_value_t = std::conditional_t<BitSize <= 8, AoL::U8,
		std::conditional_t<BitSize <= 16, AoL::U16,
		std::conditional_t<BitSize <= 32, AoL::U32, AoL::U64>>>;

	using wide_table_value_t = std::conditional_t<BitSize <= 8, AoL::U16,
		std::conditional_t<BitSize <= 16, AoL::U32, AoL::U64>>;

	using container_type = AoL::Vector<table_value_t>;

	static constexpr auto MakeTable() noexcept
	{
		container_type table(TableSize + 1);

		constexpr AoL::U64 max_rng_value = (1ull << BitSize) - 1ull;

		for (AoL::SizeT i = 0; i <= TableSize; ++i)
		{
			table[i] = static_cast<table_value_t>((static_cast<wide_table_value_t>(i) * (static_cast<wide_table_value_t>(max_rng_value) + 1)) / TableSize);
		}

		return table;
	}

	inline static const auto table = MakeTable();

	static constexpr table_value_t GetThresholdValue(AoL::SizeT val) noexcept
	{
		return table[val];
	}
};

template<
	AoL::SizeT TableSize,
	AoL::SizeT BitSize
>
using ThresholdTable = ThresholdTableImpl<TableSize, BitSize, TableSize <= 100'000>;

} // Internal namespace


/*********************************************************************************************
* Chance-based functions
* - Functions for getting a true or false values depending on a given chance
* - Some convenience functions are given as well
*********************************************************************************************/

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
template<AoL::SizeT ChanceScale = 10000, typename IntType, typename RNG, typename Pool> requires std::integral<IntType>
constexpr bool RollChance(IntType chance, RNG& rng, Pool& pool) noexcept
{
	assert(chance >= 0 && chance <= ChanceScale && "Invalid chance value! The chance value should be 0 to ChanceScale!");

	return chance == ChanceScale || pool.Next(rng) < Internal::ThresholdTable<ChanceScale, Pool::OutputBitSize>::GetThresholdValue(chance);
}

/**
* @details Rolls a chance-based RNG check using a precomputed threshold table
*
* - Draws one chunk from `pool` and compares it against a threshold derived from ChanceScale
*
* - 'chance' is still expressed on a scale of ChanceScale
* 
* - 'chance' is passed as float in this overload so 0.5025f would equate to 5025 on a ChanceScale of 10000
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
template<AoL::SizeT ChanceScale = 10000, typename FloatType, typename RNG, typename Pool> requires std::floating_point<FloatType>
constexpr bool RollChance(FloatType chance, RNG& rng, Pool& pool) noexcept
{
	assert(chance >= static_cast<FloatType>(0) && chance <= static_cast<FloatType>(1) && "Invalid chance value! Chance should be greater than or equal to 0 and less than or equal to 1");

	AoL::U64 chance_int = static_cast<AoL::U64>(chance * static_cast<FloatType>(ChanceScale));
	return RollChance<ChanceScale>(chance_int, rng, pool);
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
	if (chance == 0)
	{
		return false;
	}
	else if (chance == ChanceScale)
	{
		return true;
	}
	else
	{
		constexpr auto rng_max = std::numeric_limits<Internal::RNGReturnType<RNG>>::max();
		return static_cast<AoL::U128>(rng()) * ChanceScale < static_cast<AoL::U128>(chance) * rng_max;
	}
}

/**
* @details Flip a flippin coin
* 
* - 50% chance to land on heads or tails
*
* @tparam RNG the random number generator type
* @param rng the random number generator object
* @param pool the pool object
* @return true if the roll succeeded
*/
template<typename RNG>
constexpr bool FlipCoin(RNG& rng, PoolBit<Internal::RNGReturnType<RNG>, 1>& pool) noexcept
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
	return RollChance<100>(50, rng);
}


/*********************************************************************************************
* Default RollRange functions
* - Fast roll of random numbers
* - Tested to have a chi-squared value lower than the critical chi threshold value
* - However, in the case of really biased outputs, use the Slow variant of RollRange
*********************************************************************************************/

/***************************************************
* Unsigned integer RollRange
***************************************************/

template<auto Min, auto Max, typename RNG, typename Pool>
	requires std::unsigned_integral<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRange(RNG& rng, Pool& pool) noexcept
{
	using RangeType = std::common_type_t<decltype(Min), decltype(Max)>;
	using wide_t = AoL::U64;

	constexpr wide_t min_v = static_cast<wide_t>(Min);
	constexpr wide_t max_v = static_cast<wide_t>(Max);
	constexpr wide_t range = (max_v - min_v) + 1;

	const wide_t val = static_cast<wide_t>(pool.Next(rng)) % range;

	return static_cast<RangeType>(min_v + val);
}

template<typename RangeType, typename RNG, typename Pool>
	requires std::unsigned_integral<RangeType>
constexpr RangeType RollRange(RangeType min, RangeType max, RNG& rng, Pool& pool) noexcept
{
	using wide_t = AoL::U64;

	const wide_t range = wide_t(max) - wide_t(min) + 1;
	const wide_t val = wide_t(pool.Next(rng)) % range;

	return static_cast<RangeType>(wide_t(min) + val);
}

template<typename RangeType, typename RNG>
	requires std::unsigned_integral<RangeType>
constexpr RangeType RollRange(RangeType min, RangeType max, RNG& rng) noexcept
{
	using wide_t = std::conditional_t<sizeof(RangeType) <= 4, AoL::U64, AoL::U128>;
	const wide_t range = wide_t(max) - wide_t(min) + 1;
	const wide_t val = static_cast<wide_t>(rng()) % range;
	return static_cast<RangeType>(wide_t(min) + val);
}

template<auto Min, auto Max, typename RNG>
	requires std::unsigned_integral<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRange(RNG& rng) noexcept
{
	using RangeType = std::common_type_t<decltype(Min), decltype(Max)>;
	using wide_t = std::conditional_t<sizeof(RangeType) <= 4, AoL::U64, AoL::U128>;
	constexpr wide_t min_v = static_cast<wide_t>(Min);
	constexpr wide_t max_v = static_cast<wide_t>(Max);
	constexpr wide_t range = (max_v - min_v) + 1;
	const wide_t val = static_cast<wide_t>(rng()) % range;
	return static_cast<RangeType>(min_v + val);
}

/***************************************************
* Signed integer RollRange
***************************************************/

template<auto Min, auto Max, typename RNG, typename Pool>
	requires std::signed_integral<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRange(RNG& rng, Pool& pool) noexcept
{
	using RangeType = std::common_type_t<decltype(Min), decltype(Max)>;
	using wide_t = AoL::U64;
	using signed_wide_t = AoL::I64;

	constexpr wide_t min_v = static_cast<wide_t>(static_cast<signed_wide_t>(Min));
	constexpr wide_t max_v = static_cast<wide_t>(static_cast<signed_wide_t>(Max));
	constexpr wide_t range = (max_v - min_v) + 1;

	const wide_t val = static_cast<wide_t>(pool.Next(rng)) % range;

	return static_cast<RangeType>(static_cast<signed_wide_t>(min_v + val));
}

template<typename RangeType, typename RNG, typename Pool>
	requires std::signed_integral<RangeType>
constexpr RangeType RollRange(RangeType min, RangeType max, RNG& rng, Pool& pool) noexcept
{
	using wide_t = AoL::U64;
	using signed_wide_t = AoL::I64;

	const wide_t a = static_cast<wide_t>(signed_wide_t(min));
	const wide_t b = static_cast<wide_t>(signed_wide_t(max));
	const wide_t range = (b - a) + 1;
	const wide_t val = wide_t(pool.Next(rng)) % range;

	return static_cast<RangeType>(signed_wide_t(a + val));
}

template<typename SignedType, typename RNG>
	requires std::signed_integral<SignedType>
constexpr SignedType RollRange(SignedType min, SignedType max, RNG& rng) noexcept
{
	using wide_t = std::conditional_t<sizeof(SignedType) <= 4, AoL::U64, AoL::U128>;
	using signed_wide_t = std::conditional_t<sizeof(SignedType) <= 4, AoL::I64, AoL::I128>;
	const wide_t a = static_cast<wide_t>(signed_wide_t(min));
	const wide_t b = static_cast<wide_t>(signed_wide_t(max));
	const wide_t range = (b - a) + 1;
	const wide_t val = static_cast<wide_t>(rng()) % range;
	return static_cast<SignedType>(signed_wide_t(a + val));
}

template<auto Min, auto Max, typename RNG>
	requires std::signed_integral<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRange(RNG& rng) noexcept
{
	using RangeType = std::common_type_t<decltype(Min), decltype(Max)>;
	using wide_t = std::conditional_t<sizeof(RangeType) <= 4, AoL::U64, AoL::U128>;
	using signed_wide_t = std::conditional_t<sizeof(RangeType) <= 4, AoL::I64, AoL::I128>;
	constexpr wide_t min_v = static_cast<wide_t>(static_cast<signed_wide_t>(Min));
	constexpr wide_t max_v = static_cast<wide_t>(static_cast<signed_wide_t>(Max));
	constexpr wide_t range = (max_v - min_v) + 1;
	const wide_t val = static_cast<wide_t>(rng()) % range;
	return static_cast<RangeType>(static_cast<signed_wide_t>(min_v + val));
}

/***************************************************
* Floating-point RollRange
***************************************************/

/**
* @details Rolls a uniformly distributed floating-point value in [Min, Max),
* with Min/Max known at compile time.
*
* - Same half-open interval behavior as the runtime floating-point overload.
*
* @tparam Min the inclusive lower bound of the range (compile-time constant)
* @tparam Max the exclusive upper bound of the range (compile-time constant)
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
*
* @param rng the random number generator object
* @param pool the pool object
*
* @return a uniformly distributed value in [Min, Max)
*/
template<auto Min, auto Max, typename RNG, typename Pool> requires std::floating_point<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRange(RNG& rng, Pool& pool) noexcept
{
	using float_t = std::common_type_t<decltype(Min), decltype(Max)>;
	static_assert(Min < Max, "Min must be less than Max!");

	using draw_t = typename Pool::OutputType;
	constexpr auto draw_bits = sizeof(draw_t) * 8;
	constexpr float_t max_draw_plus_one = static_cast<float_t>(static_cast<AoL::U64>(1) << draw_bits);

	const draw_t draw = pool.Next(rng);
	const float_t normalized = static_cast<float_t>(draw) / max_draw_plus_one;

	return static_cast<float_t>(Min) + normalized * (static_cast<float_t>(Max) - static_cast<float_t>(Min));
}

/**
* @details Rolls a uniformly distributed floating-point value in [min, max).
*
* - NOTE the half-open interval: the result can equal min, but will never equal
*   max exactly, since it's derived by scaling a draw into [0, 1) and lerping.
*
* @tparam FloatType the floating-point type of the range
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
*
* @param min the inclusive lower bound of the range
* @param max the exclusive upper bound of the range
* @param rng the random number generator object
* @param pool the pool object
*
* @return a uniformly distributed value in [min, max)
*/
template<typename RangeType, typename RNG, typename Pool> requires std::floating_point<RangeType>
constexpr auto RollRange(RangeType min, RangeType max, RNG& rng, Pool& pool) noexcept
{
	using draw_t = typename Pool::OutputType;

	constexpr auto draw_bits = sizeof(draw_t) * 8;
	constexpr RangeType max_draw_plus_one = static_cast<RangeType>(static_cast<AoL::U64>(1) << draw_bits);

	const draw_t draw = pool.Next(rng);
	const RangeType normalized = static_cast<RangeType>(draw) / max_draw_plus_one; // in [0, 1)

	return min + normalized * (max - min);
}

template<typename RangeType, typename RNG> requires std::floating_point<RangeType>
constexpr auto RollRange(RangeType min, RangeType max, RNG& rng) noexcept
{
	using rng_t = Internal::RNGReturnType<RNG>;
	constexpr auto rng_bits = sizeof(rng_t) * 8;
	constexpr RangeType max_draw_plus_one = static_cast<RangeType>(rng_bits < 64
		? static_cast<AoL::U64>(1) << rng_bits
		: static_cast<AoL::U128>(1) << rng_bits);
	const rng_t draw = rng();
	const RangeType normalized = static_cast<RangeType>(draw) / max_draw_plus_one;
	return min + normalized * (max - min);
}

template<auto Min, auto Max, typename RNG> requires std::floating_point<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRange(RNG& rng) noexcept
{
	using float_t = std::common_type_t<decltype(Min), decltype(Max)>;
	using rng_t = Internal::RNGReturnType<RNG>;
	constexpr auto rng_bits = sizeof(rng_t) * 8;
	constexpr float_t max_draw_plus_one = static_cast<float_t>(rng_bits < 64
		? static_cast<AoL::U64>(1) << rng_bits
		: static_cast<AoL::U128>(1) << rng_bits);
	const rng_t draw = rng();
	const float_t normalized = static_cast<float_t>(draw) / max_draw_plus_one;
	return static_cast<float_t>(Min) + normalized * (static_cast<float_t>(Max) - static_cast<float_t>(Min));
}


/*********************************************************************************************
* Slow-variant RollRange functions
* - Slower than the default RollRange functions
* - It is guaranteed to have chi-squared value lower than the critical chi threshold value
*********************************************************************************************/

/***************************************************
* Unsigned integer RollRange
***************************************************/

/**
* @details Rolls a uniformly distributed unsigned integer in [Min, Max], inclusive,
* with Min/Max known at compile time.
*
* - value_t is automatically sized to the smallest unsigned type that can hold Max
*   (e.g. Max=150 selects U8, Max=20124 selects U16).
*
* - range_size/threshold are constexpr, computed once per unique (Min, Max)
*   instantiation rather than on every call.
*
* @tparam Min the inclusive lower bound of the range (compile-time constant)
* @tparam Max the inclusive upper bound of the range (compile-time constant)
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
*
* @param rng the random number generator object
* @param pool the pool object
*
* @return a uniformly distributed value in [Min, Max]
*/
template<auto Min, auto Max, typename RNG, typename Pool>
	requires std::unsigned_integral<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRangeSlow(RNG& rng, Pool& pool) noexcept
{
	using value_t = std::conditional_t<(Max <= std::numeric_limits<AoL::U8>::max()), AoL::U8,
					std::conditional_t<(Max <= std::numeric_limits<AoL::U16>::max()), AoL::U16,
					std::conditional_t<(Max <= std::numeric_limits<AoL::U32>::max()), AoL::U32, AoL::U64>>>;

	static_assert(Min < Max, "Min must be less than Max!");
	static_assert(sizeof(typename Pool::OutputType) >= sizeof(value_t), "Pool's OutputType must be at least as wide as the range's value type!");

	using wide_t = std::conditional_t<sizeof(value_t) <= 1, AoL::U16, std::conditional_t<sizeof(value_t) <= 2, AoL::U32, AoL::U64>>;

	constexpr wide_t range_size = static_cast<wide_t>(static_cast<value_t>(Max)) - static_cast<wide_t>(static_cast<value_t>(Min)) + 1;

	value_t draw = static_cast<value_t>(pool.Next(rng));
	wide_t wide_product = static_cast<wide_t>(draw) * range_size;
	value_t low_part = static_cast<value_t>(wide_product);

	if (static_cast<wide_t>(low_part) < range_size) [[unlikely]]
	{
		constexpr wide_t two_to_the_n = (sizeof(value_t) * 8 >= sizeof(wide_t) * 8)	? wide_t{ 0 } : (static_cast<wide_t>(1) << (sizeof(value_t) * 8));
		constexpr wide_t threshold = two_to_the_n % range_size;

		while (static_cast<wide_t>(low_part) < threshold) [[unlikely]]
		{
			draw = static_cast<value_t>(pool.Next(rng));
			wide_product = static_cast<wide_t>(draw) * range_size;
			low_part = static_cast<value_t>(wide_product);
		}
	}

	const value_t result = static_cast<value_t>(wide_product >> (sizeof(value_t) * 8));
	return static_cast<value_t>(static_cast<value_t>(Min) + result);
}

/**
* @details Rolls a uniformly distributed unsigned integer in [min, max], inclusive.
*
* - Uses Lemire's method: a single multiply + shift on the common path, with
*   division/modulo confined to a rare rejection branch, avoiding the bias a naive
*   modulo-based mapping would introduce.
*
* @tparam RangeType the unsigned integral type of the range
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
*
* @param min the inclusive lower bound of the range
* @param max the inclusive upper bound of the range
* @param rng the random number generator object
* @param pool the pool object
*
* @return a uniformly distributed value in [min, max]
*/
template<typename RangeType, typename RNG, typename Pool>
	requires std::unsigned_integral<RangeType>
constexpr RangeType RollRangeSlow(RangeType min, RangeType max, RNG& rng, Pool& pool) noexcept
{
	static_assert(sizeof(typename Pool::OutputType) >= sizeof(RangeType),
		"Pool's OutputType must be at least as wide as RangeType!");

	using value_t = RangeType;
	using wide_t = std::conditional_t<sizeof(value_t) <= 1, AoL::U16, std::conditional_t<sizeof(value_t) <= 2, AoL::U32, AoL::U64>>;

	// range_size is computed in wide_t (not value_t) so it can correctly hold
	// 2^N when the full range of value_t is requested (e.g. min=0, max=value_t's
	// max), which is one more than value_t itself can represent.
	const wide_t range_size = static_cast<wide_t>(max) - static_cast<wide_t>(min) + 1;

	value_t draw = static_cast<value_t>(pool.Next(rng));
	wide_t wide_product = static_cast<wide_t>(draw) * range_size;
	value_t low_part = static_cast<value_t>(wide_product);

	if (static_cast<wide_t>(low_part) < range_size) [[unlikely]]
	{
		// threshold = (2^N) mod range_size, where N = bit-width of value_t.
		// Computed directly in wide_t to avoid integer-promotion pitfalls
		// (sub-int unsigned types promote to `int` during arithmetic).
		const wide_t two_to_the_n = (sizeof(value_t) * 8 >= sizeof(wide_t) * 8)	? wide_t{ 0 } : (static_cast<wide_t>(1) << (sizeof(value_t) * 8));
		const wide_t threshold = two_to_the_n % range_size;

		while (static_cast<wide_t>(low_part) < threshold) [[unlikely]]
		{
			draw = static_cast<value_t>(pool.Next(rng));
			wide_product = static_cast<wide_t>(draw) * range_size;
			low_part = static_cast<value_t>(wide_product);
		}
	}

	const value_t result = static_cast<value_t>(wide_product >> (sizeof(value_t) * 8));
	return static_cast<value_t>(min + result);
}

template<auto Min, auto Max, typename RNG>
	requires std::unsigned_integral<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRangeSlow(RNG& rng) noexcept
{
	using value_t = std::conditional_t<(Max <= std::numeric_limits<AoL::U8>::max()), AoL::U8,
		std::conditional_t<(Max <= std::numeric_limits<AoL::U16>::max()), AoL::U16,
		std::conditional_t<(Max <= std::numeric_limits<AoL::U32>::max()), AoL::U32, AoL::U64>>>;
	static_assert(Min < Max, "Min must be less than Max!");
	using wide_t = std::conditional_t<sizeof(value_t) <= 1, AoL::U16, std::conditional_t<sizeof(value_t) <= 2, AoL::U32, std::conditional_t<sizeof(value_t) <= 4, AoL::U64, AoL::U128>>>;
	constexpr wide_t range_size = static_cast<wide_t>(static_cast<value_t>(Max)) - static_cast<wide_t>(static_cast<value_t>(Min)) + 1;

	value_t draw = static_cast<value_t>(rng());
	wide_t wide_product = static_cast<wide_t>(draw) * range_size;
	value_t low_part = static_cast<value_t>(wide_product);

	if (static_cast<wide_t>(low_part) < range_size) [[unlikely]]
	{
		constexpr wide_t two_to_the_n = (sizeof(value_t) * 8 >= sizeof(wide_t) * 8) ? wide_t{ 0 } : (static_cast<wide_t>(1) << (sizeof(value_t) * 8));
		constexpr wide_t threshold = two_to_the_n % range_size;
		while (static_cast<wide_t>(low_part) < threshold) [[unlikely]]
		{
			draw = static_cast<value_t>(rng());
			wide_product = static_cast<wide_t>(draw) * range_size;
			low_part = static_cast<value_t>(wide_product);
		}
	}

	const value_t result = static_cast<value_t>(wide_product >> (sizeof(value_t) * 8));
	return static_cast<value_t>(static_cast<value_t>(Min) + result);
}

template<typename RangeType, typename RNG>
	requires std::unsigned_integral<RangeType>
constexpr RangeType RollRangeSlow(RangeType min, RangeType max, RNG& rng) noexcept
{
	using value_t = RangeType;
	using wide_t = std::conditional_t<sizeof(value_t) <= 1, AoL::U16, std::conditional_t<sizeof(value_t) <= 2, AoL::U32, std::conditional_t<sizeof(value_t) <= 4, AoL::U64, AoL::U128>>>;
	const wide_t range_size = static_cast<wide_t>(max) - static_cast<wide_t>(min) + 1;

	value_t draw = static_cast<value_t>(rng());
	wide_t wide_product = static_cast<wide_t>(draw) * range_size;
	value_t low_part = static_cast<value_t>(wide_product);

	if (static_cast<wide_t>(low_part) < range_size) [[unlikely]]
	{
		const wide_t two_to_the_n = (sizeof(value_t) * 8 >= sizeof(wide_t) * 8) ? wide_t{ 0 } : (static_cast<wide_t>(1) << (sizeof(value_t) * 8));
		const wide_t threshold = two_to_the_n % range_size;
		while (static_cast<wide_t>(low_part) < threshold) [[unlikely]]
		{
			draw = static_cast<value_t>(rng());
			wide_product = static_cast<wide_t>(draw) * range_size;
			low_part = static_cast<value_t>(wide_product);
		}
	}

	const value_t result = static_cast<value_t>(wide_product >> (sizeof(value_t) * 8));
	return static_cast<value_t>(min + result);
}

/***************************************************
* Signed integer RollRange
***************************************************/

/**
* @details Rolls a uniformly distributed signed integer in [Min, Max], inclusive,
* with Min/Max known at compile time.
*
* - Shares the same unsigned-domain arithmetic and int-promotion fix as the
*   runtime signed overload, with range_size/threshold computed as constexpr.
*
* @tparam Min the inclusive lower bound of the range (compile-time constant)
* @tparam Max the inclusive upper bound of the range (compile-time constant)
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
*
* @param rng the random number generator object
* @param pool the pool object
*
* @return a uniformly distributed value in [Min, Max]
*/
template<auto Min, auto Max, typename RNG, typename Pool>
	requires std::signed_integral<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRangeSlow(RNG& rng, Pool& pool) noexcept
{
	static_assert(Min < Max, "Min must be less than Max!");

	using signed_t = std::common_type_t<decltype(Min), decltype(Max)>;
	using value_t = std::conditional_t<sizeof(signed_t) <= 1, AoL::U8,
					std::conditional_t<sizeof(signed_t) <= 2, AoL::U16,
					std::conditional_t<sizeof(signed_t) <= 4, AoL::U32, AoL::U64>>>;

	static_assert(sizeof(typename Pool::OutputType) >= sizeof(value_t),
		"Pool's OutputType must be at least as wide as the range's value type!");

	using wide_t = std::conditional_t<sizeof(value_t) <= 1, AoL::U16,
		std::conditional_t<sizeof(value_t) <= 2, AoL::U32, AoL::U64>>;

	// Same int-promotion fix as the runtime signed overload (see comment there).
	constexpr value_t diff_wrapped = static_cast<value_t>(static_cast<value_t>(Max) - static_cast<value_t>(Min));
	constexpr wide_t range_size = static_cast<wide_t>(diff_wrapped) + 1;

	value_t draw = static_cast<value_t>(pool.Next(rng));
	wide_t wide_product = static_cast<wide_t>(draw) * range_size;
	value_t low_part = static_cast<value_t>(wide_product);

	if (static_cast<wide_t>(low_part) < range_size) [[unlikely]]
	{
		constexpr wide_t two_to_the_n = (sizeof(value_t) * 8 >= sizeof(wide_t) * 8)	? wide_t{ 0 } : (static_cast<wide_t>(1) << (sizeof(value_t) * 8));
		constexpr wide_t threshold = two_to_the_n % range_size;

		while (static_cast<wide_t>(low_part) < threshold) [[unlikely]]
		{
			draw = static_cast<value_t>(pool.Next(rng));
			wide_product = static_cast<wide_t>(draw) * range_size;
			low_part = static_cast<value_t>(wide_product);
		}
	}

	const value_t result = static_cast<value_t>(wide_product >> (sizeof(value_t) * 8));
	return static_cast<signed_t>(static_cast<signed_t>(Min) + static_cast<signed_t>(result));
}

/**
* @details Rolls a uniformly distributed signed integer in [min, max], inclusive.
*
* - Internally performs all Lemire's-method arithmetic in an unsigned domain
*   (two's-complement reinterpretation), converting back to signed only for the
*   final result. This avoids relying on signed overflow/wraparound, which is
*   unreliable to depend on.
*
* @tparam SignedType the signed integral type of the range
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
*
* @param min the inclusive lower bound of the range
* @param max the inclusive upper bound of the range
* @param rng the random number generator object
* @param pool the pool object
*
* @return a uniformly distributed value in [min, max]
*/
template<typename SignedType, typename RNG, typename Pool>
	requires std::signed_integral<SignedType>
constexpr SignedType RollRangeSlow(SignedType min, SignedType max, RNG& rng, Pool& pool) noexcept
{
	using signed_t = SignedType;
	using value_t = std::conditional_t<sizeof(signed_t) <= 1, AoL::U8,
					std::conditional_t<sizeof(signed_t) <= 2, AoL::U16,
					std::conditional_t<sizeof(signed_t) <= 4, AoL::U32, AoL::U64>>>;

	static_assert(sizeof(typename Pool::OutputType) >= sizeof(value_t),
		"Pool's OutputType must be at least as wide as the range's value type!");

	using wide_t = std::conditional_t<sizeof(value_t) <= 1, AoL::U16,
		std::conditional_t<sizeof(value_t) <= 2, AoL::U32, AoL::U64>>;

	// IMPORTANT: the inner static_cast<value_t> on the subtraction's result is
	// load-bearing. Without it, C++ integer promotion silently widens the
	// subtraction to `int` for any value_t narrower than int (U8/U16),
	// breaking the wraparound-at-value_t's-width trick and producing a
	// corrupted (oversized) range_size -- which can make `threshold`
	// unreachable and hang the rejection loop forever. Verified against the
	// failing case min=-32768, max=12379 with a 16-bit value_t.
	const value_t diff_wrapped = static_cast<value_t>(static_cast<value_t>(max) - static_cast<value_t>(min));
	const wide_t range_size = static_cast<wide_t>(diff_wrapped) + 1;

	value_t draw = static_cast<value_t>(pool.Next(rng));
	wide_t wide_product = static_cast<wide_t>(draw) * range_size;
	value_t low_part = static_cast<value_t>(wide_product);

	if (static_cast<wide_t>(low_part) < range_size) [[unlikely]]
	{
		const wide_t two_to_the_n = (sizeof(value_t) * 8 >= sizeof(wide_t) * 8)	? wide_t{ 0 } : (static_cast<wide_t>(1) << (sizeof(value_t) * 8));
		const wide_t threshold = two_to_the_n % range_size;

		while (static_cast<wide_t>(low_part) < threshold) [[unlikely]]
		{
			draw = static_cast<value_t>(pool.Next(rng));
			wide_product = static_cast<wide_t>(draw) * range_size;
			low_part = static_cast<value_t>(wide_product);
		}
	}

	const value_t result = static_cast<value_t>(wide_product >> (sizeof(value_t) * 8));
	return static_cast<signed_t>(static_cast<signed_t>(min) + static_cast<signed_t>(result));
}

template<auto Min, auto Max, typename RNG>
	requires std::signed_integral<std::common_type_t<decltype(Min), decltype(Max)>>
constexpr auto RollRangeSlow(RNG& rng) noexcept
{
	static_assert(Min < Max, "Min must be less than Max!");
	using signed_t = std::common_type_t<decltype(Min), decltype(Max)>;
	using value_t = std::conditional_t<sizeof(signed_t) <= 1, AoL::U8,
		std::conditional_t<sizeof(signed_t) <= 2, AoL::U16,
		std::conditional_t<sizeof(signed_t) <= 4, AoL::U32, AoL::U64>>>;
	using wide_t = std::conditional_t<sizeof(value_t) <= 1, AoL::U16,
		std::conditional_t<sizeof(value_t) <= 2, AoL::U32,
		std::conditional_t<sizeof(value_t) <= 4, AoL::U64, AoL::U128>>>;
	constexpr value_t diff_wrapped = static_cast<value_t>(static_cast<value_t>(Max) - static_cast<value_t>(Min));
	constexpr wide_t range_size = static_cast<wide_t>(diff_wrapped) + 1;

	value_t draw = static_cast<value_t>(rng());
	wide_t wide_product = static_cast<wide_t>(draw) * range_size;
	value_t low_part = static_cast<value_t>(wide_product);

	if (static_cast<wide_t>(low_part) < range_size) [[unlikely]]
	{
		constexpr wide_t two_to_the_n = (sizeof(value_t) * 8 >= sizeof(wide_t) * 8) ? wide_t{ 0 } : (static_cast<wide_t>(1) << (sizeof(value_t) * 8));
		constexpr wide_t threshold = two_to_the_n % range_size;
		while (static_cast<wide_t>(low_part) < threshold) [[unlikely]]
		{
			draw = static_cast<value_t>(rng());
			wide_product = static_cast<wide_t>(draw) * range_size;
			low_part = static_cast<value_t>(wide_product);
		}
	}

	const value_t result = static_cast<value_t>(wide_product >> (sizeof(value_t) * 8));
	return static_cast<signed_t>(static_cast<signed_t>(Min) + static_cast<signed_t>(result));
}

template<typename SignedType, typename RNG>
	requires std::signed_integral<SignedType>
constexpr SignedType RollRangeSlow(SignedType min, SignedType max, RNG& rng) noexcept
{
	using signed_t = SignedType;
	using value_t = std::conditional_t<sizeof(signed_t) <= 1, AoL::U8,
		std::conditional_t<sizeof(signed_t) <= 2, AoL::U16,
		std::conditional_t<sizeof(signed_t) <= 4, AoL::U32, AoL::U64>>>;
	using wide_t = std::conditional_t<sizeof(value_t) <= 1, AoL::U16,
		std::conditional_t<sizeof(value_t) <= 2, AoL::U32,
		std::conditional_t<sizeof(value_t) <= 4, AoL::U64, AoL::U128>>>;
	const value_t diff_wrapped = static_cast<value_t>(static_cast<value_t>(max) - static_cast<value_t>(min));
	const wide_t range_size = static_cast<wide_t>(diff_wrapped) + 1;

	value_t draw = static_cast<value_t>(rng());
	wide_t wide_product = static_cast<wide_t>(draw) * range_size;
	value_t low_part = static_cast<value_t>(wide_product);

	if (static_cast<wide_t>(low_part) < range_size) [[unlikely]]
	{
		const wide_t two_to_the_n = (sizeof(value_t) * 8 >= sizeof(wide_t) * 8) ? wide_t{ 0 } : (static_cast<wide_t>(1) << (sizeof(value_t) * 8));
		const wide_t threshold = two_to_the_n % range_size;
		while (static_cast<wide_t>(low_part) < threshold) [[unlikely]]
		{
			draw = static_cast<value_t>(rng());
			wide_product = static_cast<wide_t>(draw) * range_size;
			low_part = static_cast<value_t>(wide_product);
		}
	}

	const value_t result = static_cast<value_t>(wide_product >> (sizeof(value_t) * 8));
	return static_cast<signed_t>(static_cast<signed_t>(min) + static_cast<signed_t>(result));
}


/*********************************************************************************************
* Utility roll functions
*********************************************************************************************/

/***************************************************
* Dice rollings
***************************************************/

template<typename RNG, typename Pool>
constexpr AoL::SizeT RollDice(AoL::SizeT max_sides, RNG& rng, Pool& pool) noexcept
{
	assert(max_sides > 1 && "max_sides should be more than 1");

	return RollRange(static_cast<AoL::SizeT>(1), max_sides, rng, pool);
}

template<AoL::SizeT MaxSides, typename RNG, typename Pool>
constexpr AoL::SizeT RollDice(RNG& rng, Pool& pool) noexcept
{
	static_assert(MaxSides > 1, "MaxSides should be more than 1!");

	return RollRange(static_cast<SizeT>(1), MaxSides, rng, pool);
}

template<typename RNG>
constexpr AoL::SizeT RollDice(AoL::SizeT max_sides, RNG& rng) noexcept
{
	assert(max_sides > 1 && "max_sides should be more than 1");
	return RollRange(static_cast<AoL::SizeT>(1), max_sides, rng);
}

template<AoL::SizeT MaxSides, typename RNG>
constexpr AoL::SizeT RollDice(RNG& rng) noexcept
{
	static_assert(MaxSides > 1, "MaxSides should be more than 1!");
	return RollRange(static_cast<SizeT>(1), MaxSides, rng);
}

/***************************************************
* Index rollings
***************************************************/

template<typename RNG, typename Pool>
constexpr AoL::SizeT RollIndex(AoL::SizeT max_size, RNG& rng, Pool& pool) noexcept
{
	assert(max_size > 1 && "size should be more than 0");

	return RollRange(static_cast<AoL::SizeT>(0), max_size, rng, pool);
}

template<AoL::SizeT MaxSize, typename RNG, typename Pool>
constexpr AoL::SizeT RollIndex(RNG& rng, Pool& pool) noexcept
{
	static_assert(MaxSize > 1, "MaxSize should be more than 0!");

	return RollRange(static_cast<AoL::SizeT>(0), MaxSize, rng, pool);
}

template<typename RNG>
constexpr AoL::SizeT RollIndex(AoL::SizeT max_size, RNG& rng) noexcept
{
	assert(max_size > 1 && "size should be more than 0");
	return RollRange(static_cast<AoL::SizeT>(0), max_size, rng);
}

template<AoL::SizeT MaxSize, typename RNG>
constexpr AoL::SizeT RollIndex(RNG& rng) noexcept
{
	static_assert(MaxSize > 1, "MaxSize should be more than 0!");
	return RollRange(static_cast<AoL::SizeT>(0), MaxSize, rng);
}

/***************************************************
* Random element pick
***************************************************/

/**
* @details Picks a random element from a range.
* 
* - Returns an iterator to the randomly selected element.
* 
* - The range must be non-empty.
* 
* @tparam It the random access iterator type
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
* @param begin iterator to the beginning of the range
* @param end iterator to the end of the range
* @param rng the random number generator object
* @param pool the pool object
* @return iterator to the randomly selected element
*/
template<std::input_or_output_iterator It, typename RNG, typename Pool>
constexpr It RollIterator(It begin, It end, RNG& rng, Pool& pool) noexcept
{
	static_assert(std::equality_comparable<It>, "Iterator must be compared by equality!");

	if constexpr (std::random_access_iterator<It>)
	{
		auto dist = static_cast<AoL::SizeT>(end - begin);
		assert(dist > 0 && "Range must be non-empty!");
		auto offset = RollRange(static_cast<AoL::SizeT>(0), dist - 1, rng, pool);
		return begin + offset;
	}
	else
	{
		assert(begin != end && "Range must be non-empty!");
		auto result = begin;
		AoL::SizeT n = 1;
		for (auto it = std::next(begin); it != end; ++it)
		{
			++n;
			auto roll = RollRange(static_cast<AoL::SizeT>(0), n - 1, rng, pool);
			if (roll == 0) { result = it; }
		}
		return result;
	}
}

template<std::input_or_output_iterator It, typename RNG>
constexpr It RollIterator(It begin, It end, RNG& rng) noexcept
{
	static_assert(std::equality_comparable<It>, "Iterator must be compared by equality!");

	if constexpr (std::random_access_iterator<It>)
	{
		auto dist = static_cast<AoL::SizeT>(end - begin);
		assert(dist > 0 && "Range must be non-empty!");
		auto offset = RollRange(static_cast<AoL::SizeT>(0), dist - 1, rng);
		return begin + offset;
	}
	else
	{
		assert(begin != end && "Range must be non-empty!");
		auto result = begin;
		AoL::SizeT n = 1;
		for (auto it = std::next(begin); it != end; ++it)
		{
			++n;
			auto roll = RollRange(static_cast<AoL::SizeT>(0), n - 1, rng);
			if (roll == 0) { result = it; }
		}
		return result;
	}
}

template<std::input_or_output_iterator It, typename RNG, typename Pool>
constexpr decltype(auto) RollElement(It begin, It end, RNG& rng, Pool& pool) noexcept
{
	return *RollIterator(begin, end, rng, pool);
}

template<std::input_or_output_iterator It, typename RNG>
constexpr decltype(auto) RollElement(It begin, It end, RNG& rng) noexcept
{
	return *RollIterator(begin, end, rng);
}

/***************************************************
* Weighted random index selection
***************************************************/

/**
* @details Selects an index based on a set of unsigned integral weights.
* 
* - Each weight represents the relative probability of that index being chosen.
* 
* - Single-pass linear scan with O(n) time and O(1) extra space (no heap alloc).
* 
* - All weights must be non-negative and their sum must be > 0.
* 
* @tparam It the forward iterator type
* @tparam RNG the random number generator type
* @tparam Pool the pool type used for pooling the rng bits
* @param begin iterator to the beginning of the weights range
* @param end iterator to the end of the weights range
* @param rng the random number generator object
* @param pool the pool object
* @return the selected index in [0, distance(begin, end))
*/
template<std::forward_iterator It, typename RNG, typename Pool>
	requires std::unsigned_integral<typename std::iterator_traits<It>::value_type>
constexpr AoL::SizeT RollWeighted(It begin, It end, RNG& rng, Pool& pool) noexcept
{
	assert(begin != end && "Weight range must be non-empty!");

	using WeightType = typename std::iterator_traits<It>::value_type;

	WeightType total = 0;

	for (auto it = begin; it != end; ++it)
	{
		total += *it;
	}

	assert(total > 0 && "Sum of weights must be greater than zero!");

	auto roll = RollRange(static_cast<WeightType>(0), static_cast<WeightType>(total - 1), rng, pool);

	WeightType sum = 0;
	AoL::SizeT idx = 0;

	for (auto it = begin; it != end; ++it, ++idx)
	{
		sum += *it;
		if (roll < sum)
		{
			return idx;
		}
	}

	return idx - 1;
}

template<std::forward_iterator It, typename RNG>
	requires std::unsigned_integral<typename std::iterator_traits<It>::value_type>
constexpr AoL::SizeT RollWeighted(It begin, It end, RNG& rng) noexcept
{
	assert(begin != end && "Weight range must be non-empty!");
	using WeightType = typename std::iterator_traits<It>::value_type;

	WeightType total = 0;
	for (auto it = begin; it != end; ++it) { total += *it; }
	assert(total > 0 && "Sum of weights must be greater than zero!");

	auto roll = RollRange(static_cast<WeightType>(0), static_cast<WeightType>(total - 1), rng);

	WeightType sum = 0;
	AoL::SizeT idx = 0;
	for (auto it = begin; it != end; ++it, ++idx)
	{
		sum += *it;
		if (roll < sum)
		{
			return idx;
		}
	}
	return idx - 1;
}

} // Rand namespace

} // AoL namespace