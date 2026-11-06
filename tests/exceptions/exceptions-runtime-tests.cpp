/********************************************************************
* Runtime exception tests - construction, codes and hierarchy
********************************************************************/


#include "pch.h"
#include "config.h"
#if AOL_TEST_EXCEPTIONS_RUNTIME

#include "aol/exceptions.h"

#include <type_traits>

// ------------------------------------------------------------------
// RuntimeException
// ------------------------------------------------------------------

TEST(RuntimeExceptionTest, CodesHaveExpectedValues)
{
    static_assert(AoL::ExceptionCode_Runtime.value_ == 1ULL);
    static_assert(AoL::ExceptionCode_FileError.value_ == 101ULL);
}

TEST(RuntimeExceptionTest, ConstructorsAreNoexcept)
{
    static_assert(std::is_nothrow_default_constructible_v<AoL::RuntimeException>);
    static_assert(std::is_nothrow_constructible_v<AoL::RuntimeException, const AoL::String&>);
    static_assert(std::is_nothrow_constructible_v<AoL::RuntimeException, const char*>);
}

TEST(RuntimeExceptionTest, DefaultCtorUsesDefaultMessageAndCode)
{
    AoL::RuntimeException e;
    AoL::String expected = AoL::Exception::CreateExceptionMessage("Runtime exception occurred!", AoL::ExceptionCode_Runtime);
    EXPECT_STREQ(e.what(), expected.c_str());
    EXPECT_TRUE(e.exc_code() == AoL::ExceptionCode_Runtime);
}

TEST(RuntimeExceptionTest, CtorFromLiteralStoresMessage)
{
    AoL::RuntimeException e("runtime boom");
    AoL::String expected = AoL::Exception::CreateExceptionMessage("runtime boom", AoL::ExceptionCode_Runtime);
    EXPECT_STREQ(e.what(), expected.c_str());
    EXPECT_TRUE(e.exc_code() == AoL::ExceptionCode_Runtime);
}

TEST(RuntimeExceptionTest, CtorFromStringStoresMessage)
{
    AoL::String message = "runtime string";
    AoL::RuntimeException e(message);
    AoL::String expected = AoL::Exception::CreateExceptionMessage(message.c_str(), AoL::ExceptionCode_Runtime);
    EXPECT_STREQ(e.what(), expected.c_str());
}

// Caught as AoL::Exception (parent).
TEST(RuntimeExceptionTest, CaughtAsBaseException)
{
    try
    {
        throw AoL::RuntimeException("parent catch");
    }
    catch (const AoL::Exception& e)
    {
        EXPECT_TRUE(e.exc_code() == AoL::ExceptionCode_Runtime);
    }
}

// Caught as std::exception (AoL::Exception's parent).
TEST(RuntimeExceptionTest, CaughtAsStdException)
{
    try
    {
        throw AoL::RuntimeException("grandparent catch");
    }
    catch (const std::exception& e)
    {
        EXPECT_STREQ(e.what(), "Runtime exception occurred!");
    }
}

// ------------------------------------------------------------------
// FileException
// ------------------------------------------------------------------

TEST(FileExceptionTest, ConstructorsAreNoexcept)
{
    static_assert(std::is_nothrow_default_constructible_v<AoL::FileException>);
    static_assert(std::is_nothrow_constructible_v<AoL::FileException, const AoL::String&>);
    static_assert(std::is_nothrow_constructible_v<AoL::FileException, const char*>);
}

TEST(FileExceptionTest, DefaultCtorUsesDefaultMessageAndCode)
{
    AoL::FileException e;
    AoL::String expected = AoL::Exception::CreateExceptionMessage("There was an error processing the file!", AoL::ExceptionCode_FileError);
    EXPECT_STREQ(e.what(), expected.c_str());
    EXPECT_TRUE(e.exc_code() == AoL::ExceptionCode_FileError);
}

TEST(FileExceptionTest, CtorFromLiteralStoresMessage)
{
    AoL::FileException e("missing file");
    AoL::String expected = AoL::Exception::CreateExceptionMessage("missing file", AoL::ExceptionCode_FileError);
    EXPECT_STREQ(e.what(), expected.c_str());
    EXPECT_TRUE(e.exc_code() == AoL::ExceptionCode_FileError);
}

TEST(FileExceptionTest, CtorFromStringStoresMessage)
{
    AoL::String message = "file string";
    AoL::FileException e(message);
    AoL::String expected = AoL::Exception::CreateExceptionMessage(message.c_str(), AoL::ExceptionCode_FileError);
    EXPECT_STREQ(e.what(), expected.c_str());
}

// Caught as RuntimeException (parent).
TEST(FileExceptionTest, CaughtAsRuntimeException)
{
    try
    {
        throw AoL::FileException("parent catch");
    }
    catch (const AoL::RuntimeException& e)
    {
        EXPECT_TRUE(e.exc_code() == AoL::ExceptionCode_FileError);
    }
}

// Caught as AoL::Exception (AoL::RuntimeException's parent).
TEST(FileExceptionTest, CaughtAsBaseException)
{
    try
    {
        throw AoL::FileException("grandparent catch");
    }
    catch (const AoL::Exception& e)
    {
        EXPECT_TRUE(e.exc_code() == AoL::ExceptionCode_FileError);
    }
}

// Caught as std::exception (AoL::Exception's parent).
TEST(FileExceptionTest, CaughtAsStdException)
{
    try
    {
        throw AoL::FileException("great-grandparent catch");
    }
    catch (const std::exception& e)
    {
        EXPECT_STREQ(e.what(), "There was an error processing the file!");
    }
}

#endif // AOL_TEST_EXCEPTIONS_RUNTIME
