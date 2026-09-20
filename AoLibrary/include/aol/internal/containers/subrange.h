/*************************************************
* AoLibrary Subrange implementations
*************************************************/
#ifndef AOL_HEADER_INTERNAL_CONTAINERS_SUBRANGE_H
#define AOL_HEADER_INTERNAL_CONTAINERS_SUBRANGE_H


#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/traits.h"
#include "aol/types.h"

#include <iterator>


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
	
	AOL_ATTRIB_NO_DISCARD constexpr iterator begin() noexcept
	{
		return start;
	}
	
	AOL_ATTRIB_NO_DISCARD constexpr iterator begin() const noexcept
	{
		return start;
	}

	AOL_ATTRIB_NO_DISCARD constexpr iterator end() noexcept
	{
		return finish;
	}

	AOL_ATTRIB_NO_DISCARD constexpr iterator end() const noexcept
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
	using value_type = typename SubrangeExBase<It>::value_type;
	using size_type = SizeT;

	AOL_ATTRIB_NO_DISCARD size_type constexpr size() const noexcept
	{
		return static_cast<size_type>(this->finish - this->start);
	}

	AOL_ATTRIB_NO_DISCARD auto constexpr empty() const noexcept
	{
		return this->start == this->finish;
	}

	AOL_ATTRIB_NO_DISCARD constexpr value_type& operator [] (size_type idx) noexcept
	{
		assert(idx < this->size() && "Invalid index!");
		return *(this->start + idx);
	}

	AOL_ATTRIB_NO_DISCARD constexpr const value_type& operator [] (size_type idx) const noexcept
	{
		assert(idx < this->size() && "Invalid index!");
		return *(this->start + idx);
	}
};

template<typename It>
SubrangeEx(It, It) -> SubrangeEx<It>;

} // AoL::Internal namespace


#endif // AOL_HEADER_INTERNAL_CONTAINERS_SUBRANGE_H