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

constexpr ExceptionCode ExceptionCode_Runtime{ 1ULL };
constexpr ExceptionCode ExceptionCode_FileError{ 101ULL };

struct RuntimeException : Exception
{
private:
	constexpr static const char* default_message = "Runtime exception occurred!";

public:
	RuntimeException() noexcept :
		RuntimeException{ default_message, ExceptionCode_Runtime }
	{}

	explicit RuntimeException(const String& exception_message) noexcept :
		RuntimeException{ exception_message, ExceptionCode_Runtime }
	{}

	explicit RuntimeException(const char* exception_message) noexcept :
		RuntimeException{ exception_message, ExceptionCode_Runtime }
	{}

protected:
	AOL_MACRO_EXCEPTION_CREATE_INHERITEE_CONSTRUCTORS(RuntimeException, Exception)

};

struct FileException : RuntimeException
{
private:
	constexpr static const char* default_message = "There was an error processing the file!";

public:
	FileException() noexcept :
		FileException{ default_message, ExceptionCode_FileError }
	{}

	explicit FileException(const String& exception_message) noexcept :
		FileException{ exception_message, ExceptionCode_FileError }
	{}

	explicit FileException(const char* exception_message) noexcept :
		FileException{ exception_message, ExceptionCode_FileError }
	{}

protected:
	AOL_MACRO_EXCEPTION_CREATE_INHERITEE_CONSTRUCTORS(FileException, RuntimeException)

};

}


#endif // AOL_HEADER_INTERNAL_EXCEPTIONS_RUNTIME_H