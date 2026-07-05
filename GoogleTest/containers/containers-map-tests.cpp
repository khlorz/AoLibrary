/********************************************************************
* Map container tests: KeyOrderMap, HashMap, InsertOrderMap
********************************************************************/

#include "pch.h"

namespace
{

// ===================================================================
// KEY-ORDERED MAP TESTS
// ===================================================================

class KeyOrderMapTest : public ::testing::Test
{
protected:
    using TestMap = AoL::KeyOrderMap<int, std::string>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(KeyOrderMapTest, InsertAndFind)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";
    map[3] = "three";

    EXPECT_EQ(AoL::GetContainerSize(map), 3);
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
}

TEST_F(KeyOrderMapTest, KeyOrdering)
{
    TestMap map;
    map[3] = "three";
    map[1] = "one";
    map[2] = "two";

    auto it = AoL::GetBeginIt(map);
    EXPECT_EQ(it->first, 1);
    ++it;
    EXPECT_EQ(it->first, 2);
    ++it;
    EXPECT_EQ(it->first, 3);
}

TEST_F(KeyOrderMapTest, Contains)
{
    TestMap map;
    map[5] = "five";

    EXPECT_NE(map.find(5), AoL::GetEndIt(map));
    EXPECT_EQ(map.find(10), AoL::GetEndIt(map));
}

TEST_F(KeyOrderMapTest, Erase)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";
    map[3] = "three";

    map.erase(2);
    EXPECT_EQ(AoL::GetContainerSize(map), 2);
    EXPECT_EQ(map.find(2), AoL::GetEndIt(map));
}

// ===================================================================
// HASH MAP TESTS
// ===================================================================

class HashMapTest : public ::testing::Test
{
protected:
    using TestMap = AoL::HashMap<int, std::string>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(HashMapTest, InsertAndRetrieve)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";
    map[42] = "answer";

    EXPECT_EQ(AoL::GetContainerSize(map), 3);
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[42], "answer");
}

TEST_F(HashMapTest, Find)
{
    TestMap map;
    map[100] = "hundred";

    auto it = map.find(100);
    EXPECT_NE(it, AoL::GetEndIt(map));
    EXPECT_EQ(it->second, "hundred");

    auto it_not_found = map.find(999);
    EXPECT_EQ(it_not_found, AoL::GetEndIt(map));
}

TEST_F(HashMapTest, ClearAndEmpty)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";

    EXPECT_FALSE(AoL::IsContainerEmpty(map));
    map.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(map));
}

TEST_F(HashMapTest, Erase)
{
    TestMap map;
    map[10] = "ten";
    map[20] = "twenty";
    map[30] = "thirty";

    map.erase(20);
    EXPECT_EQ(AoL::GetContainerSize(map), 2);
    EXPECT_EQ(map.find(20), AoL::GetEndIt(map));
}

// ===================================================================
// INSERT-ORDERED MAP TESTS
// ===================================================================

class InsertOrderMapTest : public ::testing::Test
{
protected:
    using TestMap = AoL::InsertOrderMap<int, std::string>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(InsertOrderMapTest, InsertionOrder)
{
    TestMap map;
    map[3] = "three";
    map[1] = "one";
    map[2] = "two";

    auto it = AoL::GetBeginIt(map);
    EXPECT_EQ(it->first, 3);
    ++it;
    EXPECT_EQ(it->first, 1);
    ++it;
    EXPECT_EQ(it->first, 2);
}

TEST_F(InsertOrderMapTest, Find)
{
    TestMap map;
    map[42] = "answer";

    auto it = map.find(42);
    EXPECT_NE(it, AoL::GetEndIt(map));
    EXPECT_EQ(it->second, "answer");
}

} // namespace
