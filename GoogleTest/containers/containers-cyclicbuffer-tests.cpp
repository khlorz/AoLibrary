/********************************************************************
* Cyclic buffer tests: fixed and dynamic
********************************************************************/

#include "pch.h"

namespace {

// ===================================================================
// CYCLIC BUFFER FIXED TESTS
// ===================================================================

class CyclicBufferFixedTest : public ::testing::Test {
protected:
    static constexpr AoL::SizeT BUFFER_SIZE = 8;
    using TestBuffer = AoL::CyclicBufferF<int, BUFFER_SIZE>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CyclicBufferFixedTest, PushAndSize) {
    TestBuffer buf;
    buf.push_back(10);
    buf.push_back(20);
    buf.push_back(30);

    EXPECT_EQ(buf.size(), 3);
}

TEST_F(CyclicBufferFixedTest, FIFOOrder) {
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

TEST_F(CyclicBufferFixedTest, Overflow) {
    TestBuffer buf;
    for (int i = 0; i < 8; ++i) {
        buf.push_back(i);
    }
    EXPECT_EQ(buf.size(), 8);
    EXPECT_TRUE(buf.full());

    buf.push_back(99);
    EXPECT_EQ(buf.size(), 8);

    EXPECT_EQ(buf.front(), 1);
}

TEST_F(CyclicBufferFixedTest, Empty) {
    TestBuffer buf;
    EXPECT_TRUE(buf.empty());

    buf.push_back(5);
    EXPECT_FALSE(buf.empty());

    buf.pop_front();
    EXPECT_TRUE(buf.empty());
}

// ===================================================================
// CYCLIC BUFFER DYNAMIC TESTS
// ===================================================================

class CyclicBufferDynamicTest : public ::testing::Test {
protected:
    using TestBuffer = AoL::CyclicBufferD<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CyclicBufferDynamicTest, GrowsAsNeeded) {
    TestBuffer buf(128);
    for (int i = 0; i < 100; ++i) {
        buf.push_back(i);
    }
    EXPECT_EQ(buf.size(), 100);
}

TEST_F(CyclicBufferDynamicTest, FIFOOrder) {
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

} // namespace
