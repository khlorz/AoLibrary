#pragma once

/***************************************************************************************
* AoLibrary exceptions
****************************************************************************************
* - The library's custom exception
* - With the standard exception library using std::string which is different from the
*	library's string, I had to make a custom exception
* - Do make sure to use the standard library exception for stl-related things
***************************************************************************************/

#include "config.h"

#include "strings.h"

#include <exception>

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