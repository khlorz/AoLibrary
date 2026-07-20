/********************************************************************
* Sort algorithm tests: Sort, SortReverse, with comparators, arrays
********************************************************************/


#include "pch.h"

#include "aol/algorithms.h"


namespace
{

struct CustomData
{
    int value;
    std::string name;

    CustomData(int v = 0, const std::string& n = "") :
        value(v),
        name(n)
    {
    }

    bool operator<(const CustomData& other) const
    {
        return value < other.value;
    }

    bool operator==(const CustomData& other) const
    {
        return value == other.value && name == other.name;
    }

    bool operator>(const CustomData& other) const
    {
        return value > other.value;
    }

    bool operator<=(const CustomData& other) const
    {
        return value <= other.value;
    }

    bool operator>=(const CustomData& other) const
    {
        return value >= other.value;
    }
};

struct ReverseComparator
{
    template<typename T>
    bool operator()(const T& a, const T& b) const
    {
        return a > b;
    }
};

}

// ===================================================================
// SORT BASIC TESTS
// ===================================================================

class SortBasicTest : public ::testing::Test
{
protected:
    using IntVector = std::vector<int>;

    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

TEST_F(SortBasicTest, SortUnsortedContainer)
{
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[4], 9);
}

TEST_F(SortBasicTest, SortAlreadySorted)
{
    IntVector vec{ 1, 2, 3, 4, 5 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST_F(SortBasicTest, SortReverseSorted)
{
    IntVector vec{ 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST_F(SortBasicTest, SortEmptyContainer)
{
    IntVector vec;
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(vec.empty());
}

TEST_F(SortBasicTest, SortSingleElement)
{
    IntVector vec{ 42 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_EQ(vec[0], 42);
}

TEST_F(SortBasicTest, SortDuplicates)
{
    IntVector vec{ 5, 2, 5, 1, 5, 3, 5 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    int five_count = std::count(vec.begin(), vec.end(), 5);
    EXPECT_EQ(five_count, 4);
}

TEST_F(SortBasicTest, SortNegativeNumbers)
{
    IntVector vec{ -5, 3, -1, 0, 2, -10 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    EXPECT_EQ(vec[0], -10);
    EXPECT_EQ(vec[5], 3);
}

TEST_F(SortBasicTest, SortLargeContainer)
{
    IntVector vec;
    for (int i = 1000; i >= 0; --i)
    {
        vec.push_back(i);
    }

    AoL::Sort(vec.begin(), vec.end());
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST_F(SortBasicTest, SortTwoElements)
{
    IntVector vec{ 2, 1 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
}

// ===================================================================
// SORT WITH COMPARATOR TESTS
// ===================================================================

class SortWithComparatorTest : public ::testing::Test
{
protected:
    using IntVector = std::vector<int>;

    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

TEST_F(SortWithComparatorTest, SortWithGreater)
{
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::Sort(vec.begin(), vec.end(), std::greater<int>());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
    EXPECT_EQ(vec[0], 9);
    EXPECT_EQ(vec[4], 1);
}

TEST_F(SortWithComparatorTest, SortWithLess)
{
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::Sort(vec.begin(), vec.end(), std::less<int>());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::less<int>()));
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[4], 9);
}

TEST_F(SortWithComparatorTest, SortWithCustomComparator)
{
    std::vector<CustomData> vec{
        CustomData(5, "five"),
        CustomData(2, "two"),
        CustomData(8, "eight")
    };

    AoL::Sort(
        vec.begin(), 
        vec.end(),
        [](const CustomData& a, const CustomData& b)
        {
            return a.value < b.value;
        }
    );

    EXPECT_EQ(vec[0].value, 2);
    EXPECT_EQ(vec[2].value, 8);
}

TEST_F(SortWithComparatorTest, SortStringsByLength)
{
    std::vector<std::string> vec{ "apple", "pie", "watermelon", "cat" };
    AoL::Sort(
        vec.begin(),
        vec.end(),
        [](const std::string& a, const std::string& b)
        {
            return a.length() < b.length();
        }
    );

    EXPECT_EQ(vec[0], "pie");
    EXPECT_EQ(vec[3], "watermelon");
}

// ===================================================================
// SORT REVERSE TESTS
// ===================================================================

class SortReverseTest : public ::testing::Test
{
protected:
    using IntVector = std::vector<int>;

    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

TEST_F(SortReverseTest, SortReverseBasic)
{
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
    EXPECT_EQ(vec[0], 9);
    EXPECT_EQ(vec[4], 1);
}

TEST_F(SortReverseTest, SortReverseEmpty)
{
    IntVector vec;
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(vec.empty());
}

TEST_F(SortReverseTest, SortReverseSingleElement)
{
    IntVector vec{ 42 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_EQ(vec[0], 42);
}

TEST_F(SortReverseTest, SortReverseAlreadyReverse)
{
    IntVector vec{ 9, 8, 7, 6, 5 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
}

TEST_F(SortReverseTest, SortReverseWithDuplicates)
{
    IntVector vec{ 5, 2, 5, 1, 5, 3 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
}

TEST_F(SortReverseTest, SortReverseNegativeNumbers)
{
    IntVector vec{ -5, 3, -1, 0, 2, -10 };
    AoL::SortReverse(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
    EXPECT_EQ(vec[0], 3);
    EXPECT_EQ(vec[5], -10);
}

TEST_F(SortReverseTest, SortReverseLargeContainer)
{
    IntVector vec;
    for (int i = 0; i <= 1000; ++i)
    {
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

class SortReverseWithComparatorTest : public ::testing::Test
{
protected:
    using IntVector = std::vector<int>;

    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

TEST_F(SortReverseWithComparatorTest, SortReverseWithLess)
{
    IntVector vec{ 5, 2, 8, 1, 9 };
    AoL::SortReverse(vec.begin(), vec.end(), std::less<int>());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end(), std::greater<int>()));
}

TEST_F(SortReverseWithComparatorTest, SortReverseWithCustom)
{
    std::vector<CustomData> vec{
        CustomData(5, "five"),
        CustomData(2, "two"),
        CustomData(8, "eight")
    };

    AoL::SortReverse(
        vec.begin(), 
        vec.end(),
        [](const CustomData& a, const CustomData& b)
        {
            return a.value < b.value;
        }
    );

    EXPECT_EQ(vec[0].value, 8);
    EXPECT_EQ(vec[2].value, 2);
}

// ===================================================================
// SORT WITH POINTERS/ARRAYS
// ===================================================================

class SortWithArraysTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

TEST_F(SortWithArraysTest, SortRawArray)
{
    int arr[] = { 5, 2, 8, 1, 9 };
    AoL::Sort(std::begin(arr), std::end(arr));

    EXPECT_TRUE(std::is_sorted(std::begin(arr), std::end(arr)));
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[4], 9);
}

TEST_F(SortWithArraysTest, SortReverseRawArray)
{
    int arr[] = { 5, 2, 8, 1, 9 };
    AoL::SortReverse(std::begin(arr), std::end(arr));

    EXPECT_TRUE(std::is_sorted(std::begin(arr), std::end(arr), std::greater<int>()));
    EXPECT_EQ(arr[0], 9);
    EXPECT_EQ(arr[4], 1);
}

TEST_F(SortWithArraysTest, SortArrayWithComparator)
{
    int arr[] = { 5, 2, 8, 1, 9 };
    AoL::Sort(std::begin(arr), std::end(arr), std::greater<int>());

    EXPECT_EQ(arr[0], 9);
    EXPECT_EQ(arr[4], 1);
}

// ===================================================================
// EDGE CASES AND STRESS TESTS (sort-related)
// ===================================================================

class AlgorithmEdgeCasesTest : public ::testing::Test
{
protected:
    using IntVector = std::vector<int>;

    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

TEST_F(AlgorithmEdgeCasesTest, SortPartialRange)
{
    IntVector vec{ 1, 5, 3, 7, 2, 6, 4, 8 };
    AoL::Sort(vec.begin() + 1, vec.begin() + 5);

    EXPECT_TRUE(std::is_sorted(vec.begin() + 1, vec.begin() + 5));
}

TEST_F(AlgorithmEdgeCasesTest, SortWithAllIdenticalElements)
{
    IntVector vec{ 5, 5, 5, 5, 5, 5 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    for (int val : vec)
    {
        EXPECT_EQ(val, 5);
    }
}

TEST_F(AlgorithmEdgeCasesTest, AlternatingValuesSort)
{
    IntVector vec{ 1, 2, 1, 2, 1, 2, 1, 2 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST_F(AlgorithmEdgeCasesTest, VeryLargeValues)
{
    IntVector vec{ 1000000, 500000, 2000000, 100 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_EQ(vec[0], 100);
    EXPECT_EQ(vec[3], 2000000);
}

TEST_F(AlgorithmEdgeCasesTest, MixedPositiveNegativeZero)
{
    IntVector vec{ -100, 50, 0, -1, 100, 0, -50 };
    AoL::Sort(vec.begin(), vec.end());

    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    EXPECT_EQ(vec[0], -100);
    EXPECT_EQ(vec[6], 100);
}
