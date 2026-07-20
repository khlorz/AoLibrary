/********************************************************************
* Map container tests: KeyOrderMap, HashMap, InsertOrderMap
********************************************************************/

#include "pch.h"

#include "aol/key_ordered_map.h"
#include "aol/insert_ordered_map.h"
#include "aol/hash_map.h"

#include "aol/utilities.h"

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

}

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

TEST_F(KeyOrderMapTest, ClearAndEmpty)
{
    TestMap map;
    EXPECT_TRUE(AoL::IsContainerEmpty(map));

    map[1] = "one";
    EXPECT_FALSE(AoL::IsContainerEmpty(map));

    map.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(map));
    EXPECT_EQ(AoL::GetContainerSize(map), 0);
}

TEST_F(KeyOrderMapTest, ConstFind)
{
    TestMap map;
    map[10] = "ten";

    const TestMap& const_map = map;
    auto it = const_map.find(10);
    EXPECT_NE(it, const_map.end());
    EXPECT_EQ(it->second, "ten");
}

TEST_F(KeyOrderMapTest, OperatorBracket)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";

    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
}

TEST_F(KeyOrderMapTest, ForwardIteration)
{
    TestMap map;
    map[3] = "three";
    map[1] = "one";
    map[2] = "two";

    std::vector<int> keys;

    for (auto& pair : map)
    {
        keys.push_back(pair.first);
    }

    EXPECT_EQ(keys[0], 1);
    EXPECT_EQ(keys[1], 2);
    EXPECT_EQ(keys[2], 3);
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

TEST_F(HashMapTest, OperatorBracketAccess)
{
    TestMap map;
    map[5] = "five";

    EXPECT_EQ(map[5], "five");
}

TEST_F(HashMapTest, AtAccess)
{
    TestMap map;
    map[100] = "hundred";

    EXPECT_EQ(map.at(100), "hundred");

    map.at(100) = "HUNDRED";
    EXPECT_EQ(map[100], "HUNDRED");
}

TEST_F(HashMapTest, ForwardIteration)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";
    map[3] = "three";

    int count = 0;

    for (auto it = map.begin(); it != map.end(); ++it)
    {
        count++;
    }

    EXPECT_EQ(count, 3);
}

TEST_F(HashMapTest, Reserve)
{
    TestMap map;
    map.reserve(50);

    for (int i = 0; i < 25; ++i)
    {
        map[i] = "val_" + std::to_string(i);
    }

    EXPECT_EQ(AoL::GetContainerSize(map), 25);
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

TEST_F(InsertOrderMapTest, OperatorBracketAccess)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";

    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
}

TEST_F(InsertOrderMapTest, Erase)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";
    map[3] = "three";

    map.erase(2);
    EXPECT_EQ(AoL::GetContainerSize(map), 2);
    EXPECT_EQ(map.find(2), AoL::GetEndIt(map));
}

TEST_F(InsertOrderMapTest, ClearAndEmpty)
{
    TestMap map;
    EXPECT_TRUE(AoL::IsContainerEmpty(map));

    map[42] = "answer";
    EXPECT_FALSE(AoL::IsContainerEmpty(map));

    map.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(map));
    EXPECT_EQ(AoL::GetContainerSize(map), 0);
}

TEST_F(InsertOrderMapTest, ForwardIteration)
{
    TestMap map;
    map[3] = "three";
    map[1] = "one";
    map[2] = "two";

    std::vector<int> keys;

    for (auto& pair : map)
    {
        keys.push_back(pair.first);
    }

    EXPECT_EQ(keys[0], 3);
    EXPECT_EQ(keys[1], 1);
    EXPECT_EQ(keys[2], 2);
}

