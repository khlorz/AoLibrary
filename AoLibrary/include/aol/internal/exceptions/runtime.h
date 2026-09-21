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

	static String ExceptionMessage(StringView additional_message) noexcept
	{
		String message{ default_message };
		message.append("\n");
		message.append(additional_message);
		return message;
	}

public:
	RuntimeException() noexcept :
		BaseException{ default_message }
	{}

	explicit RuntimeException(const String& additional_message) noexcept :
		BaseException{ this->ExceptionMessage(additional_message) }
	{}

	explicit RuntimeException(const char* additional_message) noexcept :
		BaseException{ this->ExceptionMessage(additional_message) }
	{}
};

struct FileOpenException : RuntimeException
{
private:
	constexpr static const char* error_message =
		"Failed to open file. Possible reasons include:\n"
		"  - File does not exist or path is incorrect\n"
		"  - Insufficient permissions to read the file\n"
		"  - File is currently locked by another process\n"
		"  - File system errors or corruption\n"
		"Please check the file path, permissions, and ensure it is accessible.\n"
		"File in question: ";

	static String ExceptionMessage(const char* filename) noexcept
	{
		String message{ error_message };
		message.append(filename);
		return message;
	}

public:
	FileOpenException(const String& filename) noexcept :
		RuntimeException{ this->ExceptionMessage(filename.c_str()) }
	{
	}

	FileOpenException(const char* filename) noexcept :
		RuntimeException{ this->ExceptionMessage(filename) }
	{
	}
};

}


#endif // AOL_HEADER_INTERNAL_EXCEPTIONS_RUNTIME_H