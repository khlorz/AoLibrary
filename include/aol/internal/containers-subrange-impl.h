#pragma once

/*************************************************
* [Container] Subrange implementations
*************************************************/

namespace AoL::Internal
{

template<typename It>
struct SubrangeExBase
{
	using iterator = It;
	using value_type = std::iterator_traits<iterator>::value_type;

	iterator start;
	iterator finish;
	
	SubrangeExBase(iterator start_, iterator finish_) noexcept :
		start{ start_ },
		finish{ finish_ }
	{
	}
	
	AOL_NO_DISCARD constexpr iterator begin() noexcept
	{
		return start;
	}
	
	AOL_NO_DISCARD constexpr iterator begin() const noexcept
	{
		return start;
	}

	AOL_NO_DISCARD constexpr iterator end() noexcept
	{
		return finish;
	}

	AOL_NO_DISCARD constexpr iterator end() const noexcept
	{
		return finish;
	}
};

template<typename It>
struct SubrangeEx : SubrangeExBase<It>
{
	using SubrangeExBase<It>::SubrangeExBase;
};

template<std::random_access_iterator It>
struct SubrangeEx<It> : SubrangeExBase<It>
{
	using SubrangeExBase<It>::SubrangeExBase;

	AOL_NO_DISCARD auto constexpr size() const noexcept
	{
		return this->finish - this->start;
	}

	AOL_NO_DISCARD auto constexpr empty() const noexcept
	{
		return this->start == this->finish;
	}
};

template<typename It>
SubrangeEx(It, It) -> SubrangeEx<It>;

} // AoL::Internal namespace