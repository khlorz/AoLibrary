/***************************************************************************************
* AoLibrary Exceptions - runtime
****************************************************************************************
* - Various runtime exception classes
***************************************************************************************/
#ifndef AOL_HEADER_INTERNAL_EXCEPTIONS_RUNTIME_H
#define AOL_HEADER_INTERNAL_EXCEPTIONS_RUNTIME_H


#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/traits.h"
#include "aol/types.h"
#include "aol/internal/exceptions/base.h"

#include <exception>


namespace AoL
{

struct RuntimeException : Internal::BaseException
{
private:
	constexpr static const char* default_message = "Runtime exception occurred!";

public:
	RuntimeException() noexcept :
		BaseException{ default_message }
	{}

	explicit RuntimeException(const String& exception_message) noexcept :
		BaseException{ exception_message }
	{}

	explicit RuntimeException(const char* exception_message) noexcept :
		BaseException{ exception_message }
	{}
};

struct FileException : RuntimeException
{
private:
	constexpr static const char* default_message = "There was an error processing the file!";

	static String ExceptionMessage(StringView filename) noexcept
	{
		String message{ default_message };
		message.append(filename);
		return message;
	}

public:
	FileException(const String& filename) noexcept :
		RuntimeException{ ExceptionMessage(filename) }
	{
	}

	FileException(const char* filename) noexcept :
		RuntimeException{ ExceptionMessage(filename) }
	{}
};

}


#endif // AOL_HEADER_INTERNAL_EXCEPTIONS_RUNTIME_H