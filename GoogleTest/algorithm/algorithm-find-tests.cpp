/********************************************************************
* Find algorithm tests: FindBrute, FindLowerBound (general, branchless, default)
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

}

// ===================================================================
// FIND BRUTE TESTS
// ===================================================================

class FindBruteTest : public ::testing::Test
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

TEST_F(FindBruteTest, FindExistingElement)
{
    IntVector vec{ 1, 5, 3, 9, 2, 7 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindBruteTest, FindNonExistingElement)
{
    IntVector vec{ 1, 5, 3, 9, 2, 7 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 100);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindBruteTest, FindFirstOccurrence)
{
    IntVector vec{ 1, 5, 3, 5, 2, 5 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(std::distance(vec.begin(), it), 1);
}

TEST_F(FindBruteTest, FindInEmptyContainer)
{
    IntVector vec;
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 5);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindBruteTest, FindFirstElement)
{
    IntVector vec{ 10, 5, 3, 9, 2 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 10);

    EXPECT_EQ(it, vec.begin());
}

TEST_F(FindBruteTest, FindLastElement)
{
    IntVector vec{ 1, 5, 3, 9, 42 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 42);

    EXPECT_EQ(*it, 42);
    EXPECT_EQ(std::distance(vec.begin(), it), 4);
}

TEST_F(FindBruteTest, FindInSingleElementContainer)
{
    IntVector vec{ 7 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 7);

    EXPECT_EQ(it, vec.begin());
}

TEST_F(FindBruteTest, FindWithCustomType)
{
    std::vector<CustomData> vec{
        CustomData(10, "ten"),
        CustomData(20, "twenty"),
        CustomData(30, "thirty")
    };

    auto it = AoL::FindBrute(vec.begin(), vec.end(), CustomData(20, "twenty"));
    EXPECT_NE(it, vec.end());
    EXPECT_EQ(it->value, 20);
}

TEST_F(FindBruteTest, FindNegativeNumbers)
{
    IntVector vec{ -5, -1, 0, 1, 5 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), -1);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, -1);
}

TEST_F(FindBruteTest, FindWithArrayPointers)
{
    int arr[] = { 1, 2, 3, 4, 5 };
    auto it = AoL::FindBrute(std::begin(arr), std::end(arr), 3);

    EXPECT_NE(it, std::end(arr));
    EXPECT_EQ(*it, 3);
}

// ===================================================================
// FIND LOWER BOUND GENERAL TESTS
// ===================================================================

class FindLowerBoundGeneralTest : public ::testing::Test
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

TEST_F(FindLowerBoundGeneralTest, LowerBoundExistingValue)
{
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundNonExistingValueBetween)
{
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 4);

    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundBeforeAll)
{
    IntVector vec{ 5, 10, 15, 20 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 2);

    EXPECT_EQ(it, vec.begin());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundAfterAll)
{
    IntVector vec{ 5, 10, 15, 20 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 25);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundEmptyContainer)
{
    IntVector vec;
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 5);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundSingleElement)
{
    IntVector vec{ 10 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 10);

    EXPECT_EQ(*it, 10);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundDuplicates)
{
    IntVector vec{ 1, 5, 5, 5, 9 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 5);

    EXPECT_EQ(*it, 5);
    EXPECT_EQ(std::distance(vec.begin(), it), 1);
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundWithCustomComparator)
{
    IntVector vec{ 20, 15, 10, 5, 1 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 12, std::greater<int>());

    EXPECT_NE(it, vec.end());
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundLargeRange)
{
    IntVector vec;
    for (int i = 0; i < 1000; ++i)
    {
        vec.push_back(i * 2);
    }

    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), 500);
    EXPECT_NE(it, vec.end());
}

TEST_F(FindLowerBoundGeneralTest, LowerBoundNegativeNumbers)
{
    IntVector vec{ -100, -50, -10, 0, 10, 50 };
    auto it = AoL::FindLowerBoundGeneral(vec.begin(), vec.end(), -25);

    EXPECT_EQ(*it, -10);
}

// ===================================================================
// FIND LOWER BOUND BRANCHLESS TESTS
// ===================================================================

class FindLowerBoundBranchlessTest : public ::testing::Test
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

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundExistingValue)
{
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundNonExistingBetween)
{
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 4);

    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundBeforeAll)
{
    IntVector vec{ 10, 20, 30, 40 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5);

    EXPECT_EQ(it, vec.begin());
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundAfterAll)
{
    IntVector vec{ 10, 20, 30, 40 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 50);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundEmptyContainer)
{
    IntVector vec;
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5);

    EXPECT_EQ(it, vec.end());
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundPowerOfTwo)
{
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 10);

    EXPECT_EQ(*it, 10);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundNonPowerOfTwo)
{
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5);

    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundLargeContainer)
{
    IntVector vec;
    for (int i = 0; i < 10000; ++i)
    {
        vec.push_back(i);
    }

    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 5000);
    EXPECT_EQ(*it, 5000);
}

TEST_F(FindLowerBoundBranchlessTest, BranchlessLowerBoundCustomComparator)
{
    IntVector vec{ 100, 80, 60, 40, 20 };
    auto it = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 50, std::greater<int>());

    EXPECT_NE(it, vec.end());
}

// ===================================================================
// FIND LOWER BOUND DEFAULT TESTS
// ===================================================================

class FindLowerBoundDefaultTest : public ::testing::Test
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

TEST_F(FindLowerBoundDefaultTest, DefaultLowerBoundBasic)
{
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBound(vec.begin(), vec.end(), 5);

    EXPECT_NE(it, vec.end());
    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundDefaultTest, DefaultLowerBoundMissing)
{
    IntVector vec{ 1, 3, 5, 7, 9 };
    auto it = AoL::FindLowerBound(vec.begin(), vec.end(), 4);

    EXPECT_EQ(*it, 5);
}

TEST_F(FindLowerBoundDefaultTest, DefaultLowerBoundConsistency)
{
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto it_default = AoL::FindLowerBound(vec.begin(), vec.end(), 6);
    auto it_eytzinger = AoL::FindLowerBoundBranchless(vec.begin(), vec.end(), 6);

    EXPECT_EQ(it_default, it_eytzinger);
}

// ===================================================================
// EDGE CASES AND STRESS TESTS (find-related)
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

TEST_F(AlgorithmEdgeCasesTest, FindInPartialRange)
{
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto it = AoL::FindBrute(vec.begin() + 3, vec.end() - 2, 7);

    EXPECT_NE(it, vec.end() - 2);
    EXPECT_EQ(*it, 7);
}

TEST_F(AlgorithmEdgeCasesTest, LowerBoundPartialRange)
{
    IntVector vec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto it = AoL::FindLowerBound(vec.begin() + 2, vec.end() - 2, 5);

    EXPECT_EQ(*it, 5);
}

TEST_F(AlgorithmEdgeCasesTest, FindWithAllIdenticalElements)
{
    IntVector vec{ 7, 7, 7, 7, 7 };
    auto it = AoL::FindBrute(vec.begin(), vec.end(), 7);

    EXPECT_EQ(it, vec.begin());
}
