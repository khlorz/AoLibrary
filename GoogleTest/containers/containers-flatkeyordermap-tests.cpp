/********************************************************************
* FlatKeyOrderMap tests: all container operations
********************************************************************/


#include "pch.h"
#include "config.h"
#if AOL_TEST_CONTAINERS_FLATKEYORDERMAP

#include "aol/key_ordered_map.h"
#include "aol/utilities.h"

#include <limits>


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
    EXPECT_EQ(map.find(4), map.end());
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
    EXPECT_EQ(const_map.at_ref(5), "five");
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
    EXPECT_NE(ptr, map.end());
    EXPECT_EQ(ptr->first, 5);
    EXPECT_EQ(ptr->second, "five");
}

TEST_F(FlatKeyOrderMapFindTest, FindNonExistentKey)
{
    TestMap map;
    map.insert(5, "five");

    auto ptr = map.find(999);
    EXPECT_EQ(ptr, map.end());
}

TEST_F(FlatKeyOrderMapFindTest, FindEmptyMap)
{
    TestMap map;
    auto ptr = map.find(5);

    EXPECT_EQ(ptr, map.end());
}

TEST_F(FlatKeyOrderMapFindTest, FindConstMap)
{
    TestMap map;
    map.insert(7, "seven");

    const TestMap& const_map = map;
    auto ptr = const_map.find(7);

    EXPECT_NE(ptr, const_map.end());
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
        EXPECT_NE(ptr, map.end());
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
        EXPECT_NE(ptr, map.end());
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
        EXPECT_NE(ptr, map.end());
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

// ===================================================================
// COPY SEMANTICS TESTS
// ===================================================================

class FlatKeyOrderMapCopyTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapCopyTest, CopyConstructEmpty)
{
    TestMap map;
    TestMap copied(map);

    EXPECT_TRUE(copied.empty());
}

TEST_F(FlatKeyOrderMapCopyTest, CopyConstructFilled)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(3, "three");
    map.insert(2, "two");

    TestMap copied(map);

    EXPECT_EQ(copied.size(), 3);
    EXPECT_EQ(copied[1], "one");
    EXPECT_EQ(copied[2], "two");
    EXPECT_EQ(copied[3], "three");
}

TEST_F(FlatKeyOrderMapCopyTest, CopyIsIndependent)
{
    TestMap map;
    map.insert(1, "one");

    TestMap copied(map);
    map.insert(2, "two");

    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(copied.size(), 1);
    EXPECT_EQ(map[2], "two");
    EXPECT_FALSE(copied.contains(2));
}

TEST_F(FlatKeyOrderMapCopyTest, CopyAssignFilled)
{
    TestMap map;
    map.insert(5, "five");
    map.insert(1, "one");

    TestMap assigned;
    assigned = map;

    EXPECT_EQ(assigned.size(), 2);
    EXPECT_EQ(assigned[1], "one");
    EXPECT_EQ(assigned[5], "five");
}

TEST_F(FlatKeyOrderMapCopyTest, CopyAssignSelf)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    map = map;

    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map[1], "one");
}

TEST_F(FlatKeyOrderMapCopyTest, CopyAssignOverwritesOldData)
{
    TestMap map;
    map.insert(1, "one");

    TestMap assigned;
    assigned.insert(99, "old");
    assigned = map;

    EXPECT_EQ(assigned.size(), 1);
    EXPECT_EQ(assigned[1], "one");
    EXPECT_FALSE(assigned.contains(99));
}

// ===================================================================
// MOVE SEMANTICS TESTS
// ===================================================================

class FlatKeyOrderMapMoveTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapMoveTest, MoveConstructEmpty)
{
    TestMap map;
    TestMap moved(std::move(map));

    EXPECT_TRUE(moved.empty());
}

TEST_F(FlatKeyOrderMapMoveTest, MoveConstructFilled)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    TestMap moved(std::move(map));

    EXPECT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[1], "one");
    EXPECT_EQ(moved[2], "two");
}

TEST_F(FlatKeyOrderMapMoveTest, MoveAssignFilled)
{
    TestMap map;
    map.insert(10, "ten");

    TestMap assigned;
    assigned = std::move(map);

    EXPECT_EQ(assigned.size(), 1);
    EXPECT_EQ(assigned[10], "ten");
}

TEST_F(FlatKeyOrderMapMoveTest, MoveAssignSelf)
{
    TestMap map;
    map.insert(1, "one");

    map = std::move(map);

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[1], "one");
}

// ===================================================================
// CONSTRUCTION FROM CONTAINER TESTS
// ===================================================================

class FlatKeyOrderMapContainerCtorTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;
    using PairType = AoL::FlatKeyOrderMapPair<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapContainerCtorTest, FromVectorCopy)
{
    typename TestMap::container_type data{ PairType{3, "three"}, PairType{1, "one"}, PairType{2, "two"} };
    TestMap map(data);

    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.begin()->first, 1);
    EXPECT_EQ((map.begin() + 1)->first, 2);
    EXPECT_EQ((map.begin() + 2)->first, 3);
}

TEST_F(FlatKeyOrderMapContainerCtorTest, FromVectorMove)
{
    typename TestMap::container_type data{ PairType{5, "five"}, PairType{4, "four"} };
    TestMap map(std::move(data));

    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.begin()->first, 4);
    EXPECT_EQ((map.begin() + 1)->first, 5);
}

// ===================================================================
// CONST CORRECTNESS TESTS
// ===================================================================

class FlatKeyOrderMapConstTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapConstTest, ConstEmpty)
{
    const TestMap map;
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.find(1), map.end());
    EXPECT_FALSE(map.contains(1));
}

TEST_F(FlatKeyOrderMapConstTest, ConstAccess)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    const TestMap& const_map = map;

    EXPECT_FALSE(const_map.empty());
    EXPECT_EQ(const_map.size(), 2);
    EXPECT_TRUE(const_map.contains(1));
    EXPECT_FALSE(const_map.contains(3));

    auto it = const_map.find(1);
    EXPECT_NE(it, const_map.end());
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, "one");

    it = const_map.find(3);
    EXPECT_EQ(it, const_map.end());
}

TEST_F(FlatKeyOrderMapConstTest, ConstAtRef)
{
    TestMap map;
    map.insert(5, "five");

    const TestMap& const_map = map;
    EXPECT_EQ(const_map.at_ref(5), "five");
}

TEST_F(FlatKeyOrderMapConstTest, ConstAtPtr)
{
    TestMap map;
    map.insert(7, "seven");

    const TestMap& const_map = map;
    const std::string* ptr = const_map.at_ptr(7);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, "seven");
}

TEST_F(FlatKeyOrderMapConstTest, ConstAtPtrNonExistent)
{
    TestMap map;
    map.insert(1, "one");

    const TestMap& const_map = map;
    EXPECT_EQ(const_map.at_ptr(999), nullptr);
}

TEST_F(FlatKeyOrderMapConstTest, ConstAtPtrEmpty)
{
    const TestMap map;
    EXPECT_EQ(map.at_ptr(1), nullptr);
}

TEST_F(FlatKeyOrderMapConstTest, ConstData)
{
    TestMap map;
    map.insert(3, "three");

    const TestMap& const_map = map;
    const auto* data = const_map.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0].first, 3);
}

TEST_F(FlatKeyOrderMapConstTest, ConstIterators)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    const TestMap& const_map = map;

    auto it = const_map.cbegin();
    EXPECT_EQ(it->first, 1);
    ++it;
    EXPECT_EQ(it->first, 2);
    ++it;
    EXPECT_EQ(it, const_map.cend());

    auto rit = const_map.crbegin();
    EXPECT_EQ(rit->first, 2);
    ++rit;
    EXPECT_EQ(rit->first, 1);
    ++rit;
    EXPECT_EQ(rit, const_map.crend());
}

// ===================================================================
// AT_PTR MODIFICATION TESTS
// ===================================================================

class FlatKeyOrderMapAtPtrModifyTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapAtPtrModifyTest, ModifyThroughPointer)
{
    TestMap map;
    map.insert(1, "one");

    auto ptr = map.at_ptr(1);
    ASSERT_NE(ptr, nullptr);
    *ptr = "modified";

    EXPECT_EQ(map[1], "modified");
}

// ===================================================================
// OPERATOR[] DEFAULT INSERTION TESTS
// ===================================================================

class FlatKeyOrderMapBracketTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapBracketTest, CreatesEntryOnNonExistentKey)
{
    TestMap map;
    std::string& ref = map[42];

    EXPECT_EQ(map.size(), 1);
    EXPECT_TRUE(ref.empty());
}

TEST_F(FlatKeyOrderMapBracketTest, ReturnsExistingEntry)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";

    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
}

TEST_F(FlatKeyOrderMapBracketTest, ChainedModification)
{
    TestMap map;
    map[1] = "first";
    map[1] = "second";

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[1], "second");
}

// ===================================================================
// KEY TYPE VARIATIONS TESTS
// ===================================================================

class FlatKeyOrderMapStringKeyTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<std::string, int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapStringKeyTest, InsertAndAccess)
{
    TestMap map;
    map.insert("apple", 5);
    map.insert("banana", 3);
    map.insert("cherry", 8);

    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map["apple"], 5);
    EXPECT_EQ(map["banana"], 3);
    EXPECT_EQ(map["cherry"], 8);
}

TEST_F(FlatKeyOrderMapStringKeyTest, KeyOrderAlphabetical)
{
    TestMap map;
    map.insert("zebra", 1);
    map.insert("apple", 2);
    map.insert("mango", 3);

    auto it = map.begin();
    EXPECT_EQ(it->first, "apple");
    ++it;
    EXPECT_EQ(it->first, "mango");
    ++it;
    EXPECT_EQ(it->first, "zebra");
}

TEST_F(FlatKeyOrderMapStringKeyTest, FindAndContains)
{
    TestMap map;
    map.insert("key", 42);

    EXPECT_TRUE(map.contains("key"));
    EXPECT_FALSE(map.contains("nonexistent"));

    auto it = map.find("key");
    EXPECT_NE(it, map.end());
    EXPECT_EQ(it->second, 42);

    EXPECT_EQ(map.find("other"), map.end());
}

TEST_F(FlatKeyOrderMapStringKeyTest, BuildWithStringKeys)
{
    TestMap map;
    map.build_start();
    map.build_add("c", 3);
    map.build_add("a", 1);
    map.build_add("b", 2);
    map.build_end();

    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map["a"], 1);
    EXPECT_EQ(map["b"], 2);
    EXPECT_EQ(map["c"], 3);
}

TEST_F(FlatKeyOrderMapStringKeyTest, EmptyStringKey)
{
    TestMap map;
    map.insert("", 0);
    map.insert("nonempty", 1);

    EXPECT_EQ(map[""], 0);
    EXPECT_EQ(map["nonempty"], 1);
}

// ===================================================================
// ITERATOR CATEGORY TESTS
// ===================================================================

class FlatKeyOrderMapIteratorCategoryTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapIteratorCategoryTest, BeginEndRange)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");
    map.insert(3, "three");

    int count = 0;
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        ++count;
    }
    EXPECT_EQ(count, 3);
}

TEST_F(FlatKeyOrderMapIteratorCategoryTest, CBeginCEnd)
{
    TestMap map;
    map.insert(10, "ten");

    const TestMap& const_map = map;
    auto it = const_map.cbegin();
    EXPECT_EQ(it->first, 10);
    EXPECT_NE(it, const_map.cend());
    ++it;
    EXPECT_EQ(it, const_map.cend());
}

TEST_F(FlatKeyOrderMapIteratorCategoryTest, CRBeginCREnd)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    const TestMap& const_map = map;
    auto it = const_map.crbegin();
    EXPECT_EQ(it->first, 2);
    ++it;
    EXPECT_EQ(it->first, 1);
    ++it;
    EXPECT_EQ(it, const_map.crend());
}

TEST_F(FlatKeyOrderMapIteratorCategoryTest, IteratorDifference)
{
    TestMap map;
    for (int i = 0; i < 5; ++i)
    {
        map.insert(i, "val");
    }

    auto diff = map.end() - map.begin();
    EXPECT_EQ(diff, 5);
}

// ===================================================================
// BOUNDARY VALUE TESTS
// ===================================================================

class FlatKeyOrderMapBoundaryTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapBoundaryTest, IntMinMaxKeys)
{
    TestMap map;
    map.insert(INT_MIN, "min");
    map.insert(0, "zero");
    map.insert(INT_MAX, "max");

    EXPECT_EQ(map[INT_MIN], "min");
    EXPECT_EQ(map[0], "zero");
    EXPECT_EQ(map[INT_MAX], "max");

    EXPECT_EQ(map.begin()->first, INT_MIN);
    EXPECT_EQ(map.rbegin()->first, INT_MAX);
}

TEST_F(FlatKeyOrderMapBoundaryTest, MixedNegativeAndPositive)
{
    TestMap map;
    map.insert(-100, "neg");
    map.insert(100, "pos");
    map.insert(-1, "neg_small");
    map.insert(1, "pos_small");

    EXPECT_EQ(map.begin()->first, -100);
    EXPECT_EQ(map.rbegin()->first, 100);
}

TEST_F(FlatKeyOrderMapBoundaryTest, LargeNumberOfElements)
{
    TestMap map;
    const int n = 10000;

    for (int i = 0; i < n; ++i)
    {
        map.insert(i, "v");
    }

    EXPECT_EQ(map.size(), n);

    for (int i = 0; i < n; ++i)
    {
        EXPECT_TRUE(map.contains(i));
    }

    EXPECT_FALSE(map.contains(n));
}

// ===================================================================
// EMPTY MAP EDGE CASE TESTS
// ===================================================================

class FlatKeyOrderMapEmptyTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapEmptyTest, DataOnEmptyMap)
{
    TestMap map;
    auto* data = map.data();
    (void)data;
    EXPECT_TRUE(map.empty());
}

TEST_F(FlatKeyOrderMapEmptyTest, ConstDataOnEmptyMap)
{
    const TestMap map;
    const auto* data = map.data();
    (void)data;
    EXPECT_TRUE(map.empty());
}

TEST_F(FlatKeyOrderMapEmptyTest, AtPtrOnEmptyMap)
{
    TestMap map;
    EXPECT_EQ(map.at_ptr(1), nullptr);
}

TEST_F(FlatKeyOrderMapEmptyTest, ClearEmptyMapIsNoOp)
{
    TestMap map;
    map.clear();
    EXPECT_TRUE(map.empty());
}

TEST_F(FlatKeyOrderMapEmptyTest, ClearThenBuild)
{
    TestMap map;
    map.insert(1, "one");
    map.clear();

    EXPECT_TRUE(map.empty());

    map.build_start();
    map.build_add(2, "two");
    map.build_end();

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[2], "two");
}

TEST_F(FlatKeyOrderMapEmptyTest, MultipleClearCycles)
{
    TestMap map;

    for (int cycle = 0; cycle < 10; ++cycle)
    {
        for (int i = 0; i < cycle; ++i)
        {
            map.insert(i, "val");
        }

        EXPECT_EQ(map.size(), cycle);
        map.clear();
        EXPECT_TRUE(map.empty());
    }
}

// ===================================================================
// MIXED OPERATION SEQUENCE TESTS
// ===================================================================

class FlatKeyOrderMapMixedTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapMixedTest, InsertAndBuildSequence)
{
    TestMap map;
    map.insert(5, 50);

    map.build_start();
    map.build_add(1, 10);
    map.build_add(3, 30);
    map.build_end();

    map.insert(2, 20);

    EXPECT_EQ(map.size(), 4);
    EXPECT_EQ(map[1], 10);
    EXPECT_EQ(map[2], 20);
    EXPECT_EQ(map[3], 30);
    EXPECT_EQ(map[5], 50);
}

TEST_F(FlatKeyOrderMapMixedTest, BuildClearBuild)
{
    TestMap map;
    map.build_start();
    map.build_add(3, 30);
    map.build_add(1, 10);
    map.build_end();

    map.clear();

    map.build_start();
    map.build_add(7, 70);
    map.build_add(5, 50);
    map.build_add(6, 60);
    map.build_end();

    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map[5], 50);
    EXPECT_EQ(map[6], 60);
    EXPECT_EQ(map[7], 70);
}

TEST_F(FlatKeyOrderMapMixedTest, CopyOfBuiltMap)
{
    TestMap map;
    map.build_start();
    map.build_add(2, 20);
    map.build_add(1, 10);
    map.build_end();

    TestMap copied(map);
    EXPECT_EQ(copied.size(), 2);
    EXPECT_EQ(copied[1], 10);
    EXPECT_EQ(copied[2], 20);
}

TEST_F(FlatKeyOrderMapMixedTest, MoveOfInsertedMap)
{
    TestMap map;
    map.insert(1, 10);
    map.insert(2, 20);

    TestMap moved(std::move(map));
    EXPECT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[1], 10);
}

TEST_F(FlatKeyOrderMapMixedTest, LargeBuildWithContiguousKeys)
{
    TestMap map(20000);
    map.build_start();

    for (int i = 19999; i >= 0; --i)
    {
        map.build_add(i, i * 2);
    }

    map.build_end();

    EXPECT_EQ(map.size(), 20000);

    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(),
        [](const auto& a, const auto& b)
        {
            return a.first < b.first;
        }));

    for (int i = 0; i < 20000; i += 500)
    {
        EXPECT_EQ(map[i], i * 2);
    }
}

// ===================================================================
// FORWARDING REFERENCE TESTS — STRING KEY
// ===================================================================
// Verifies that every InKey&& / InValue&& parameter compiles and works
// correctly with lvalues, const lvalues, rvalues, and string literals.

class FlatKeyOrderMapFwdStringKeyTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<std::string, int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertLvalueString)
{
    TestMap map;
    std::string key = "hello";
    map.insert(key, 1);

    EXPECT_EQ(map["hello"], 1);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertConstLvalueString)
{
    TestMap map;
    const std::string key = "world";
    map.insert(key, 2);

    EXPECT_EQ(map["world"], 2);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertRvalueString)
{
    TestMap map;
    map.insert(std::string("temp"), 3);

    EXPECT_EQ(map["temp"], 3);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertStringLiteral)
{
    TestMap map;
    map.insert("literal", 4);

    EXPECT_EQ(map["literal"], 4);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertConstCharPointer)
{
    TestMap map;
    const char* ptr = "cstring";
    map.insert(ptr, 5);

    EXPECT_EQ(map["cstring"], 5);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, BracketLvalueString)
{
    TestMap map;
    std::string key = "alpha";
    map[key] = 10;

    EXPECT_EQ(map["alpha"], 10);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, BracketRvalueString)
{
    TestMap map;
    map[std::string("beta")] = 20;

    EXPECT_EQ(map["beta"], 20);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, BracketStringLiteral)
{
    TestMap map;
    map["gamma"] = 30;

    EXPECT_EQ(map["gamma"], 30);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtRefLvalueString)
{
    TestMap map;
    map.insert("key", 42);
    std::string key = "key";
    EXPECT_EQ(map.at_ref(key), 42);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtRefRvalueString)
{
    TestMap map;
    map.insert("key", 42);
    EXPECT_EQ(map.at_ref(std::string("key")), 42);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtRefStringLiteral)
{
    TestMap map;
    map.insert("key", 42);
    EXPECT_EQ(map.at_ref("key"), 42);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtPtrLvalueString)
{
    TestMap map;
    map.insert("key", 42);
    std::string key = "key";
    int* ptr = map.at_ptr(key);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtPtrRvalueString)
{
    TestMap map;
    map.insert("key", 42);
    int* ptr = map.at_ptr(std::string("key"));
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtPtrStringLiteral)
{
    TestMap map;
    map.insert("key", 42);
    int* ptr = map.at_ptr("key");
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, FindLvalueString)
{
    TestMap map;
    map.insert("target", 99);
    std::string key = "target";
    EXPECT_NE(map.find(key), map.end());
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, FindRvalueString)
{
    TestMap map;
    map.insert("target", 99);
    EXPECT_NE(map.find(std::string("target")), map.end());
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, FindStringLiteral)
{
    TestMap map;
    map.insert("target", 99);
    EXPECT_NE(map.find("target"), map.end());
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, ContainsLvalueString)
{
    TestMap map;
    map.insert("present", 1);
    std::string key = "present";
    EXPECT_TRUE(map.contains(key));
    std::string missing = "absent";
    EXPECT_FALSE(map.contains(missing));
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, ContainsRvalueString)
{
    TestMap map;
    map.insert("present", 1);
    EXPECT_TRUE(map.contains(std::string("present")));
    EXPECT_FALSE(map.contains(std::string("absent")));
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, ContainsStringLiteral)
{
    TestMap map;
    map.insert("present", 1);
    EXPECT_TRUE(map.contains("present"));
    EXPECT_FALSE(map.contains("absent"));
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, BuildAddLvalueString)
{
    TestMap map;
    std::string k1 = "b", k2 = "a";
    map.build_start();
    map.build_add(k1, 2);
    map.build_add(k2, 1);
    map.build_end();

    EXPECT_EQ(map.begin()->first, "a");
    EXPECT_EQ(map["b"], 2);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, BuildAddRvalueString)
{
    TestMap map;
    map.build_start();
    map.build_add(std::string("z"), 26);
    map.build_add(std::string("a"), 1);
    map.build_end();

    EXPECT_EQ(map.begin()->first, "a");
    EXPECT_EQ(map["z"], 26);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, BuildAddStringLiteral)
{
    TestMap map;
    map.build_start();
    map.build_add("second", 2);
    map.build_add("first", 1);
    map.build_end();

    EXPECT_EQ(map.begin()->first, "first");
    EXPECT_EQ(map["second"], 2);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, ConstFindLvalueString)
{
    TestMap map;
    map.insert("key", 7);
    const TestMap& cmap = map;
    std::string k = "key";
    EXPECT_NE(cmap.find(k), cmap.end());
    EXPECT_EQ(cmap.find(std::string("nonexistent")), cmap.end());
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, ConstAtRefStringLiteral)
{
    TestMap map;
    map.insert("val", 42);
    const TestMap& cmap = map;
    EXPECT_EQ(cmap.at_ref("val"), 42);
}

TEST_F(FlatKeyOrderMapFwdStringKeyTest, ConstAtPtrRvalueString)
{
    TestMap map;
    map.insert("val", 42);
    const TestMap& cmap = map;
    const int* ptr = cmap.at_ptr(std::string("val"));
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

// ===================================================================
// FORWARDING REFERENCE TESTS — STRING VALUE
// ===================================================================

class FlatKeyOrderMapFwdStringValueTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertLvalueString)
{
    TestMap map;
    std::string val = "hello";
    map.insert(1, val);

    EXPECT_EQ(map[1], "hello");
}

TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertConstLvalueString)
{
    TestMap map;
    const std::string val = "world";
    map.insert(2, val);

    EXPECT_EQ(map[2], "world");
}

TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertRvalueString)
{
    TestMap map;
    map.insert(3, std::string("temp"));

    EXPECT_EQ(map[3], "temp");
}

TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertStringLiteral)
{
    TestMap map;
    map.insert(4, "literal");

    EXPECT_EQ(map[4], "literal");
}

TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertConstCharPointer)
{
    TestMap map;
    const char* ptr = "cstring";
    map.insert(5, ptr);

    EXPECT_EQ(map[5], "cstring");
}

TEST_F(FlatKeyOrderMapFwdStringValueTest, BuildAddLvalueString)
{
    TestMap map;
    std::string v1 = "two", v2 = "one";
    map.build_start();
    map.build_add(2, v1);
    map.build_add(1, v2);
    map.build_end();

    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
}

TEST_F(FlatKeyOrderMapFwdStringValueTest, BuildAddRvalueString)
{
    TestMap map;
    map.build_start();
    map.build_add(1, std::string("first"));
    map.build_add(2, std::string("second"));
    map.build_end();

    EXPECT_EQ(map[1], "first");
    EXPECT_EQ(map[2], "second");
}

TEST_F(FlatKeyOrderMapFwdStringValueTest, BuildAddStringLiteral)
{
    TestMap map;
    map.build_start();
    map.build_add(3, "three");
    map.build_add(1, "one");
    map.build_end();

    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[3], "three");
}

// ===================================================================
// FORWARDING REFERENCE TESTS — BOTH STRING KEY AND VALUE
// ===================================================================

class FlatKeyOrderMapFwdStringBothTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<std::string, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLvalueLvalue)
{
    TestMap map;
    std::string k = "key", v = "val";
    map.insert(k, v);
    EXPECT_EQ(map["key"], "val");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLvalueRvalue)
{
    TestMap map;
    std::string k = "key";
    map.insert(k, std::string("val"));
    EXPECT_EQ(map["key"], "val");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertRvalueLvalue)
{
    TestMap map;
    std::string v = "val";
    map.insert(std::string("key"), v);
    EXPECT_EQ(map["key"], "val");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertRvalueRvalue)
{
    TestMap map;
    map.insert(std::string("key"), std::string("val"));
    EXPECT_EQ(map["key"], "val");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLiteralLiteral)
{
    TestMap map;
    map.insert("key", "val");
    EXPECT_EQ(map["key"], "val");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLiteralLvalue)
{
    TestMap map;
    std::string v = "val";
    map.insert("key", v);
    EXPECT_EQ(map["key"], "val");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLiteralRvalue)
{
    TestMap map;
    map.insert("key", std::string("val"));
    EXPECT_EQ(map["key"], "val");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, BuildAddAllCombinations)
{
    TestMap map;
    std::string k1 = "b", v1 = "B";
    std::string v2 = "A";

    map.build_start();
    map.build_add(k1, v1);
    map.build_add("a", std::move(v2));
    map.build_add(std::string("c"), "C");
    map.build_end();

    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.begin()->first, "a");
    EXPECT_EQ(map["b"], "B");
    EXPECT_EQ(map["c"], "C");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, FindAndContainsAllCategories)
{
    TestMap map;
    map.insert("key", "val");

    std::string lkey = "key";
    EXPECT_NE(map.find(lkey), map.end());

    EXPECT_NE(map.find(std::string("key")), map.end());
    EXPECT_NE(map.find("key"), map.end());

    EXPECT_TRUE(map.contains(lkey));
    EXPECT_TRUE(map.contains(std::string("key")));
    EXPECT_TRUE(map.contains("key"));

    std::string lmissing = "no";
    EXPECT_FALSE(map.contains(lmissing));
    EXPECT_FALSE(map.contains(std::string("no")));
    EXPECT_FALSE(map.contains("no"));
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, AtRefAllCategories)
{
    TestMap map;
    map.insert("key", "val");

    std::string lk = "key";
    EXPECT_EQ(map.at_ref(lk), "val");
    EXPECT_EQ(map.at_ref(std::string("key")), "val");
    EXPECT_EQ(map.at_ref("key"), "val");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, ConstAtPtrAllCategories)
{
    TestMap map;
    map.insert("key", "val");

    const TestMap& cmap = map;
    std::string lk = "key";

    const std::string* p1 = cmap.at_ptr(lk);
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(*p1, "val");

    const std::string* p2 = cmap.at_ptr(std::string("key"));
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(*p2, "val");

    const std::string* p3 = cmap.at_ptr("key");
    ASSERT_NE(p3, nullptr);
    EXPECT_EQ(*p3, "val");
}

TEST_F(FlatKeyOrderMapFwdStringBothTest, MixedInsertAndBuildSequence)
{
    TestMap map;

    std::string k1 = "c";
    map.insert(k1, "C");

    map.build_start();
    map.build_add("a", std::string("A"));
    map.build_add(std::string("b"), "B");
    map.build_end();

    map.insert(std::string("d"), std::string("D"));
    map.insert("e", "E");

    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(map["a"], "A");
    EXPECT_EQ(map["b"], "B");
    EXPECT_EQ(map["c"], "C");
    EXPECT_EQ(map["d"], "D");
    EXPECT_EQ(map["e"], "E");
}

// ===================================================================
// ALLOCATION TRACKING TESTS
// ===================================================================
// Tracks how many times a key type is constructed (direct/copy/move)
// during each operation. Verifies that forwarding references don't
// cause unnecessary extra constructions.
// Expected rule: every insert/build_add/operator[] for a new key
// constructs the stored key exactly ONCE; all other operations
// (find, contains, at_ref, at_ptr, operator[] for existing key)
// construct ZERO keys.

struct TrackedKey
{
    static inline int direct_count = 0;
    static inline int copy_count = 0;
    static inline int move_count = 0;

    std::string value;

    TrackedKey() : value() { direct_count++; }
    TrackedKey(const char* s) : value(s) { direct_count++; }
    TrackedKey(const std::string& s) : value(s) { direct_count++; }
    TrackedKey(const TrackedKey& o) : value(o.value) { copy_count++; }
    TrackedKey(TrackedKey&& o) noexcept : value(std::move(o.value)) { move_count++; o.value.clear(); }
    TrackedKey& operator=(const TrackedKey&) = default;
    TrackedKey& operator=(TrackedKey&&) noexcept = default;

    bool operator==(const TrackedKey& o) const { return value == o.value; }
    bool operator!=(const TrackedKey& o) const { return value != o.value; }
    bool operator<(const TrackedKey& o) const { return value < o.value; }
    auto operator <=> (const TrackedKey& o) const { return this->value <=> o.value; }

    bool operator==(const char* s) const { return value == s; }
    bool operator!=(const char* s) const { return value != s; }
    bool operator<(const char* s) const { return value < s; }
    auto operator <=> (const char* s) const { return this->value <=> s; }

    bool operator==(const std::string& s) const { return value == s; }
    bool operator!=(const std::string& s) const { return value != s; }
    bool operator<(const std::string& s) const { return value < s; }
    auto operator <=> (const std::string& s) const { return this->value <=> s; }

    static void ResetCounts()
    {
        direct_count = 0;
        copy_count = 0;
        move_count = 0;
    }
};

static void ExpectCounts(int direct, int copy, int move,
    int line = __LINE__)
{
    EXPECT_EQ(TrackedKey::direct_count, direct)
        << "  direct_count mismatch at approx line " << line;
    EXPECT_EQ(TrackedKey::copy_count, copy)
        << "  copy_count mismatch at approx line " << line;
    EXPECT_EQ(TrackedKey::move_count, move)
        << "  move_count mismatch at approx line " << line;
}

class FlatKeyOrderMapTrackedTest : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<TrackedKey, int>;

    void SetUp() override
    {
        TrackedKey::ResetCounts();
    }
};

TEST_F(FlatKeyOrderMapTrackedTest, InsertTrackedKeyLvalue)
{
    TestMap map;
    TrackedKey key("hello");
    EXPECT_EQ(map.size(), 0);
    TrackedKey::ResetCounts();

    map.insert(key, 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(0, 1, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, InsertTrackedKeyRvalue)
{
    TestMap map;
    TrackedKey key("hello");
    TrackedKey::ResetCounts();

    map.insert(std::move(key), 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(0, 0, 1, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, InsertStringLiteral)
{
    TestMap map;
    TrackedKey::ResetCounts();

    map.insert("hello", 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(1, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, InsertStdStringRvalue)
{
    TestMap map;
    TrackedKey::ResetCounts();

    map.insert(std::string("hello"), 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(1, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, InsertStdStringLvalue)
{
    TestMap map;
    std::string s("hello");
    TrackedKey::ResetCounts();

    map.insert(s, 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(1, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, InsertConstStdStringLvalue)
{
    TestMap map;
    const std::string s("hello");
    TrackedKey::ResetCounts();

    map.insert(s, 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(1, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, FindLvalueTrackedKeyZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey search("key");
    TrackedKey::ResetCounts();

    auto it = map.find(search);
    EXPECT_NE(it, map.end());
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, FindRvalueTrackedKeyZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey search("key");
    TrackedKey::ResetCounts();

    auto it = map.find(std::move(search));
    EXPECT_NE(it, map.end());
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, FindStringLiteralZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey::ResetCounts();

    auto it = map.find("key");
    EXPECT_NE(it, map.end());
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, FindStdStringRvalueZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey::ResetCounts();

    auto it = map.find(std::string("key"));
    EXPECT_NE(it, map.end());
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, ContainsAllCategoriesZeroConstructions)
{
    TestMap map;
    map.insert("present", 1);
    TrackedKey tracked_key("present");
    TrackedKey tracked_missing("absent");
    TrackedKey::ResetCounts();

    EXPECT_TRUE(map.contains(tracked_key));
    EXPECT_FALSE(map.contains(tracked_missing));
    EXPECT_TRUE(map.contains("present"));
    EXPECT_FALSE(map.contains("absent"));
    EXPECT_TRUE(map.contains(std::string("present")));
    EXPECT_FALSE(map.contains(std::string("absent")));
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, AtRefAllCategoriesZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey tk("key");
    TrackedKey::ResetCounts();

    EXPECT_EQ(map.at_ref(tk), 42);
    EXPECT_EQ(map.at_ref("key"), 42);
    EXPECT_EQ(map.at_ref(std::string("key")), 42);
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, AtPtrAllCategoriesZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey tk("key");
    TrackedKey::ResetCounts();

    EXPECT_NE(map.at_ptr(tk), nullptr);
    EXPECT_NE(map.at_ptr("key"), nullptr);
    EXPECT_NE(map.at_ptr(std::string("key")), nullptr);
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, ConstFindRvalueTrackedKeyZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    const TestMap& cmap = map;
    TrackedKey search("key");
    TrackedKey::ResetCounts();

    EXPECT_NE(cmap.find(std::move(search)), cmap.end());
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, OperatorBracketExistingZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey::ResetCounts();

    EXPECT_EQ(map["key"], 42);
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, OperatorBracketNewKeyOneConstruction)
{
    TestMap map;
    TrackedKey::ResetCounts();

    map["new_key"] = 99;

    EXPECT_EQ(map["new_key"], 99);
    ExpectCounts(1, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, BuildAddLiteralOneConstructionEach)
{
    TestMap map;
    map.build_start();
    TrackedKey::ResetCounts();

    map.build_add("a", 1);
    map.build_add("b", 2);

    // 2 direct construction
    // 1 move construction due to re-allocation
    ExpectCounts(2, 0, 1, __LINE__);

    // Build here to avoid counting the move operations while sorting
    map.build_end();
}

TEST_F(FlatKeyOrderMapTrackedTest, BuildAddTrackedLvalueOneCopy)
{
    TestMap map;
    map.build_start();
    TrackedKey ka("a"), kb("b");
    TrackedKey::ResetCounts();

    map.build_add(ka, 1);
    map.build_add(kb, 2);

    // 2 copy construction
    // 1 move construction due to re-allocation
    ExpectCounts(0, 2, 1, __LINE__);

    // Build here to avoid counting the move operations while sorting
    map.build_end();
}

TEST_F(FlatKeyOrderMapTrackedTest, BuildAddTrackedRvalueOneMove)
{
    TestMap map;
    map.build_start();
    TrackedKey kx("x"), ky("y");
    TrackedKey::ResetCounts();

    map.build_add(std::move(kx), 10);
    map.build_add(std::move(ky), 20);

    // 2 move construction
    // 1 move construction due to re-allocation
    ExpectCounts(0, 0, 3, __LINE__);

    // Build here to avoid counting the move operations while sorting
    map.build_end();
}

TEST_F(FlatKeyOrderMapTrackedTest, MultipleInsertThenFindNoExtraConstructions)
{
    TestMap map;
    TrackedKey::ResetCounts();

    map.insert("first", 1);
    ExpectCounts(1, 0, 0, __LINE__);

    map.insert("second", 2);
    // 1 move construction due to re-allocation
    ExpectCounts(2, 0, 1, __LINE__);

    map.insert("third", 3);
    // 2 additional move construction due to re-allocation
    ExpectCounts(3, 0, 3, __LINE__);

    EXPECT_TRUE(map.contains("first"));
    EXPECT_TRUE(map.contains("second"));
    EXPECT_TRUE(map.contains("third"));

    ExpectCounts(3, 0, 3, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, BuildAddThenFindNoExtraConstructions)
{
    TestMap map;
    map.build_start();
    map.build_add("c", 3);
    map.build_add("a", 1);
    map.build_add("b", 2);
    map.build_end();
    TrackedKey::ResetCounts();

    EXPECT_EQ(map.find("a")->second, 1);
    EXPECT_EQ(map.find("b")->second, 2);
    EXPECT_EQ(map.find("c")->second, 3);
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, AtPtrModifyThenReadNoExtraConstructions)
{
    TestMap map;
    map.insert("data", 0);
    TrackedKey::ResetCounts();

    *map.at_ptr("data") = 77;
    EXPECT_EQ(map.find("data")->second, 77);
    ExpectCounts(0, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, ClearThenBuildNewKeys)
{
    TestMap map;
    map.insert("old", 1);
    map.clear();
    TrackedKey::ResetCounts();

    map.build_start();
    map.build_add("new", 2);
    map.build_end();

    ExpectCounts(1, 0, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, CopyMapNoAdditionalKeyConstructions)
{
    TestMap map;
    map.insert("a", 1);
    map.insert("b", 2);
    TrackedKey::ResetCounts();

    TestMap copied(map);

    ExpectCounts(0, 2, 0, __LINE__);
}

TEST_F(FlatKeyOrderMapTrackedTest, MoveMapOneAdditionalKeyConstruction)
{
    TestMap map;
    map.insert("a", 1);
    TrackedKey::ResetCounts();

    TestMap moved(std::move(map));

    // Could be one move construction.
    // Moving the continer itself does not trigger this though
    // So 0 move count here
    ExpectCounts(0, 0, 0, __LINE__);
}

#endif // AOL_TEST_CONTAINERS_FLATKEYORDERMAP
