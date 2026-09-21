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

#include <exception>

namespace AoL
{

namespace Internal
{

struct BaseException : std::exception
{
private:
	constexpr static const char* default_message = "Unknown exception thrown!";

public:
	BaseException() noexcept :
#if AOL_COMPILER_MSVC
		std::exception{ default_message }
#else
		exception_message{ default_message }
#endif
	{}

	explicit BaseException(String exception_str) noexcept :
#if AOL_COMPILER_MSVC
		std::exception{ exception_str.c_str() }
#else
		exception_message{ std::move(exception_str) }
#endif
	{}

	explicit BaseException(const char* exception_str) noexcept :
#if AOL_COMPILER_MSVC
		std::exception{ exception_str }
#else
		exception_message(exception_str)
#endif
	{}

#if AOL_COMPILER_MSVC
	// MSVC's std::exception already owns a copy of the message:
	// what() is inherited, no duplicate member needed.
#else
	String exception_message;

	const char* what() const noexcept override
	{
		return exception_message.c_str();
	}
#endif
};

}

}

#endif // AOL_HEADER_INTERNAL_EXCEPTIONS_BASE_H