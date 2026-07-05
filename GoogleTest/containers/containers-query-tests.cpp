/********************************************************************
* Container query function tests: GetBeginIt, GetEndIt, GetContainerSize, etc.
********************************************************************/

#include "pch.h"

namespace {

class ContainerQueriesTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ContainerQueriesTest, GetBeginItAndEndIt) {
    AoL::Vector<int> vec{ 1, 2, 3 };
    auto begin = AoL::GetBeginIt(vec);
    auto end = AoL::GetEndIt(vec);

    EXPECT_NE(begin, end);
    EXPECT_EQ(*begin, 1);
}

TEST_F(ContainerQueriesTest, GetContainerSize) {
    AoL::Vector<double> vec{ 1.1, 2.2, 3.3, 4.4 };
    EXPECT_EQ(AoL::GetContainerSize(vec), 4);
}

TEST_F(ContainerQueriesTest, IsContainerEmpty) {
    AoL::Vector<int> vec;
    EXPECT_TRUE(AoL::IsContainerEmpty(vec));

    vec.push_back(42);
    EXPECT_FALSE(AoL::IsContainerEmpty(vec));
}

TEST_F(ContainerQueriesTest, GetContainerData) {
    AoL::Vector<int> vec{ 1, 2, 3, 4 };
    auto data = AoL::GetContainerData(vec);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0], 1);
    EXPECT_EQ(data[3], 4);
}

TEST_F(ContainerQueriesTest, ReverseIterators) {
    AoL::Vector<int> vec{ 1, 2, 3 };
    auto rbegin = AoL::GetBeginReverseIt(vec);
    EXPECT_EQ(*rbegin, 3);
}

} // namespace
