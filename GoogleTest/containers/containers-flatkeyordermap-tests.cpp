/********************************************************************
* FlatKeyOrderMap tests: all container operations
********************************************************************/

#include "pch.h"

namespace
{

struct TestData
{
    int id;
    std::string description;

    TestData(int i = 0, const std::string& d = "") :
        id(i),
        description(d)
    {
    }

    bool operator==(const TestData& other) const
    {
        return id == other.id && description == other.description;
    }

    bool operator<(const TestData& other) const
    {
        return id < other.id;
    }
};

}

// ===================================================================
// FLAT KEY ORDER MAP BASIC TESTS
// ===================================================================

class FlatKeyOrderMapBasicTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapBasicTest, DefaultConstruction)
{
    TestMap map;
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

TEST_F(FlatKeyOrderMapBasicTest, ConstructionWithCapacity)
{
    TestMap map(100);
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

TEST_F(FlatKeyOrderMapBasicTest, ConstructionFromIterators)
{
    std::vector<AoL::FlatKeyOrderMapPair<int, std::string>> data
    {
        {1, "one"},
        {3, "three"},
        {2, "two"}
    };
    TestMap map(data.begin(), data.end());

    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.begin()->first, 1);
    EXPECT_EQ((map.begin() + 1)->first, 2);
    EXPECT_EQ((map.begin() + 2)->first, 3);
}

// ===================================================================
// BUILD PATTERN TESTS
// ===================================================================

class FlatKeyOrderMapBuildPatternTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapBuildPatternTest, BuildStartAddEnd)
{
    TestMap map;
    map.build_start();

    map.build_add(3, "three");
    map.build_add(1, "one");
    map.build_add(2, "two");

    map.build_end();

    EXPECT_EQ(map.size(), 3);
    EXPECT_TRUE(
        std::is_sorted(
            map.begin(), 
            map.end(),
            [](const auto& a, const auto& b)
            {
                return a.first < b.first;
            }
        )
    );
}

TEST_F(FlatKeyOrderMapBuildPatternTest, BuildWithManyElements)
{
    TestMap map(1000);
    map.build_start();

    for (int i = 100; i >= 0; --i)
    {
        map.build_add(i, "value_" + std::to_string(i));
    }

    map.build_end();

    EXPECT_EQ(map.size(), 101);
    EXPECT_TRUE(
        std::is_sorted(
            map.begin(), 
            map.end(),
            [](const auto& a, const auto& b)
            {
                return a.first < b.first;
            }
        )
    );
}

TEST_F(FlatKeyOrderMapBuildPatternTest, BuildPreservesOrder)
{
    TestMap map;
    map.build_start();

    map.build_add(5, "five");
    map.build_add(2, "two");
    map.build_add(8, "eight");
    map.build_add(1, "one");

    map.build_end();

    auto it = map.begin();
    EXPECT_EQ(it->first, 1);
    ++it;
    EXPECT_EQ(it->first, 2);
    ++it;
    EXPECT_EQ(it->first, 5);
    ++it;
    EXPECT_EQ(it->first, 8);
}

// ===================================================================
// INSERT OPERATIONS TESTS
// ===================================================================

class FlatKeyOrderMapInsertTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapInsertTest, InsertSingleElement)
{
    TestMap map;
    map.insert(5, "five");

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[5], "five");
}

TEST_F(FlatKeyOrderMapInsertTest, InsertMultipleElements)
{
    TestMap map;
    map.insert(5, "five");
    map.insert(2, "two");
    map.insert(8, "eight");

    EXPECT_EQ(map.size(), 3);
    EXPECT_TRUE(
        std::is_sorted(
            map.begin(), 
            map.end(),
            [](const auto& a, const auto& b)
            {
                return a.first < b.first;
            }
        )
    );
}

TEST_F(FlatKeyOrderMapInsertTest, InsertMaintainsKeyOrder)
{
    TestMap map;
    map.insert(10, "ten");
    map.insert(5, "five");
    map.insert(15, "fifteen");
    map.insert(3, "three");

    EXPECT_EQ(map.begin()->first, 3);
    EXPECT_EQ((map.begin() + 1)->first, 5);
    EXPECT_EQ((map.begin() + 2)->first, 10);
    EXPECT_EQ((map.begin() + 3)->first, 15);
}

TEST_F(FlatKeyOrderMapInsertTest, InsertInOrder)
{
    TestMap map;

    for (int i = 1; i <= 10; ++i)
    {
        map.insert(i, "value_" + std::to_string(i));
    }

    EXPECT_EQ(map.size(), 10);
    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(),
        [](const auto& a, const auto& b)
        {
            return a.first < b.first;
        }));
}

TEST_F(FlatKeyOrderMapInsertTest, InsertReverseOrder)
{
    TestMap map;

    for (int i = 10; i >= 1; --i)
    {
        map.insert(i, "value_" + std::to_string(i));
    }

    EXPECT_EQ(map.size(), 10);
    EXPECT_TRUE(
        std::is_sorted(
            map.begin(), 
            map.end(),
            [](const auto& a, const auto& b)
            {
                return a.first < b.first;
            }
        )
    );
}

// ===================================================================
// ACCESS OPERATIONS TESTS
// ===================================================================

class FlatKeyOrderMapAccessTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapAccessTest, OperatorBracketAccess)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");
    map.insert(3, "three");

    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
    EXPECT_EQ(map[3], "three");
}

TEST_F(FlatKeyOrderMapAccessTest, ConstOperatorBracketAccess)
{
    TestMap map;
    map.insert(5, "five");

    const TestMap& const_map = map;
    EXPECT_EQ(const_map[5], "five");
}

TEST_F(FlatKeyOrderMapAccessTest, AtRefAccess)
{
    TestMap map;
    map.insert(10, "ten");

    auto& ref = map.at_ref(10);
    ref = "TEN";

    EXPECT_EQ(map[10], "TEN");
}

TEST_F(FlatKeyOrderMapAccessTest, AtPtrAccess)
{
    TestMap map;
    map.insert(20, "twenty");

    auto ptr = map.at_ptr(20);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, "twenty");
}

TEST_F(FlatKeyOrderMapAccessTest, AtPtrNonExistent)
{
    TestMap map;
    map.insert(5, "five");

    auto ptr = map.at_ptr(999);
    EXPECT_EQ(ptr, nullptr);
}

// ===================================================================
// FIND OPERATIONS TESTS
// ===================================================================

class FlatKeyOrderMapFindTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapFindTest, FindExistingKey)
{
    TestMap map;
    map.insert(5, "five");
    map.insert(10, "ten");

    auto ptr = map.find(5);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->first, 5);
    EXPECT_EQ(ptr->second, "five");
}

TEST_F(FlatKeyOrderMapFindTest, FindNonExistentKey)
{
    TestMap map;
    map.insert(5, "five");

    auto ptr = map.find(999);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(FlatKeyOrderMapFindTest, FindEmptyMap)
{
    TestMap map;
    auto ptr = map.find(5);

    EXPECT_EQ(ptr, nullptr);
}

TEST_F(FlatKeyOrderMapFindTest, FindConstMap)
{
    TestMap map;
    map.insert(7, "seven");

    const TestMap& const_map = map;
    auto ptr = const_map.find(7);

    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->first, 7);
}

TEST_F(FlatKeyOrderMapFindTest, FindMultipleElements)
{
    TestMap map;

    for (int i = 1; i <= 100; ++i)
    {
        map.insert(i, "value_" + std::to_string(i));
    }

    for (int i : {1, 25, 50, 75, 100})
    {
        auto ptr = map.find(i);
        EXPECT_NE(ptr, nullptr);
        EXPECT_EQ(ptr->first, i);
    }
}

// ===================================================================
// CONTAINS TESTS
// ===================================================================

class FlatKeyOrderMapContainsTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapContainsTest, ContainsExisting)
{
    TestMap map;
    map.insert(42, "answer");

    EXPECT_TRUE(map.contains(42));
}

TEST_F(FlatKeyOrderMapContainsTest, ContainsNonExistent)
{
    TestMap map;
    map.insert(42, "answer");

    EXPECT_FALSE(map.contains(999));
}

TEST_F(FlatKeyOrderMapContainsTest, ContainsEmptyMap)
{
    TestMap map;

    EXPECT_FALSE(map.contains(5));
}

TEST_F(FlatKeyOrderMapContainsTest, ContainsMultipleKeys)
{
    TestMap map;

    for (int i = 0; i < 50; ++i)
    {
        map.insert(i, "val");
    }

    for (int i = 0; i < 50; ++i)
    {
        EXPECT_TRUE(map.contains(i));
    }

    EXPECT_FALSE(map.contains(100));
}

// ===================================================================
// ITERATION TESTS
// ===================================================================

class FlatKeyOrderMapIterationTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapIterationTest, ForwardIteration)
{
    TestMap map;
    map.insert(5, "five");
    map.insert(2, "two");
    map.insert(8, "eight");

    std::vector<int> keys;

    for (auto& pair : map)
    {
        keys.push_back(pair.first);
    }

    EXPECT_EQ(keys[0], 2);
    EXPECT_EQ(keys[1], 5);
    EXPECT_EQ(keys[2], 8);
}

TEST_F(FlatKeyOrderMapIterationTest, ReverseIteration)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");
    map.insert(3, "three");

    std::vector<int> keys;

    for (auto it = map.rbegin(); it != map.rend(); ++it)
    {
        keys.push_back(it->first);
    }

    EXPECT_EQ(keys[0], 3);
    EXPECT_EQ(keys[1], 2);
    EXPECT_EQ(keys[2], 1);
}

TEST_F(FlatKeyOrderMapIterationTest, ConstIteration)
{
    TestMap map;
    map.insert(10, "ten");
    map.insert(20, "twenty");

    const TestMap& const_map = map;
    int count = 0;

    for (auto it = const_map.cbegin(); it != const_map.cend(); ++it)
    {
        count++;
    }

    EXPECT_EQ(count, 2);
}

TEST_F(FlatKeyOrderMapIterationTest, IterationOrderIsKeyOrder)
{
    TestMap map;

    for (int i = 100; i >= 1; --i)
    {
        map.insert(i, "val");
    }

    int prev_key = 0;

    for (auto& pair : map)
    {
        EXPECT_GT(pair.first, prev_key);
        prev_key = pair.first;
    }
}

// ===================================================================
// CLEAR AND SIZE TESTS
// ===================================================================

class FlatKeyOrderMapClearTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapClearTest, ClearEmptyMap)
{
    TestMap map;
    map.clear();

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

TEST_F(FlatKeyOrderMapClearTest, ClearNonEmptyMap)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");
    map.insert(3, "three");

    map.clear();

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

TEST_F(FlatKeyOrderMapClearTest, InsertAfterClear)
{
    TestMap map;
    map.insert(5, "five");
    map.clear();

    map.insert(10, "ten");

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[10], "ten");
}

TEST_F(FlatKeyOrderMapClearTest, SizeTracking)
{
    TestMap map;
    EXPECT_EQ(map.size(), 0);

    for (int i = 1; i <= 10; ++i)
    {
        map.insert(i, "val");
        EXPECT_EQ(map.size(), i);
    }
}

// ===================================================================
// DATA ACCESS TESTS
// ===================================================================

class FlatKeyOrderMapDataTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapDataTest, DataPointerAccess)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    auto data = map.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0].first, 1);
    EXPECT_EQ(data[1].first, 2);
}

TEST_F(FlatKeyOrderMapDataTest, ConstDataPointerAccess)
{
    TestMap map;
    map.insert(5, "five");

    const TestMap& const_map = map;
    auto data = const_map.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0].first, 5);
}

// ===================================================================
// CUSTOM TYPE TESTS
// ===================================================================

class FlatKeyOrderMapCustomTypeTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, TestData>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapCustomTypeTest, InsertCustomType)
{
    TestMap map;
    map.insert(1, TestData(1, "first"));
    map.insert(2, TestData(2, "second"));

    EXPECT_EQ(map[1].id, 1);
    EXPECT_EQ(map[1].description, "first");
}

TEST_F(FlatKeyOrderMapCustomTypeTest, BuildWithCustomType)
{
    TestMap map;
    map.build_start();

    map.build_add(5, TestData(5, "five"));
    map.build_add(2, TestData(2, "two"));
    map.build_add(8, TestData(8, "eight"));

    map.build_end();

    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map[5].id, 5);
}

// ===================================================================
// EDGE CASES TESTS
// ===================================================================

class FlatKeyOrderMapEdgeCasesTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapEdgeCasesTest, SingleElement)
{
    TestMap map;
    map.insert(42, "answer");

    EXPECT_FALSE(map.empty());
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[42], "answer");
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, NegativeKeys)
{
    TestMap map;
    map.insert(-5, "negative_five");
    map.insert(0, "zero");
    map.insert(5, "positive_five");

    EXPECT_EQ(map[-5], "negative_five");
    EXPECT_EQ(map[0], "zero");
    EXPECT_EQ(map[5], "positive_five");

    EXPECT_EQ(map.begin()->first, -5);
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, LargeNumbers)
{
    TestMap map;
    int large = 1000000;
    map.insert(large, "large");
    map.insert(large - 1, "smaller");

    EXPECT_EQ(map[large], "large");
    EXPECT_EQ(map[large - 1], "smaller");
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, EmptyStrings)
{
    TestMap map;
    map.insert(1, "");
    map.insert(2, "non-empty");

    EXPECT_EQ(map[1], "");
    EXPECT_EQ(map[2], "non-empty");
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, LargeMapPerformance)
{
    TestMap map(10000);

    for (int i = 5000; i >= 0; --i)
    {
        map.insert(i, "value_" + std::to_string(i));
    }

    for (int i = 5001; i < 10000; ++i)
    {
        map.insert(i, "value_" + std::to_string(i));
    }

    EXPECT_EQ(map.size(), 10000);

    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(),
        [](const auto& a, const auto& b)
        {
            return a.first < b.first;
        }));

    for (int i : {0, 2500, 5000, 7500, 9999})
    {
        auto ptr = map.find(i);
        EXPECT_NE(ptr, nullptr);
        EXPECT_EQ(ptr->first, i);
    }
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, BuildEmptyMap)
{
    TestMap map;
    map.build_start();
    map.build_end();

    EXPECT_TRUE(map.empty());
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, MultipleBuilds)
{
    TestMap map;

    map.build_start();
    map.build_add(1, "one");
    map.build_add(2, "two");
    map.build_end();

    EXPECT_EQ(map.size(), 2);

    map.clear();
    map.build_start();
    map.build_add(5, "five");
    map.build_add(3, "three");
    map.build_end();

    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map[3].c_str()[0], 't');
}

// ===================================================================
// PERFORMANCE CHARACTERISTIC TESTS
// ===================================================================

class FlatKeyOrderMapPerformanceTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapPerformanceTest, LookupInLargeMapVsFill)
{
    TestMap map;
    const int size = 1000;

    map.build_start();

    for (int i = 0; i < size; ++i)
    {
        map.build_add(i, i * 2);
    }

    map.build_end();

    EXPECT_EQ(map.size(), size);

    for (int i : {0, size / 2, size - 1})
    {
        auto ptr = map.find(i);
        EXPECT_NE(ptr, nullptr);
        EXPECT_EQ(ptr->second, i * 2);
    }
}

TEST_F(FlatKeyOrderMapPerformanceTest, IterationThroughAllElements)
{
    TestMap map;
    const int size = 100;

    for (int i = 0; i < size; ++i)
    {
        map.insert(i, i * 10);
    }

    int sum = 0;

    for (auto& pair : map)
    {
        sum += pair.second;
    }

    int expected = 0;

    for (int i = 0; i < size; ++i)
    {
        expected += i * 10;
    }

    EXPECT_EQ(sum, expected);
}
