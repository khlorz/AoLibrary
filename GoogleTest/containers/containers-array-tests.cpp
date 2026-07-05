/********************************************************************
* Array container tests
********************************************************************/

#include "pch.h"

namespace {

class ArrayTest : public ::testing::Test {
protected:
    using TestArray = AoL::Array<int, 5>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ArrayTest, DefaultConstruction) {
    TestArray arr;
    EXPECT_EQ(arr.size(), 5);
}

TEST_F(ArrayTest, InitializerConstruction) {
    TestArray arr{ 10, 20, 30, 40, 50 };
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[4], 50);
}

TEST_F(ArrayTest, FillOperation) {
    TestArray arr{};
    arr.fill(42);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(arr[i], 42);
    }
}

TEST_F(ArrayTest, BackAndFront) {
    TestArray arr{ 1, 2, 3, 4, 99 };
    EXPECT_EQ(arr.front(), 1);
    EXPECT_EQ(arr.back(), 99);
}

TEST_F(ArrayTest, GetContainerData) {
    TestArray arr{ 1, 2, 3, 4, 5 };
    auto data = AoL::GetContainerData(arr);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0], 1);
    EXPECT_EQ(data[4], 5);
}

} // namespace
