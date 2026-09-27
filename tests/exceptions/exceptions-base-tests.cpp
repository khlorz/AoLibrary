/********************************************************************
* BaseException tests - minimal construction and portability checks
********************************************************************/


#include "pch.h"
#include "config.h"
#if AOL_TEST_EXCEPTIONS_BASE

#include "aol/exceptions.h"

#include <string>

// Default construction reports the built-in message.
TEST(BaseExceptionTest, DefaultCtorUsesDefaultMessage)
{
    AoL::Internal::BaseException e;
    EXPECT_STREQ(e.what(), "Unknown exception thrown!");
}

// Literal construction stores the message verbatim.
TEST(BaseExceptionTest, CtorFromLiteralStoresMessage)
{
    constexpr const char* test_text = "boom";

    AoL::Internal::BaseException e(test_text);
    EXPECT_STREQ(e.what(), test_text);
}

// By-value sink accepts lvalues and rvalues alike.
TEST(BaseExceptionTest, CtorFromStringLvalueAndRvalue)
{
    constexpr const char* test_text_1 = "left";
    AoL::String lv = test_text_1;
    EXPECT_STREQ(AoL::Internal::BaseException(lv).what(), test_text_1);

    constexpr const char* test_text_2 = "right";
    EXPECT_STREQ(AoL::Internal::BaseException(AoL::String(test_text_2)).what(), test_text_2);
}

// Portable catch: message survives as const std::exception&.
TEST(BaseExceptionTest, CaughtAsStdExceptionKeepsMessage)
{
    constexpr const char* test_text = "caught";
    try
    {
        throw AoL::Internal::BaseException(test_text);
    }
    catch (const std::exception& e)
    {
        EXPECT_STREQ(e.what(), test_text);
    }
}

// Copying preserves the message.
TEST(BaseExceptionTest, CopyPreservesMessage)
{
    constexpr const char* test_text = "copy text";
    AoL::Internal::BaseException src(test_text);
    AoL::Internal::BaseException dst = src;
    EXPECT_STREQ(dst.what(), test_text);
}

#endif // AOL_TEST_EXCEPTIONS_BASE
