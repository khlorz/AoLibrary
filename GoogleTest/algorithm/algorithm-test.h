#pragma once

/********************************************************************
* Algorithm tests
* - Tests all algorithm implementations: Find and Sort variants
* - Tests edge cases, boundary conditions, and various data types
* - Tests with different comparators and custom types
* - Exhaustive coverage of all find and sort algorithms
********************************************************************/

#include "aol/aol.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <random>
#include <functional>

namespace {

// ===================================================================
// TEST FIXTURES AND UTILITIES
// ===================================================================

// Custom type for testing
struct CustomData {
    int value;
    std::string name;

    CustomData(int v = 0, const std::string& n = "") : value(v), name(n) {}

    bool operator<(const CustomData& other) const {
        return value < other.value;
    }

    bool operator==(const CustomData& other) const {
        return value == other.value && name == other.name;
    }

    bool operator>(const CustomData& other) const {
        return value > other.value;
    }

    bool operator<=(const CustomData& other) const {
        return value <= other.value;
    }

    bool operator>=(const CustomData& other) const {
        return value >= other.value;
    }
};

// Custom comparator for reverse order
struct ReverseComparator {
    template<typename T>
    bool operator()(const T& a, const T& b) const {
        return a > b;
    }
};

// ===================================================================
// FIND BRUTE TESTS
// ===================================================================

class FindBruteTest : public ::testing::Test {
protected:
    using IntVector = std::vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FindBruteTest, FindExistingElement) {
    IntVector vec{ 1, 5, 3, 9, 2, 7 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindBruteTest, FindNonExistingElement) {
    IntVector vec{ 1, 5, 3, 9, 2, 7 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 100);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindBruteTest, FindFirstOccurrence) {
    IntVector vec{ 1, 5, 3, 5, 2, 5 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(std::distance(vec.begin(), it), 1);
}

TEST_F(FindBruteTest, FindInEmptyContainer) {
    IntVector vec;
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 5);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindBruteTest, FindFirstElement) {
    IntVector vec{ 10, 5, 3, 9, 2 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 10);

    EXPECT_EQ(it, vec.begin());
}

TEST_F(FindBruteTest, FindLastElement) {
    IntVector vec{ 1, 5, 3, 9, 42 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 42);

    EXPECT_EQ(*it, 42);
    EXPECT_EQ(std::distance(vec.begin(), it), 4);
}

TEST_F(FindBruteTest, FindInSingleElementContainer) {
    IntVector vec{ 7 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 7);

    EXPECT_EQ(it, vec.begin());
}

TEST_F(FindBruteTest, FindWithCustomType) {
    std::vector<CustomData> vec{
        CustomData(10, "ten"),
        CustomData(20, "twenty"),
        CustomData(30, "thirty")
    };

    auto it = AoL::FindBrute(vec.begin(), vec.end(), CustomData(20, "twenty"));
    EXPECT_NE(it, vec.end());
    EXPECT_EQ(it->value, 20);
}

TEST_F(FindBruteTest, FindNegativeNumbers) {
    IntVector vec{ -5, -1, 0, 1, 5 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), -1);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, -1);
}

TEST_F(FindBruteTest, FindWithArrayPointers) {
    int arr[] = { 1, 2, 3, 4, 5 };
    auto it = AoL::FindBrute(std::begin(arr), std::end(arr), 3);

    EXPECT_NE(it, std::end(arr));
    EXPECT_EQ(*it, 3);
}

// ===================================================================
// FIND LOWER BOUND GENERAL TESTS
// ===================================================================

class FindLowerBoundGeneralTest : public ::testing::Test {
protected:
    using IntVector = std::vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FindLowerBoundGeneralTest, LowerBoundExistingValue) {
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundNonExistingValueBetween) {
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 4);

    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundBeforeAll) {
    IntVector vec{ 5, 10, 15, 20 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 2);

    EXPECT_EQ(it, vec.begin());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundAfterAll) {
    IntVector vec{ 5, 10, 15, 20 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 25);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundEmptyContainer) {
    IntVector vec;
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 5);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundSingleElement) {
    IntVector vec{ 10 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 10);

    EXPECT_EQ(*it, 10);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundDuplicates) {
    IntVector vec{ 1, 5, 5, 5, 9 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 5);

    EXPECT_EQ(*it, 5);
    EXPECT_EQ(std::distance(vec.begin(), it), 1);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundWithCustomComparator) {
    IntVector vec{ 20, 15, 10, 5, 1 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 12, std::greater<int>());

    EXPECT_NE(it, vec.end());
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundLargeRange) {
    IntVector vec;
    for (int i = 0; i < 1000; ++i) {
        vec.push_back(i * 2);
    }

    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 500);
    EXPECT_NE(it, vec.end());
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundNegativeNumbers) {
    IntVector vec{ -100, -50, -10, 0, 10, 50 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), -25);

    EXPECT_EQ(*it, -10);
}

// ===================================================================
// FIND LOWER BOUND BRANCHLESS TESTS
// ===================================================================

class FindLowerBoundBranchlessTest : public ::testing::Test {
protected:
    using IntVector = std::vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundExistingValue) {
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundNonExistingBetween) {
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 4);

    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundBeforeAll) {
    IntVector vec{ 10, 20, 30, 40 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5);

    EXPECT_EQ(it, vec.begin());
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundAfterAll) {
    IntVector vec{ 10, 20, 30, 40 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 50);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundEmptyContainer) {
    IntVector vec;
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundPowerOfTwo) {
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 10);

    EXPECT_EQ(*it, 10);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundNonPowerOfTwo) {
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5);

    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundLargeContainer) {
    IntVector vec;
    for (int i = 0; i < 10000; ++i) {
        vec.push_back(i);
    }

    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5000);
    EXPECT_EQ(*it, 5000);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundCustomComparator) {
    IntVector vec{ 100, 80, 60, 40, 20 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 50, std::greater<int>());

    EXPECT_NE(it, vec.end());
}

// ===================================================================
// FIND LOWER BOUND DEFAULT TESTS
// ===================================================================

class FindLowerBoundDefaultTest : public ::testing::Test {
protected:
    using IntVector = std::vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FindLowerBoundDefaultTest, DefaultLowerBoundBasic) {
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBound(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundDefaultTest, DefaultLowerBoundMissing) {
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBound(vec.begin(), vec.end(), 4);

    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundDefaultTest, DefaultLowerBoundConsistency) {
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    // Default should match Eytzinger
    auto it_default = AoL::FindLowerBound(vec.begin(), vec.end(), 6);
    auto it_eytzinger = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 6);

    EXPECT_EQ(it_default, it_eytzinger);
}

// ===================================================================
// SORT BASIC TESTS
// ===================================================================

class SortBasicTest : public ::testing::Test {
protected:
    using IntVector = std::vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SortBasicTest, SortUnsortedContainer) {
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[4], 9);
}

TEST_F(SortBasicTest, SortAlreadySorted) {
    IntVector vec{ 1, 2, 3, 4, 5 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST_F(SortBasicTest, SortReverseSorted) {
    IntVector vec{ 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST_F(SortBasicTest, SortEmptyContainer) {
    IntVector vec;
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(vec.empty());
}

TEST_F(SortBasicTest, SortSingleElement) {
    IntVector vec{ 42 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_EQ(vec[0], 42);
}

TEST_F(SortBasicTest, SortDuplicates) {
    IntVector vec{ 5, 2, 5, 1, 5, 3, 5 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    int five_count = std::count(vec.begin(), vec.end(), 5);
    EXPECT_EQ(five_count, 4);
}

TEST_F(SortBasicTest, SortNegativeNumbers) {
    IntVector vec{ -5, 3, -1, 0, 2, -10 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    EXPECT_EQ(vec[0], -10);
    EXPECT_EQ(vec[5], 3);
}

TEST_F(SortBasicTest, SortLargeContainer) {
    IntVector vec;
    for (int i = 1000; i >= 0; --i) {
        vec.push_back(i);
    }

    AoL::Sort(vec.begin(), vec.end());
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST_F(SortBasicTest, SortTwoElements) {
    IntVector vec{ 2, 1 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
}

// ===================================================================
// SORT WITH COMPARATOR TESTS
// ===================================================================

class SortWithComparatorTest : public ::testing::Test {
protected:
    using IntVector = std::vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SortWithComparatorTest, SortWithGreater) {
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::Sort(vec.begin(), vec.end(), std::greater<int>());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
    EXPECT_EQ(vec[0], 9);
    EXPECT_EQ(vec[4], 1);
}

TEST_F(SortWithComparatorTest, SortWithLess) {
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::Sort(vec.begin(), vec.end(), std::less<int>());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::less<int>()));
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[4], 9);
}

TEST_F(SortWithComparatorTest, SortWithCustomComparator) {
    std::vector<CustomData> vec{
        CustomData(5, "five"),
        CustomData(2, "two"),
        CustomData(8, "eight")
    };

    AoL::Sort(vec.begin(), vec.end(),
        [](const CustomData& a, const CustomData& b) { return a.value < b.value; });

    EXPECT_EQ(vec[0].value, 2);
    EXPECT_EQ(vec[2].value, 8);
}

TEST_F(SortWithComparatorTest, SortStringsByLength) {
    std::vector<std::string> vec{ "apple", "pie", "watermelon", "cat" };
    AoL::Sort(vec.begin(), vec.end(),
        [](const std::string& a, const std::string& b) { return a.length() < b.length(); });

    EXPECT_EQ(vec[0], "pie");
    EXPECT_EQ(vec[3], "watermelon");
}

// ===================================================================
// SORT REVERSE TESTS
// ===================================================================

class SortReverseTest : public ::testing::Test {
protected:
    using IntVector = std::vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SortReverseTest, SortReverseBasic) {
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
    EXPECT_EQ(vec[0], 9);
    EXPECT_EQ(vec[4], 1);
}

TEST_F(SortReverseTest, SortReverseEmpty) {
    IntVector vec;
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(vec.empty());
}

TEST_F(SortReverseTest, SortReverseSingleElement) {
    IntVector vec{ 42 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_EQ(vec[0], 42);
}

TEST_F(SortReverseTest, SortReverseAlreadyReverse) {
    IntVector vec{ 9, 8, 7, 6, 5 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
}

TEST_F(SortReverseTest, SortReverseWithDuplicates) {
    IntVector vec{ 5, 2, 5, 1, 5, 3 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
}

TEST_F(SortReverseTest, SortReverseNegativeNumbers) {
    IntVector vec{ -5, 3, -1, 0, 2, -10 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
    EXPECT_EQ(vec[0], 3);
    EXPECT_EQ(vec[5], -10);
}

TEST_F(SortReverseTest, SortReverseLargeContainer) {
    IntVector vec;
    for (int i = 0; i <= 1000; ++i) {
        vec.push_back(i);
    }

    AoL::SortReverse(vec.begin(), vec.end());
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
    EXPECT_EQ(vec[0], 1000);
    EXPECT_EQ(vec[1000], 0);
}

// ===================================================================
// SORT REVERSE WITH COMPARATOR TESTS
// ===================================================================

class SortReverseWithComparatorTest : public ::testing::Test {
protected:
    using IntVector = std::vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SortReverseWithComparatorTest, SortReverseWithLess) {
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::SortReverse(vec.begin(), vec.end(), std::less<int>());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
}

TEST_F(SortReverseWithComparatorTest, SortReverseWithCustom) {
    std::vector<CustomData> vec{
        CustomData(5, "five"),
        CustomData(2, "two"),
        CustomData(8, "eight")
    };

    AoL::SortReverse(vec.begin(), vec.end(),
        [](const CustomData& a, const CustomData& b) { return a.value < b.value; });

    EXPECT_EQ(vec[0].value, 8);
    EXPECT_EQ(vec[2].value, 2);
}

// ===================================================================
// SORT WITH POINTERS/ARRAYS
// ===================================================================

class SortWithArraysTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SortWithArraysTest, SortRawArray) {
    int arr[] = { 5, 2, 8, 1, 9 };
    AoL::Sort(std::begin(arr), std::end(arr));

    EXPECT_TRUE(std::is_sorted(std::begin(arr), std::end(arr)));
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[4], 9);
}

TEST_F(SortWithArraysTest, SortReverseRawArray) {
    int arr[] = { 5, 2, 8, 1, 9 };
    AoL::SortReverse(std::begin(arr), std::end(arr));

    EXPECT_TRUE(std::is_sorted(std::begin(arr), std::end(arr), std::greater<int>()));
    EXPECT_EQ(arr[0], 9);
    EXPECT_EQ(arr[4], 1);
}

TEST_F(SortWithArraysTest, SortArrayWithComparator) {
    int arr[] = { 5, 2, 8, 1, 9 };
    AoL::Sort(std::begin(arr), std::end(arr), std::greater<int>());

    EXPECT_EQ(arr[0], 9);
    EXPECT_EQ(arr[4], 1);
}

// ===================================================================
// EDGE CASES AND STRESS TESTS
// ===================================================================

class AlgorithmEdgeCasesTest : public ::testing::Test {
protected:
    using IntVector = std::vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AlgorithmEdgeCasesTest, FindInPartialRange) {
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto it = AoL::FindBrute(vec.begin() + 3, vec.end() - 2, 7);

    EXPECT_NE(it, vec.end() - 2);
    EXPECT_EQ(*it, 7);
}

TEST_F(AlgorithmEdgeCasesTest, LowerBoundPartialRange) {
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto it = AoL::FindLowerBound(vec.begin() + 2, vec.end() - 2, 5);

    EXPECT_EQ(*it, 5);
}

TEST_F(AlgorithmEdgeCasesTest, SortPartialRange) {
    IntVector vec{ 1, 5, 3, 7, 2, 6, 4, 8 };
    AoL::Sort(vec.begin() + 1, vec.begin() + 5);  // Sort only middle part

    // Middle should be sorted, but boundaries untouched
    EXPECT_TRUE(std::is_sorted(vec.begin() + 1, vec.begin() + 5));
}

TEST_F(AlgorithmEdgeCasesTest, SortWithAllIdenticalElements) {
    IntVector vec{ 5, 5, 5, 5, 5, 5 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    for (int val : vec) {
        EXPECT_EQ(val, 5);
    }
}

TEST_F(AlgorithmEdgeCasesTest, FindWithAllIdenticalElements) {
    IntVector vec{ 7, 7, 7, 7, 7 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 7);

    EXPECT_EQ(it, vec.begin());
}

TEST_F(AlgorithmEdgeCasesTest, AlternatingValuesSort) {
    IntVector vec{ 1, 2, 1, 2, 1, 2, 1, 2 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST_F(AlgorithmEdgeCasesTest, VeryLargeValues) {
    IntVector vec{ 1000000, 500000, 2000000, 100 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_EQ(vec[0], 100);
    EXPECT_EQ(vec[3], 2000000);
}

TEST_F(AlgorithmEdgeCasesTest, MixedPositiveNegativeZero) {
    IntVector vec{ -100, 50, 0, -1, 100, 0, -50 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    EXPECT_EQ(vec[0], -100);
    EXPECT_EQ(vec[6], 100);
}

// ===================================================================
// TEST FIXTURES AND UTILITIES
// ===================================================================

// Custom type for testing
struct TestData {
    int id;
    std::string description;

    TestData(int i = 0, const std::string& d = "") : id(i), description(d) {}

    bool operator==(const TestData& other) const {
        return id == other.id && description == other.description;
    }

    bool operator<(const TestData& other) const {
        return id < other.id;
    }
};

// ===================================================================
// FLAT KEY ORDER MAP BASIC TESTS
// ===================================================================

class FlatKeyOrderMapBasicTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapBasicTest, DefaultConstruction) {
    TestMap map;
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

TEST_F(FlatKeyOrderMapBasicTest, ConstructionWithCapacity) {
    TestMap map(100);
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

TEST_F(FlatKeyOrderMapBasicTest, ConstructionFromIterators) {
    std::vector<AoL::FlatKeyOrderMapPair<int, std::string>> data{
        {1, "one"},
        {3, "three"},
        {2, "two"}
    };
    TestMap map(data.begin(), data.end());

    EXPECT_EQ(map.size(), 3);
    // Should be sorted by key
    EXPECT_EQ(map.begin()->first, 1);
    EXPECT_EQ((map.begin() + 1)->first, 2);
    EXPECT_EQ((map.begin() + 2)->first, 3);
}

// ===================================================================
// BUILD PATTERN TESTS
// ===================================================================

class FlatKeyOrderMapBuildPatternTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapBuildPatternTest, BuildStartAddEnd) {
    TestMap map;
    map.build_start();

    map.build_add(3, "three");
    map.build_add(1, "one");
    map.build_add(2, "two");

    map.build_end();

    EXPECT_EQ(map.size(), 3);
    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; }));
}

TEST_F(FlatKeyOrderMapBuildPatternTest, BuildWithManyElements) {
    TestMap map(1000);
    map.build_start();

    // Add in random order
    for (int i = 100; i >= 0; --i) {
        map.build_add(i, "value_" + std::to_string(i));
    }

    map.build_end();

    EXPECT_EQ(map.size(), 101);
    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; }));
}

TEST_F(FlatKeyOrderMapBuildPatternTest, BuildPreservesOrder) {
    TestMap map;
    map.build_start();

    map.build_add(5, "five");
    map.build_add(2, "two");
    map.build_add(8, "eight");
    map.build_add(1, "one");

    map.build_end();

    // After build_end, should be sorted by key
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

class FlatKeyOrderMapInsertTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapInsertTest, InsertSingleElement) {
    TestMap map;
    map.insert(5, "five");

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[5], "five");
}

TEST_F(FlatKeyOrderMapInsertTest, InsertMultipleElements) {
    TestMap map;
    map.insert(5, "five");
    map.insert(2, "two");
    map.insert(8, "eight");

    EXPECT_EQ(map.size(), 3);
    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; }));
}

TEST_F(FlatKeyOrderMapInsertTest, InsertMaintainsKeyOrder) {
    TestMap map;
    map.insert(10, "ten");
    map.insert(5, "five");
    map.insert(15, "fifteen");
    map.insert(3, "three");

    // Verify key order
    EXPECT_EQ(map.begin()->first, 3);
    EXPECT_EQ((map.begin() + 1)->first, 5);
    EXPECT_EQ((map.begin() + 2)->first, 10);
    EXPECT_EQ((map.begin() + 3)->first, 15);
}

TEST_F(FlatKeyOrderMapInsertTest, InsertInOrder) {
    TestMap map;
    for (int i = 1; i <= 10; ++i) {
        map.insert(i, "value_" + std::to_string(i));
    }

    EXPECT_EQ(map.size(), 10);
    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; }));
}

TEST_F(FlatKeyOrderMapInsertTest, InsertReverseOrder) {
    TestMap map;
    for (int i = 10; i >= 1; --i) {
        map.insert(i, "value_" + std::to_string(i));
    }

    EXPECT_EQ(map.size(), 10);
    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; }));
}

// ===================================================================
// ACCESS OPERATIONS TESTS
// ===================================================================

class FlatKeyOrderMapAccessTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapAccessTest, OperatorBracketAccess) {
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");
    map.insert(3, "three");

    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
    EXPECT_EQ(map[3], "three");
}

TEST_F(FlatKeyOrderMapAccessTest, ConstOperatorBracketAccess) {
    TestMap map;
    map.insert(5, "five");

    const TestMap& const_map = map;
    EXPECT_EQ(const_map[5], "five");
}

TEST_F(FlatKeyOrderMapAccessTest, AtRefAccess) {
    TestMap map;
    map.insert(10, "ten");

    auto& ref = map.at_ref(10);
    ref = "TEN";

    EXPECT_EQ(map[10], "TEN");
}

TEST_F(FlatKeyOrderMapAccessTest, AtPtrAccess) {
    TestMap map;
    map.insert(20, "twenty");

    auto ptr = map.at_ptr(20);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, "twenty");
}

TEST_F(FlatKeyOrderMapAccessTest, AtPtrNonExistent) {
    TestMap map;
    map.insert(5, "five");

    auto ptr = map.at_ptr(999);
    EXPECT_EQ(ptr, nullptr);
}

// ===================================================================
// FIND OPERATIONS TESTS
// ===================================================================

class FlatKeyOrderMapFindTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapFindTest, FindExistingKey) {
    TestMap map;
    map.insert(5, "five");
    map.insert(10, "ten");

    auto ptr = map.find(5);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->first, 5);
    EXPECT_EQ(ptr->second, "five");
}

TEST_F(FlatKeyOrderMapFindTest, FindNonExistentKey) {
    TestMap map;
    map.insert(5, "five");

    auto ptr = map.find(999);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(FlatKeyOrderMapFindTest, FindEmptyMap) {
    TestMap map;
    auto ptr = map.find(5);

    EXPECT_EQ(ptr, nullptr);
}

TEST_F(FlatKeyOrderMapFindTest, FindConstMap) {
    TestMap map;
    map.insert(7, "seven");

    const TestMap& const_map = map;
    auto ptr = const_map.find(7);

    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->first, 7);
}

TEST_F(FlatKeyOrderMapFindTest, FindMultipleElements) {
    TestMap map;
    for (int i = 1; i <= 100; ++i) {
        map.insert(i, "value_" + std::to_string(i));
    }

    // Test various lookups
    for (int i : {1, 25, 50, 75, 100}) {
        auto ptr = map.find(i);
        EXPECT_NE(ptr, nullptr);
        EXPECT_EQ(ptr->first, i);
    }
}

// ===================================================================
// CONTAINS TESTS
// ===================================================================

class FlatKeyOrderMapContainsTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapContainsTest, ContainsExisting) {
    TestMap map;
    map.insert(42, "answer");

    EXPECT_TRUE(map.contains(42));
}

TEST_F(FlatKeyOrderMapContainsTest, ContainsNonExistent) {
    TestMap map;
    map.insert(42, "answer");

    EXPECT_FALSE(map.contains(999));
}

TEST_F(FlatKeyOrderMapContainsTest, ContainsEmptyMap) {
    TestMap map;

    EXPECT_FALSE(map.contains(5));
}

TEST_F(FlatKeyOrderMapContainsTest, ContainsMultipleKeys) {
    TestMap map;
    for (int i = 0; i < 50; ++i) {
        map.insert(i, "val");
    }

    for (int i = 0; i < 50; ++i) {
        EXPECT_TRUE(map.contains(i));
    }

    EXPECT_FALSE(map.contains(100));
}

// ===================================================================
// ITERATION TESTS
// ===================================================================

class FlatKeyOrderMapIterationTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapIterationTest, ForwardIteration) {
    TestMap map;
    map.insert(5, "five");
    map.insert(2, "two");
    map.insert(8, "eight");

    std::vector<int> keys;
    for (auto& pair : map) {
        keys.push_back(pair.first);
    }

    EXPECT_EQ(keys[0], 2);
    EXPECT_EQ(keys[1], 5);
    EXPECT_EQ(keys[2], 8);
}

TEST_F(FlatKeyOrderMapIterationTest, ReverseIteration) {
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");
    map.insert(3, "three");

    std::vector<int> keys;
    for (auto it = map.rbegin(); it != map.rend(); ++it) {
        keys.push_back(it->first);
    }

    EXPECT_EQ(keys[0], 3);
    EXPECT_EQ(keys[1], 2);
    EXPECT_EQ(keys[2], 1);
}

TEST_F(FlatKeyOrderMapIterationTest, ConstIteration) {
    TestMap map;
    map.insert(10, "ten");
    map.insert(20, "twenty");

    const TestMap& const_map = map;
    int count = 0;
    for (auto it = const_map.cbegin(); it != const_map.cend(); ++it) {
        count++;
    }

    EXPECT_EQ(count, 2);
}

TEST_F(FlatKeyOrderMapIterationTest, IterationOrderIsKeyOrder) {
    TestMap map;
    for (int i = 100; i >= 1; --i) {
        map.insert(i, "val");
    }

    int prev_key = 0;
    for (auto& pair : map) {
        EXPECT_GT(pair.first, prev_key);
        prev_key = pair.first;
    }
}

// ===================================================================
// CLEAR AND SIZE TESTS
// ===================================================================

class FlatKeyOrderMapClearTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapClearTest, ClearEmptyMap) {
    TestMap map;
    map.clear();

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

TEST_F(FlatKeyOrderMapClearTest, ClearNonEmptyMap) {
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");
    map.insert(3, "three");

    map.clear();

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

TEST_F(FlatKeyOrderMapClearTest, InsertAfterClear) {
    TestMap map;
    map.insert(5, "five");
    map.clear();

    map.insert(10, "ten");

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[10], "ten");
}

TEST_F(FlatKeyOrderMapClearTest, SizeTracking) {
    TestMap map;
    EXPECT_EQ(map.size(), 0);

    for (int i = 1; i <= 10; ++i) {
        map.insert(i, "val");
        EXPECT_EQ(map.size(), i);
    }
}

// ===================================================================
// DATA ACCESS TESTS
// ===================================================================

class FlatKeyOrderMapDataTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapDataTest, DataPointerAccess) {
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    auto data = map.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0].first, 1);
    EXPECT_EQ(data[1].first, 2);
}

TEST_F(FlatKeyOrderMapDataTest, ConstDataPointerAccess) {
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

class FlatKeyOrderMapCustomTypeTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, TestData>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapCustomTypeTest, InsertCustomType) {
    TestMap map;
    map.insert(1, TestData(1, "first"));
    map.insert(2, TestData(2, "second"));

    EXPECT_EQ(map[1].id, 1);
    EXPECT_EQ(map[1].description, "first");
}

TEST_F(FlatKeyOrderMapCustomTypeTest, BuildWithCustomType) {
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

class FlatKeyOrderMapEdgeCasesTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapEdgeCasesTest, SingleElement) {
    TestMap map;
    map.insert(42, "answer");

    EXPECT_FALSE(map.empty());
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[42], "answer");
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, NegativeKeys) {
    TestMap map;
    map.insert(-5, "negative_five");
    map.insert(0, "zero");
    map.insert(5, "positive_five");

    EXPECT_EQ(map[-5], "negative_five");
    EXPECT_EQ(map[0], "zero");
    EXPECT_EQ(map[5], "positive_five");

    // Check order
    EXPECT_EQ(map.begin()->first, -5);
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, LargeNumbers) {
    TestMap map;
    int large = 1000000;
    map.insert(large, "large");
    map.insert(large - 1, "smaller");

    EXPECT_EQ(map[large], "large");
    EXPECT_EQ(map[large - 1], "smaller");
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, EmptyStrings) {
    TestMap map;
    map.insert(1, "");
    map.insert(2, "non-empty");

    EXPECT_EQ(map[1], "");
    EXPECT_EQ(map[2], "non-empty");
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, LargeMapPerformance) {
    TestMap map(10000);

    // Insert in random order
    for (int i = 5000; i >= 0; --i) {
        map.insert(i, "value_" + std::to_string(i));
    }
    for (int i = 5001; i < 10000; ++i) {
        map.insert(i, "value_" + std::to_string(i));
    }

    EXPECT_EQ(map.size(), 10000);

    // Verify ordering
    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; }));

    // Spot check lookups
    for (int i : {0, 2500, 5000, 7500, 9999}) {
        auto ptr = map.find(i);
        EXPECT_NE(ptr, nullptr);
        EXPECT_EQ(ptr->first, i);
    }
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, BuildEmptyMap) {
    TestMap map;
    map.build_start();
    map.build_end();

    EXPECT_TRUE(map.empty());
}

TEST_F(FlatKeyOrderMapEdgeCasesTest, MultipleBuilds) {
    TestMap map;

    // First build
    map.build_start();
    map.build_add(1, "one");
    map.build_add(2, "two");
    map.build_end();

    EXPECT_EQ(map.size(), 2);

    // Clear and second build
    map.clear();
    map.build_start();
    map.build_add(5, "five");
    map.build_add(3, "three");
    map.build_end();

    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map[3].c_str()[0], 't');  // Verify value
}

// ===================================================================
// PERFORMANCE CHARACTERISTIC TESTS
// ===================================================================

class FlatKeyOrderMapPerformanceTest : public ::testing::Test {
protected:
    using TestMap = AoL::FlatKeyOrderMap<int, int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FlatKeyOrderMapPerformanceTest, LookupInLargeMapVsFill) {
    TestMap map;
    const int size = 1000;

    // Build pattern is faster for bulk inserts
    map.build_start();
    for (int i = 0; i < size; ++i) {
        map.build_add(i, i * 2);
    }
    map.build_end();

    EXPECT_EQ(map.size(), size);

    // Lookups should be efficient (binary search)
    for (int i : {0, size / 2, size - 1}) {
        auto ptr = map.find(i);
        EXPECT_NE(ptr, nullptr);
        EXPECT_EQ(ptr->second, i * 2);
    }
}

TEST_F(FlatKeyOrderMapPerformanceTest, IterationThroughAllElements) {
    TestMap map;
    const int size = 100;

    for (int i = 0; i < size; ++i) {
        map.insert(i, i * 10);
    }

    int sum = 0;
    for (auto& pair : map) {
        sum += pair.second;
    }

    int expected = 0;
    for (int i = 0; i < size; ++i) {
        expected += i * 10;
    }

    EXPECT_EQ(sum, expected);
}

} // namespace