/********************************************************************
* Set container tests: KeyOrderSet, InsertOrderSet, HashSet
********************************************************************/

#include "pch.h"

#include "aol/key_ordered_set.h"
#include "aol/insert_ordered_set.h"
#include "aol/hash_set.h"

#include "aol/utilities.h"

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

}

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

TEST_F(KeyOrderSetTest, Contains)
{
    TestSet set;
    set.insert(42);

    EXPECT_TRUE(set.contains(42));
    EXPECT_FALSE(set.contains(99));
}

TEST_F(KeyOrderSetTest, ClearAndEmpty)
{
    TestSet set;
    EXPECT_TRUE(AoL::IsContainerEmpty(set));

    set.insert(7);
    EXPECT_FALSE(AoL::IsContainerEmpty(set));

    set.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(set));
    EXPECT_EQ(AoL::GetContainerSize(set), 0);
}

TEST_F(KeyOrderSetTest, ForwardIteration)
{
    TestSet set;
    set.insert(30);
    set.insert(10);
    set.insert(20);

    std::vector<int> values;

    for (auto& v : set)
    {
        values.push_back(v);
    }

    EXPECT_EQ(values[0], 10);
    EXPECT_EQ(values[1], 20);
    EXPECT_EQ(values[2], 30);
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

TEST_F(HashSetTest, Contains)
{
    TestSet set;
    set.insert(100);

    EXPECT_TRUE(set.contains(100));
    EXPECT_FALSE(set.contains(999));
}

TEST_F(HashSetTest, ClearAndEmpty)
{
    TestSet set;
    EXPECT_TRUE(AoL::IsContainerEmpty(set));

    set.insert(42);
    EXPECT_FALSE(AoL::IsContainerEmpty(set));

    set.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(set));
    EXPECT_EQ(AoL::GetContainerSize(set), 0);
}

TEST_F(HashSetTest, ForwardIteration)
{
    TestSet set;
    set.insert(5);
    set.insert(10);
    set.insert(15);

    int count = 0;

    for (auto it = set.begin(); it != set.end(); ++it)
    {
        count++;
    }

    EXPECT_EQ(count, 3);
}

// ===================================================================
// INSERT-ORDERED SET TESTS
// ===================================================================

class InsertOrderSetTest : public ::testing::Test
{
protected:
    using TestSet = AoL::InsertOrderSet<int>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(InsertOrderSetTest, InsertAndFind)
{
    TestSet set;
    set.insert(10);
    set.insert(5);

    EXPECT_EQ(AoL::GetContainerSize(set), 2);
    EXPECT_NE(set.find(10), AoL::GetEndIt(set));
}

TEST_F(InsertOrderSetTest, InsertionOrder)
{
    TestSet set;
    set.insert(30);
    set.insert(10);
    set.insert(20);

    auto it = AoL::GetBeginIt(set);
    EXPECT_EQ(*it, 30);
    ++it;
    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
}

TEST_F(InsertOrderSetTest, NoDuplicates)
{
    TestSet set;
    set.insert(5);
    set.insert(5);

    EXPECT_EQ(AoL::GetContainerSize(set), 1);
}

TEST_F(InsertOrderSetTest, Erase)
{
    TestSet set{ 1, 2, 3 };
    set.erase(2);

    EXPECT_EQ(AoL::GetContainerSize(set), 2);
    EXPECT_EQ(set.find(2), AoL::GetEndIt(set));
}

TEST_F(InsertOrderSetTest, Contains)
{
    TestSet set;
    set.insert(42);

    EXPECT_TRUE(set.contains(42));
    EXPECT_FALSE(set.contains(99));
}

TEST_F(InsertOrderSetTest, ClearAndEmpty)
{
    TestSet set;
    EXPECT_TRUE(AoL::IsContainerEmpty(set));

    set.insert(7);
    EXPECT_FALSE(AoL::IsContainerEmpty(set));

    set.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(set));
}

TEST_F(InsertOrderSetTest, ForwardIteration)
{
    TestSet set;
    set.insert(1);
    set.insert(2);
    set.insert(3);

    int count = 0;

    for (auto it = set.begin(); it != set.end(); ++it)
    {
        count++;
    }

    EXPECT_EQ(count, 3);
}
