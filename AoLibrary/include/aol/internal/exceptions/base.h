/***************************************************************************************
* AoLibrary Exceptions - base
****************************************************************************************
* - Base exception class used by the library
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_EXCEPTIONS_BASE_H
#define AOL_HEADER_INTERNAL_EXCEPTIONS_BASE_H


#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/traits.h"
#include "aol/types.h"
#include "aol/strings.h"


namespace AoL
{

namespace Internal
{

struct BaseException
{
private:
	constexpr static const char* default_message = "Unknown exception thrown!";

public:
	String exception_message;

	BaseException() noexcept :
		exception_message{ default_message }
	{}

	BaseException(String&& exception_str) noexcept :
		exception_message{ std::move(exception_str) }
	{}

	BaseException(const char* exception_str) noexcept :
		exception_message(exception_str)
	{}

	virtual ~BaseException() noexcept = default;

	virtual const char* What() const noexcept = 0;
};

}

}

#endif // AOL_HEADER_INTERNAL_EXCEPTIONS_BASE_H