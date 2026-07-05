/********************************************************************
* Partition container tests
********************************************************************/

#include "pch.h"

namespace
{

// ===================================================================
// PARTITION VECTOR TESTS
// ===================================================================

class PartitionVectorTest : public ::testing::Test
{
protected:
    using TestPV = AoL::PartitionVector<int>;

    void SetUp() override
    {}

    void TearDown() override
    {}
};

}

TEST_F(PartitionVectorTest, DefaultConstruction)
{
    TestPV pv;
    EXPECT_EQ(AoL::GetContainerSize(pv), 0);
    EXPECT_EQ(pv.number_of_partitions(), 1);
}

TEST_F(PartitionVectorTest, InitListConstruction)
{
    TestPV pv{ 10, 20, 30 };
    EXPECT_EQ(AoL::GetContainerSize(pv), 3);
    EXPECT_EQ(pv.number_of_partitions(), 1);
    EXPECT_EQ(pv.size_of_partition(0), 3);
}

TEST_F(PartitionVectorTest, DefaultPartitionAccess)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& dp = pv.get_default_partition();

    EXPECT_EQ(dp.size(), 4);
    EXPECT_FALSE(dp.empty());
    EXPECT_EQ(dp[0], 1);
    EXPECT_EQ(dp[3], 4);
}

TEST_F(PartitionVectorTest, CreatePartitionBySize)
{
    TestPV pv{ 1, 2, 3, 4, 5 };
    auto& p1 = pv.create_partition(2, false);

    EXPECT_EQ(pv.number_of_partitions(), 2);
    EXPECT_EQ(pv.size_of_partition(0), 2);
    EXPECT_EQ(pv.get_partition(1).size(), 3);
    EXPECT_EQ(p1[0], 1);
    EXPECT_EQ(p1[1], 2);
}

TEST_F(PartitionVectorTest, SubPartitionPushBack)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p1 = pv.create_partition(2);

    EXPECT_TRUE(p1.emplace_back(99));
    EXPECT_EQ(p1.size(), 1);
    EXPECT_EQ(p1[0], 99);
}

TEST_F(PartitionVectorTest, SubPartitionEmplaceBack)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p1 = pv.create_partition(2);

    EXPECT_NE(p1.emplace_back(99), nullptr);
    EXPECT_EQ(p1.size(), 1);
    EXPECT_EQ(p1[0], 99);
}

TEST_F(PartitionVectorTest, SubPartitionClear)
{
    TestPV pv{ 1, 2, 3 };
    auto& dp = pv.get_default_partition();
    dp.clear();

    EXPECT_TRUE(dp.empty());
    EXPECT_EQ(dp.size(), 0);
}

TEST_F(PartitionVectorTest, SubPartitionErase)
{
    TestPV pv{ 10, 20, 30 };
    auto& dp = pv.get_default_partition();
    dp.erase(1);

    EXPECT_EQ(dp.size(), 2);
    EXPECT_EQ(dp[0], 10);
    EXPECT_EQ(dp[1], 30);
}

TEST_F(PartitionVectorTest, SubPartitionPopFront)
{
    TestPV pv{ 1, 2, 3 };
    auto& dp = pv.get_default_partition();
    dp.pop_front();

    EXPECT_EQ(dp.size(), 2);
    EXPECT_EQ(dp[0], 2);
}

TEST_F(PartitionVectorTest, SubPartitionPopBack)
{
    TestPV pv{ 5, 10 };
    auto& dp = pv.get_default_partition();
    dp.pop_back();

    EXPECT_EQ(dp.size(), 1);
    EXPECT_EQ(dp[0], 5);
}

TEST_F(PartitionVectorTest, ForwardIteration)
{
    TestPV pv{ 1, 2, 3 };
    auto& dp = pv.get_default_partition();
    int sum = 0;

    for (auto it = dp.begin(); it != dp.end(); ++it)
    {
        sum += *it;
    }

    EXPECT_EQ(sum, 6);
}

TEST_F(PartitionVectorTest, GetPartitionAccess)
{
    TestPV pv{ 1, 2, 3, 4, 5 };
    pv.create_partition(2, false);

    EXPECT_EQ(pv.get_partition(0).size(), 2);
    EXPECT_EQ(pv.get_partition(0)[0], 1);
    EXPECT_EQ(pv.get_partition(1).size(), 3);
    EXPECT_EQ(pv.get_partition(1)[0], 3);
}

TEST_F(PartitionVectorTest, SubPartitionMaxSize)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p1 = pv.create_partition(3);

    EXPECT_EQ(p1.max_size(), 3);
    EXPECT_FALSE(p1.full());
    EXPECT_NE(p1.emplace_back(10), nullptr);
    EXPECT_EQ(p1.size(), 1);
    p1.emplace_back(20);
    p1.emplace_back(30);
    EXPECT_TRUE(p1.full());
    EXPECT_EQ(p1.emplace_back(40), nullptr);
}

// ===================================================================
// PARTITION ARRAY TESTS
// ===================================================================

class PartitionArrayTest : public ::testing::Test
{
protected:
    using TestPA = AoL::PartitionArray<int, 8>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(PartitionArrayTest, DefaultConstruction)
{
    TestPA pa;
    EXPECT_EQ(pa.number_of_partitions(), 1);
}

TEST_F(PartitionArrayTest, SubPartitionInitiallyEmpty)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();

    EXPECT_TRUE(dp.empty());
    EXPECT_EQ(dp.size(), 0);
    EXPECT_EQ(dp.max_size(), 8);
}

TEST_F(PartitionArrayTest, SubPartitionEmplaceBack)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();

    EXPECT_NE(dp.emplace_back(42), nullptr);
    EXPECT_EQ(dp.size(), 1);
    EXPECT_EQ(dp[0], 42);
}

TEST_F(PartitionArrayTest, SubPartitionPushBack)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();

    EXPECT_NE(dp.emplace_back(42), nullptr);
    EXPECT_EQ(dp.size(), 1);
    EXPECT_EQ(dp[0], 42);
}

TEST_F(PartitionArrayTest, SubPartitionFull)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();

    for (int i = 0; i < 8; ++i)
    {
        EXPECT_NE(dp.emplace_back(i), nullptr);
    }

    EXPECT_TRUE(dp.full());
    EXPECT_EQ(dp.emplace_back(99), nullptr);
    EXPECT_EQ(dp.emplace_back(99), nullptr);
}

TEST_F(PartitionArrayTest, CreatePartitionBySize)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();

    for (int i = 0; i < 6; ++i)
    {
        dp.emplace_back(i);
    }

    auto& p1 = pa.create_partition(3, false);
    EXPECT_EQ(pa.number_of_partitions(), 2);
    EXPECT_EQ(p1.size(), 3);
    EXPECT_EQ(p1[0], 0);
    EXPECT_EQ(p1[1], 1);
    EXPECT_EQ(p1[2], 2);
}

TEST_F(PartitionArrayTest, ForwardIteration)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();

    for (int i = 0; i < 4; ++i)
    {
        dp.emplace_back(i);
    }

    int sum = 0;

    for (auto it = dp.begin(); it != dp.end(); ++it)
    {
        sum += *it;
    }

    EXPECT_EQ(sum, 6);
}
