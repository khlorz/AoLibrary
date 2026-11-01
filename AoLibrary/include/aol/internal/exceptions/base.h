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

#include "type_safe/strong_typedef.hpp"

#include <exception>


// HELPER MACRO
// This is for creating a protected constructor for the child class
#define AOL_MACRO_EXCEPTION_CREATE_INHERITEE_CONSTRUCTORS(ClassName, BaseName)		\
explicit ClassName(const String& exception_message, ExceptionCode code) noexcept :			\
	BaseName{ exception_message, code }														\
	{}																						\
																							\
explicit ClassName(const char* exception_message, ExceptionCode code) noexcept :			\
	BaseName{ exception_message, code }														\
{}


namespace AoL
{

struct AOL_EMPTY_BASE_OPTIMIZATION ExceptionCode : type_safe::strong_typedef<ExceptionCode, AoL::SizeT>,
	type_safe::strong_typedef_op::integer_arithmetic<ExceptionCode>,
	type_safe::strong_typedef_op::equality_comparison<ExceptionCode>,
	type_safe::strong_typedef_op::relational_comparison<ExceptionCode>
{
private:
	using BaseT = type_safe::strong_typedef<ExceptionCode, AoL::SizeT>;

public:
	using BaseT::strong_typedef;
	using UType = AoL::SizeT;
};

// Exception ExceptionCode values

constexpr ExceptionCode ExceptionCode_Unknown{ 0ULL };
constexpr ExceptionCode ExceptionCode_Base{ 100ULL };

struct Exception : std::exception
{
private:
	constexpr static const char* default_message = "Unknown exception thrown!";

	ExceptionCode exception_code;
	String exception_message;

public:
	Exception() noexcept :
		Exception{ default_message, ExceptionCode_Unknown }
	{}

	explicit Exception(const String& str) noexcept :
		Exception{ str, ExceptionCode_Base }
	{}

	explicit Exception(const char* str) noexcept :
		Exception{ str, ExceptionCode_Base }
	{}

protected:
	explicit Exception(const String& str, ExceptionCode code) noexcept :
		exception_code{ code },
		exception_message{ CreateExceptionMessage(str.c_str(), exception_code) }
	{}

	explicit Exception(const char* str, ExceptionCode code) noexcept :
		exception_code{ code },
		exception_message{ CreateExceptionMessage(str, exception_code) }
	{}

public:
	const char* what() const noexcept override
	{
		return exception_message.c_str();
	}

	ExceptionCode exc_code() const noexcept
	{
		return exception_code;
}

	static String CreateExceptionMessage(const char* str, ExceptionCode code) noexcept
	{
		String out_str(
			std::char_traits<char>::length(str) +
			std::numeric_limits<ExceptionCode::UType>::digits10 + 
			std::char_traits<char>::length("Exception caught! Code #: M") +
			1,
			'\0'
		);
		AoL::StrPrintF(
			out_str.data(),
			"Exception caught! Code %zu: %s",
			code.value_,
			str
		);
		return out_str;
	}
};
}

#endif // AOL_HEADER_INTERNAL_EXCEPTIONS_BASE_H