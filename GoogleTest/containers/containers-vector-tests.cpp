/********************************************************************
* Vector container tests
********************************************************************/

#include "pch.h"

namespace {

class VectorTest : public ::testing::Test {
protected:
    using TestVector = AoL::Vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(VectorTest, DefaultConstruction) {
    TestVector vec;
    EXPECT_TRUE(AoL::IsContainerEmpty(vec));
    EXPECT_EQ(AoL::GetContainerSize(vec), 0);
}

TEST_F(VectorTest, PushBackAndAccess) {
    TestVector vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[2], 30);
}

TEST_F(VectorTest, PopBack) {
    TestVector vec{ 1, 2, 3 };
    vec.pop_back();

    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
}

TEST_F(VectorTest, ClearAndEmpty) {
    TestVector vec{ 1, 2, 3 };
    EXPECT_FALSE(AoL::IsContainerEmpty(vec));

    vec.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(vec));
    EXPECT_EQ(AoL::GetContainerSize(vec), 0);
}

TEST_F(VectorTest, Reserve) {
    TestVector vec;
    vec.reserve(100);
    EXPECT_GE(vec.capacity(), 100);
}

TEST_F(VectorTest, IterationForward) {
    TestVector vec{ 1, 2, 3, 4, 5 };
    int sum = 0;
    for (auto it = AoL::GetBeginIt(vec); it != AoL::GetEndIt(vec); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 15);
}

TEST_F(VectorTest, RangeIteration) {
    TestVector vec{ 10, 20, 30 };
    int count = 0;
    for (auto& elem : vec) {
        count++;
    }
    EXPECT_EQ(count, 3);
}

} // namespace
