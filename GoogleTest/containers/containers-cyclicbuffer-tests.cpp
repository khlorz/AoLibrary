/********************************************************************
* Cyclic buffer tests: fixed and dynamic
********************************************************************/


#include "pch.h"

#include "aol/cyclic_buffer.h"
#include "aol/utilities.h"


namespace
{

// ===================================================================
// CYCLIC BUFFER FIXED TESTS
// ===================================================================

class CyclicBufferFixedTest : public ::testing::Test
{
protected:
    static constexpr AoL::SizeT buffer_size = 8;
    using TestBuffer = AoL::CyclicBufferF<int, buffer_size>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

}

TEST_F(CyclicBufferFixedTest, PushAndSize)
{
    TestBuffer buf;
    buf.push_back(10);
    buf.push_back(20);
    buf.push_back(30);

    EXPECT_EQ(buf.size(), 3);
}

TEST_F(CyclicBufferFixedTest, FIFOOrder)
{
    TestBuffer buf;
    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);

    EXPECT_EQ(buf.front(), 1);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 2);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 3);
    buf.pop_front();

    EXPECT_TRUE(buf.empty());
}

TEST_F(CyclicBufferFixedTest, Overflow)
{
    TestBuffer buf;
    for (int i = 0; i < 8; ++i)
    {
        buf.push_back(i);
    }
    EXPECT_EQ(buf.size(), 8);
    EXPECT_TRUE(buf.full());

    buf.push_back(99);
    EXPECT_EQ(buf.size(), 8);

    EXPECT_EQ(buf.front(), 1);
}

TEST_F(CyclicBufferFixedTest, Empty)
{
    TestBuffer buf;
    EXPECT_TRUE(buf.empty());

    buf.push_back(5);
    EXPECT_FALSE(buf.empty());

    buf.pop_front();
    EXPECT_TRUE(buf.empty());
}

TEST_F(CyclicBufferFixedTest, BackAccess)
{
    TestBuffer buf;
    buf.push_back(10);
    buf.push_back(20);
    buf.push_back(30);

    EXPECT_EQ(buf.back(), 30);

    buf.pop_back();
    EXPECT_EQ(buf.back(), 20);
}

TEST_F(CyclicBufferFixedTest, Clear)
{
    TestBuffer buf;
    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);

    EXPECT_FALSE(buf.empty());

    buf.clear();
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), 0);
}

TEST_F(CyclicBufferFixedTest, RandomAccess)
{
    TestBuffer buf;

    for (int i = 0; i < 5; ++i)
    {
        buf.push_back(i * 10);
    }

    EXPECT_EQ(buf[0], 0);
    EXPECT_EQ(buf[2], 20);
    EXPECT_EQ(buf[4], 40);
}

TEST_F(CyclicBufferFixedTest, ForwardIteration)
{
    TestBuffer buf;

    for (int i = 1; i <= 4; ++i)
    {
        buf.push_back(i);
    }

    int sum = 0;

    for (auto it = buf.begin(); it != buf.end(); ++it)
    {
        sum += *it;
    }

    EXPECT_EQ(sum, 10);
}

// ===================================================================
// CYCLIC BUFFER DYNAMIC TESTS
// ===================================================================

class CyclicBufferDynamicTest : public ::testing::Test
{
protected:
    using TestBuffer = AoL::CyclicBufferD<int>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(CyclicBufferDynamicTest, GrowsAsNeeded)
{
    TestBuffer buf(128);
    for (int i = 0; i < 100; ++i)
    {
        buf.push_back(i);
    }
    EXPECT_EQ(buf.size(), 100);
}

TEST_F(CyclicBufferDynamicTest, FIFOOrder)
{
    TestBuffer buf(16);
    buf.push_back(5);
    buf.push_back(10);
    buf.push_back(15);

    EXPECT_EQ(buf.front(), 5);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 10);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 15);
    buf.pop_front();

    EXPECT_TRUE(buf.empty());
}

TEST_F(CyclicBufferDynamicTest, Overflow)
{
    TestBuffer buf(8);

    for (int i = 0; i < 10; ++i)
    {
        buf.push_back(i);
    }

    EXPECT_EQ(buf.size(), 8);
    EXPECT_TRUE(buf.full());

    EXPECT_EQ(buf.front(), 2);
}

TEST_F(CyclicBufferDynamicTest, EmplaceBack)
{
    TestBuffer buf(4);
    buf.emplace_back(42);
    buf.emplace_back(99);

    EXPECT_EQ(buf.size(), 2);
    EXPECT_EQ(buf[0], 42);
    EXPECT_EQ(buf[1], 99);
}

TEST_F(CyclicBufferDynamicTest, BackAndPopBack)
{
    TestBuffer buf(8);
    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);

    EXPECT_EQ(buf.back(), 3);

    buf.pop_back();
    EXPECT_EQ(buf.size(), 2);
    EXPECT_EQ(buf.back(), 2);
}

TEST_F(CyclicBufferDynamicTest, Clear)
{
    TestBuffer buf(8);
    buf.push_back(10);
    buf.push_back(20);

    EXPECT_FALSE(buf.empty());

    buf.clear();
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), 0);
}

TEST_F(CyclicBufferDynamicTest, RandomAccess)
{
    TestBuffer buf(8);

    for (int i = 0; i < 6; ++i)
    {
        buf.push_back(i * 5);
    }

    EXPECT_EQ(buf[0], 0);
    EXPECT_EQ(buf[3], 15);
}

TEST_F(CyclicBufferDynamicTest, ForwardIteration)
{
    TestBuffer buf(8);

    for (int i = 1; i <= 3; ++i)
    {
        buf.push_back(i * 10);
    }

    int sum = 0;

    for (auto it = buf.begin(); it != buf.end(); ++it)
    {
        sum += *it;
    }

    EXPECT_EQ(sum, 60);
}

TEST_F(CyclicBufferDynamicTest, IncreaseThenDecreaseCapacity)
{
    TestBuffer buf(4);
    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);
    buf.push_back(4);

    EXPECT_TRUE(buf.full());

    buf.increase_capacity(8);

    buf.push_back(5);
    buf.push_back(6);
    buf.push_back(7);
    buf.push_back(8);

    EXPECT_TRUE(buf.full());
    EXPECT_EQ(buf.capacity(), 8);
    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[7], 8);

    buf.decrease_capacity(4);
    EXPECT_EQ(buf.capacity(), 4);
    EXPECT_EQ(buf.size(), 4);
    EXPECT_EQ(buf[0], 1);
}
