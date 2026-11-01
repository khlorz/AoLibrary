/********************************************************************
* Exception tests - minimal construction and portability checks
********************************************************************/


#include "pch.h"
#include "config.h"
#if AOL_TEST_EXCEPTIONS_BASE

#include "aol/exceptions.h"

#include <string>

// Default construction reports the built-in message.
TEST(BaseExceptionTest, DefaultCtorUsesDefaultMessage)
{
    AoL::Exception e;
    AoL::String exception_string = AoL::Exception::CreateExceptionMessage("Unknown exception thrown!", AoL::ExceptionCode_Unknown);
    EXPECT_STREQ(e.what(), exception_string.c_str());
}

// Literal construction stores the message verbatim.
TEST(BaseExceptionTest, CtorFromLiteralStoresMessage)
{
    constexpr const char* test_text = "boom";
    AoL::String exception_string = AoL::Exception::CreateExceptionMessage(test_text, AoL::ExceptionCode_Base);

    AoL::Exception e(test_text);
    EXPECT_STREQ(e.what(), exception_string.c_str());
}

// By-value sink accepts lvalues and rvalues alike.
TEST(BaseExceptionTest, CtorFromStringLvalueAndRvalue)
{
    constexpr const char* test_text_1 = "left";
    AoL::String exception_string_1 = AoL::Exception::CreateExceptionMessage(test_text_1, AoL::ExceptionCode_Base);
    AoL::String lv = test_text_1;
    EXPECT_STREQ(AoL::Exception(lv).what(), exception_string_1.c_str());

    constexpr const char* test_text_2 = "right";
    AoL::String exception_string_2 = AoL::Exception::CreateExceptionMessage(test_text_2, AoL::ExceptionCode_Base);
    EXPECT_STREQ(AoL::Exception(AoL::String(test_text_2)).what(), exception_string_2.c_str());
}

// Portable catch: message survives as const std::exception&.
TEST(BaseExceptionTest, CaughtAsStdExceptionKeepsMessage)
{
    constexpr const char* test_text = "caught";
    AoL::String exception_string = AoL::Exception::CreateExceptionMessage(test_text, AoL::ExceptionCode_Base);
    try
    {
        throw AoL::Exception(test_text);
    }
    catch (const std::exception& e)
    {
        EXPECT_STREQ(e.what(), exception_string.c_str());
    }
}

// Copying preserves the message.
TEST(BaseExceptionTest, CopyPreservesMessage)
{
    constexpr const char* test_text = "copy text";
    AoL::String exception_text = AoL::Exception::CreateExceptionMessage(test_text, AoL::ExceptionCode_Base);
    AoL::Exception src(test_text);
    AoL::Exception dst = src;
    EXPECT_STREQ(dst.what(), exception_text.c_str());
}

#endif // AOL_TEST_EXCEPTIONS_BASE
