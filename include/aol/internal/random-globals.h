/***************************************************************************************
* Random global values
***************************************************************************************/

namespace AoL
{

namespace Rand
{

namespace Internal
{

inline auto& DefaultRNG() noexcept
{
	static thread_local DefaultGen default_rng(
		static_cast<AoL::U64>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) 
		^
		AoL::Internal::DefaultHash<std::thread::id>{}(std::this_thread::get_id())
	);
	return default_rng;
}

template<AoL::SizeT BitSize, typename RNG>
constexpr auto& DefaultPool(RNG& rng = DefaultRNG()) noexcept
{
	using Pool = PoolBit<std::remove_cvref_t<decltype(std::declval<RNG&>()())>, BitSize>;
	static thread_local Pool default_local_pool{ DefaultRNG() };
	return default_local_pool;
}

}

}

}