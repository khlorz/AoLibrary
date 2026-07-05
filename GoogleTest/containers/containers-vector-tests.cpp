/********************************************************************
* Vector container tests
********************************************************************/

#include "pch.h"

namespace
{

class VectorTest : public ::testing::Test
{
protected:
    using TestVector = AoL::Vector<int>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

}

TEST_F(VectorTest, DefaultConstruction)
{
    TestVector vec;
    EXPECT_TRUE(AoL::IsContainerEmpty(vec));
    EXPECT_EQ(AoL::GetContainerSize(vec), 0);
}

TEST_F(VectorTest, PushBackAndAccess)
{
    TestVector vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[2], 30);
}

TEST_F(VectorTest, PopBack)
{
    TestVector vec{ 1, 2, 3 };
    vec.pop_back();

    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
}

TEST_F(VectorTest, ClearAndEmpty)
{
    TestVector vec{ 1, 2, 3 };
    EXPECT_FALSE(AoL::IsContainerEmpty(vec));

    vec.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(vec));
    EXPECT_EQ(AoL::GetContainerSize(vec), 0);
}

TEST_F(VectorTest, Reserve)
{
    TestVector vec;
    vec.reserve(100);
    EXPECT_GE(vec.capacity(), 100);
}

TEST_F(VectorTest, IterationForward)
{
    TestVector vec{ 1, 2, 3, 4, 5 };
    int sum = 0;
    for (auto it = AoL::GetBeginIt(vec); it != AoL::GetEndIt(vec); ++it)
    {
        sum += *it;
    }
    EXPECT_EQ(sum, 15);
}

TEST_F(VectorTest, EmplaceBack)
{
    TestVector vec;
    vec.emplace_back(42);
    vec.emplace_back(99);

    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec[0], 42);
    EXPECT_EQ(vec[1], 99);
}

TEST_F(VectorTest, InsertAtPosition)
{
    TestVector vec{ 1, 2, 4, 5 };
    vec.insert(vec.begin() + 2, 3);

    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec[2], 3);
    EXPECT_EQ(vec[3], 4);
}

TEST_F(VectorTest, EraseElement)
{
    TestVector vec{ 10, 20, 30, 40 };
    vec.erase(vec.begin() + 1);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 30);
}

TEST_F(VectorTest, FrontAndBack)
{
    TestVector vec{ 7, 14, 21 };

    EXPECT_EQ(vec.front(), 7);
    EXPECT_EQ(vec.back(), 21);

    vec.front() = 99;
    EXPECT_EQ(vec[0], 99);
}

TEST_F(VectorTest, DataAccess)
{
    TestVector vec{ 1, 2, 3, 4 };

    int* ptr = vec.data();
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr[0], 1);
    EXPECT_EQ(ptr[3], 4);

    ptr[2] = 99;
    EXPECT_EQ(vec[2], 99);
}

TEST_F(VectorTest, Resize)
{
    TestVector vec{ 1, 2, 3 };
    vec.resize(5);

    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[3], 0);

    vec.resize(2);
    EXPECT_EQ(vec.size(), 2);
}

TEST_F(VectorTest, ReverseIteration)
{
    TestVector vec{ 1, 2, 3 };

    auto it = vec.rbegin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 1);
}

TEST_F(VectorTest, CopyConstruction)
{
    TestVector original{ 1, 2, 3 };
    TestVector copy(original);

    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy[0], 1);

    copy[0] = 99;
    EXPECT_EQ(original[0], 1);
}

TEST_F(VectorTest, MoveConstruction)
{
    TestVector original{ 1, 2, 3 };
    TestVector moved(std::move(original));

    EXPECT_EQ(moved.size(), 3);
    EXPECT_EQ(moved[0], 1);
    EXPECT_EQ(moved[2], 3);
}

TEST_F(VectorTest, EqualityComparison)
{
    TestVector a{ 1, 2, 3 };
    TestVector b{ 1, 2, 3 };
    TestVector c{ 4, 5, 6 };

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}
