/********************************************************************
* Subrange container tests
********************************************************************/

#include "pch.h"

#include "aol/subrange.h"

namespace
{

class SubrangeTest : public ::testing::Test
{
protected:
    using TestVec = std::vector<int>;
    using TestSubrange = AoL::Subrange<TestVec::iterator>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(SubrangeTest, ConstructFromIterators)
{
    TestVec vec{ 1, 2, 3, 4, 5 };
    TestSubrange sr(vec.begin(), vec.end());

    EXPECT_EQ(sr.size(), 5);
}

TEST_F(SubrangeTest, BeginEndIteration)
{
    TestVec vec{ 10, 20, 30 };
    TestSubrange sr(vec.begin(), vec.end());
    auto it = sr.begin();

    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
}

TEST_F(SubrangeTest, EmptyRange)
{
    TestVec vec{ 1, 2, 3 };
    TestSubrange sr(vec.begin(), vec.begin());

    EXPECT_TRUE(sr.empty());
    EXPECT_EQ(sr.size(), 0);
}

TEST_F(SubrangeTest, IndexAccess)
{
    TestVec vec{ 100, 200, 300, 400 };
    TestSubrange sr(vec.begin(), vec.end());

    EXPECT_EQ(sr[0], 100);
    EXPECT_EQ(sr[2], 300);
}

TEST_F(SubrangeTest, SubrangeOfSubrange)
{
    TestVec vec{ 1, 2, 3, 4, 5 };
    TestSubrange sr(vec.begin() + 1, vec.begin() + 4);

    EXPECT_EQ(sr.size(), 3);
    EXPECT_EQ(sr[0], 2);
    EXPECT_EQ(sr[2], 4);
}

} // namespace
