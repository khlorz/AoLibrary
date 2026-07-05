/********************************************************************
* Set container tests: KeyOrderSet, HashSet
********************************************************************/

#include "pch.h"

namespace
{

// ===================================================================
// KEY-ORDERED SET TESTS
// ===================================================================

class KeyOrderSetTest : public ::testing::Test
{
protected:
    using TestSet = AoL::KeyOrderSet<int>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(KeyOrderSetTest, InsertAndFind)
{
    TestSet set;
    set.insert(10);
    set.insert(5);
    set.insert(15);

    EXPECT_EQ(AoL::GetContainerSize(set), 3);
    EXPECT_NE(set.find(10), AoL::GetEndIt(set));
}

TEST_F(KeyOrderSetTest, KeyOrdering)
{
    TestSet set;
    set.insert(30);
    set.insert(10);
    set.insert(20);

    auto it = AoL::GetBeginIt(set);
    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
    ++it;
    EXPECT_EQ(*it, 30);
}

TEST_F(KeyOrderSetTest, NoDuplicates)
{
    TestSet set;
    set.insert(5);
    set.insert(5);
    set.insert(5);

    EXPECT_EQ(AoL::GetContainerSize(set), 1);
}

TEST_F(KeyOrderSetTest, Erase)
{
    TestSet set{ 1, 2, 3, 4, 5 };
    set.erase(3);

    EXPECT_EQ(AoL::GetContainerSize(set), 4);
    EXPECT_EQ(set.find(3), AoL::GetEndIt(set));
}

// ===================================================================
// HASH SET TESTS
// ===================================================================

class HashSetTest : public ::testing::Test
{
protected:
    using TestSet = AoL::HashSet<int>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(HashSetTest, InsertAndFind)
{
    TestSet set;
    set.insert(42);
    set.insert(100);
    set.insert(7);

    EXPECT_EQ(AoL::GetContainerSize(set), 3);
    EXPECT_NE(set.find(42), AoL::GetEndIt(set));
}

TEST_F(HashSetTest, NoDuplicates)
{
    TestSet set;
    set.insert(1);
    set.insert(1);
    set.insert(1);

    EXPECT_EQ(AoL::GetContainerSize(set), 1);
}

TEST_F(HashSetTest, Erase)
{
    TestSet set{ 10, 20, 30 };
    set.erase(20);

    EXPECT_EQ(AoL::GetContainerSize(set), 2);
    EXPECT_EQ(set.find(20), AoL::GetEndIt(set));
}

} // namespace
