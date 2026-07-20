/********************************************************************
* Array container tests
********************************************************************/


#include "pch.h"

#include "aol/array.h"
#include "aol/utilities.h"


namespace
{

class ArrayTest : public ::testing::Test
{
protected:
    using TestArray = AoL::Array<int, 5>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

}

TEST_F(ArrayTest, DefaultConstruction)
{
    TestArray arr;
    EXPECT_EQ(arr.size(), 5);
}

TEST_F(ArrayTest, InitializerConstruction)
{
    TestArray arr{ 10, 20, 30, 40, 50 };
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[4], 50);
}

TEST_F(ArrayTest, FillOperation)
{
    TestArray arr{};
    arr.fill(42);
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(arr[i], 42);
    }
}

TEST_F(ArrayTest, BackAndFront)
{
    TestArray arr{ 1, 2, 3, 4, 99 };
    EXPECT_EQ(arr.front(), 1);
    EXPECT_EQ(arr.back(), 99);
}

TEST_F(ArrayTest, GetContainerData)
{
    TestArray arr{ 1, 2, 3, 4, 5 };
    auto data = AoL::GetContainerData(arr);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0], 1);
    EXPECT_EQ(data[4], 5);
}

TEST_F(ArrayTest, ForwardIteration)
{
    TestArray arr{ 10, 20, 30, 40, 50 };
    int sum = 0;

    for (auto& v : arr)
    {
        sum += v;
    }

    EXPECT_EQ(sum, 150);
}

TEST_F(ArrayTest, ReverseIteration)
{
    TestArray arr{ 1, 2, 3, 4, 5 };
    auto it = arr.rbegin();
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 4);
}

TEST_F(ArrayTest, ConstIteration)
{
    TestArray arr{ 7, 8, 9, 10, 11 };
    const TestArray& const_arr = arr;
    int count = 0;

    for (auto it = const_arr.cbegin(); it != const_arr.cend(); ++it)
    {
        count++;
    }

    EXPECT_EQ(count, 5);
}

TEST_F(ArrayTest, DataPointer)
{
    TestArray arr{ 1, 2, 3, 4, 5 };
    int* ptr = arr.data();
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr[0], 1);
    ptr[0] = 99;
    EXPECT_EQ(arr[0], 99);
}

TEST_F(ArrayTest, AtMethod)
{
    TestArray arr{ 9, 8, 7, 6, 5 };
    EXPECT_EQ(arr.at(0), 9);
    EXPECT_EQ(arr.at(4), 5);
    arr.at(2) = 77;
    EXPECT_EQ(arr[2], 77);
}

TEST_F(ArrayTest, Swap)
{
    TestArray a{ 1, 2, 3, 4, 5 };
    TestArray b{ 10, 20, 30, 40, 50 };

    a.swap(b);

    EXPECT_EQ(a[0], 10);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(a[4], 50);
    EXPECT_EQ(b[4], 5);
}

// ===================================================================
// ARRAY NAMED 2 TESTS
// ===================================================================

class ArrayNamed2Test : public ::testing::Test
{
protected:
    using TestArr2 = AoL::ArrayNamed2<int>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(ArrayNamed2Test, DefaultConstruction)
{
    TestArr2 arr;
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr.x, 0);
    EXPECT_EQ(arr.y, 0);
}

TEST_F(ArrayNamed2Test, ValueConstruction)
{
    TestArr2 arr(42);
    EXPECT_EQ(arr.x, 42);
    EXPECT_EQ(arr.y, 42);
}

TEST_F(ArrayNamed2Test, ExplicitConstruction)
{
    TestArr2 arr(10, 20);
    EXPECT_EQ(arr.x, 10);
    EXPECT_EQ(arr.y, 20);
}

TEST_F(ArrayNamed2Test, NamedAccess)
{
    TestArr2 arr(7, 99);
    EXPECT_EQ(arr.x, 7);
    EXPECT_EQ(arr.y, 99);
}

TEST_F(ArrayNamed2Test, ArrayAccess)
{
    TestArr2 arr(3, 8);
    EXPECT_EQ(arr.data_arr[0], 3);
    EXPECT_EQ(arr.data_arr[1], 8);
}

TEST_F(ArrayNamed2Test, ForwardIteration)
{
    TestArr2 arr(5, 10);
    int sum = 0;

    for (auto& v : arr)
    {
        sum += v;
    }

    EXPECT_EQ(sum, 15);
}

// ===================================================================
// ARRAY NAMED 3 TESTS
// ===================================================================

class ArrayNamed3Test : public ::testing::Test
{
protected:
    using TestArr3 = AoL::ArrayNamed3<int>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(ArrayNamed3Test, DefaultConstruction)
{
    TestArr3 arr;
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr.x, 0);
    EXPECT_EQ(arr.y, 0);
    EXPECT_EQ(arr.z, 0);
}

TEST_F(ArrayNamed3Test, ValueConstruction)
{
    TestArr3 arr(7);
    EXPECT_EQ(arr.x, 7);
    EXPECT_EQ(arr.y, 7);
    EXPECT_EQ(arr.z, 7);
}

TEST_F(ArrayNamed3Test, ExplicitConstruction)
{
    TestArr3 arr(1, 2, 3);
    EXPECT_EQ(arr.x, 1);
    EXPECT_EQ(arr.y, 2);
    EXPECT_EQ(arr.z, 3);
}

TEST_F(ArrayNamed3Test, NamedAccess)
{
    TestArr3 arr(10, 20, 30);
    EXPECT_EQ(arr.x, 10);
    EXPECT_EQ(arr.y, 20);
    EXPECT_EQ(arr.z, 30);
}

TEST_F(ArrayNamed3Test, ArrayAccess)
{
    TestArr3 arr(4, 5, 6);
    EXPECT_EQ(arr.data_arr[0], 4);
    EXPECT_EQ(arr.data_arr[1], 5);
    EXPECT_EQ(arr.data_arr[2], 6);
}

TEST_F(ArrayNamed3Test, ForwardIteration)
{
    TestArr3 arr(1, 2, 3);
    int sum = 0;

    for (auto& v : arr)
    {
        sum += v;
    }

    EXPECT_EQ(sum, 6);
}

// ===================================================================
// ARRAY NAMED 4 TESTS
// ===================================================================

class ArrayNamed4Test : public ::testing::Test
{
protected:
    using TestArr4 = AoL::ArrayNamed4<int>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(ArrayNamed4Test, DefaultConstruction)
{
    TestArr4 arr;
    EXPECT_EQ(arr.size(), 4);
    EXPECT_EQ(arr.x, 0);
    EXPECT_EQ(arr.y, 0);
    EXPECT_EQ(arr.z, 0);
    EXPECT_EQ(arr.w, 0);
}

TEST_F(ArrayNamed4Test, ValueConstruction)
{
    TestArr4 arr(5);
    EXPECT_EQ(arr.x, 5);
    EXPECT_EQ(arr.y, 5);
    EXPECT_EQ(arr.z, 5);
    EXPECT_EQ(arr.w, 5);
}

TEST_F(ArrayNamed4Test, ExplicitConstruction)
{
    TestArr4 arr(1, 2, 3, 4);
    EXPECT_EQ(arr.x, 1);
    EXPECT_EQ(arr.y, 2);
    EXPECT_EQ(arr.z, 3);
    EXPECT_EQ(arr.w, 4);
}

TEST_F(ArrayNamed4Test, NamedAccess)
{
    TestArr4 arr(10, 20, 30, 40);
    EXPECT_EQ(arr.x, 10);
    EXPECT_EQ(arr.w, 40);
}

TEST_F(ArrayNamed4Test, ArrayAccess)
{
    TestArr4 arr(4, 5, 6, 7);
    EXPECT_EQ(arr.data_arr[0], 4);
    EXPECT_EQ(arr.data_arr[3], 7);
}

TEST_F(ArrayNamed4Test, ForwardIteration)
{
    TestArr4 arr(1, 2, 3, 4);
    int sum = 0;

    for (auto& v : arr)
    {
        sum += v;
    }

    EXPECT_EQ(sum, 10);
}

