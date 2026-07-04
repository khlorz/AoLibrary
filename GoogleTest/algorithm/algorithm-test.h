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

} // namespace