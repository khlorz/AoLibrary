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

/**
* 'Using' wrapper for std::exception
*
* - Use this for cases of exceptions where std::exceptions or its derived types are thrown
*
* - Since the library have its exception types, this is basically the handler for exceptions not in its control
*/
using STLException = std::exception;

struct RuntimeException : Internal::BaseException
{
private:
	constexpr static const char* default_message = "Runtime exception occured!";

public:
	using Internal::BaseException::BaseException;

	RuntimeException() noexcept :
		BaseException{ default_message }
	{}

	const char* What() const noexcept override
	{
		return exception_message.c_str();
	}
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

public:
	FileOpenException(const String& filename) noexcept :
		RuntimeException{ error_message }
	{
		exception_message.append(filename);
	}

	FileOpenException(const char* filename) noexcept :
		RuntimeException{ error_message }
	{
		exception_message.append(filename);
	}
};

}


#endif // AOL_HEADER_INTERNAL_EXCEPTIONS_RUNTIME_H