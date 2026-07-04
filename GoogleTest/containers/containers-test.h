#pragma once

/********************************************************************
* Container tests
* - Tests all container types: Array, Vector, Maps, Sets, CyclicBuffers
* - Tests insertion, deletion, iteration, and various operations
********************************************************************/

#include <gtest/gtest.h>
#include "aol/aol.h"

namespace {

// ===================================================================
// ARRAY TESTS
// ===================================================================

class ArrayTest : public ::testing::Test {
protected:
    static constexpr AoL::SizeT SIZE = 10;
    using TestArray = AoL::Array<int, SIZE>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ArrayTest, ConstructionAndSize) {
    TestArray arr{};
    EXPECT_EQ(AoL::GetContainerSize(arr), SIZE);
}

TEST_F(ArrayTest, InitializerList) {
    AoL::Array<int, 5> arr{ 1, 2, 3, 4, 5 };
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[4], 5);
}

TEST_F(ArrayTest, ElementAccess) {
    TestArray arr{};
    for (AoL::SizeT i = 0; i < SIZE; ++i) {
        arr[i] = static_cast<int>(i);
    }
    for (AoL::SizeT i = 0; i < SIZE; ++i) {
        EXPECT_EQ(arr[i], static_cast<int>(i));
    }
}

TEST_F(ArrayTest, IteratorAccess) {
    AoL::Array<int, 3> arr{ 10, 20, 30 };
    auto it = AoL::GetBeginIt(arr);
    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
}

TEST_F(ArrayTest, ConstIteratorAccess) {
    AoL::Array<int, 3> arr{ 5, 10, 15 };
    auto it = AoL::GetBeginIt(static_cast<const AoL::Array<int, 3>&>(arr));
    EXPECT_EQ(*it, 5);
}

// ===================================================================
// VECTOR TESTS
// ===================================================================

class VectorTest : public ::testing::Test {
protected:
    using TestVector = AoL::Vector<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(VectorTest, DefaultConstruction) {
    TestVector vec;
    EXPECT_TRUE(AoL::IsContainerEmpty(vec));
    EXPECT_EQ(AoL::GetContainerSize(vec), 0);
}

TEST_F(VectorTest, PushBackAndSize) {
    TestVector vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    EXPECT_EQ(AoL::GetContainerSize(vec), 3);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_EQ(vec[2], 30);
}

TEST_F(VectorTest, PopBack) {
    TestVector vec{ 10, 20, 30 };
    vec.pop_back();

    EXPECT_EQ(AoL::GetContainerSize(vec), 2);
    EXPECT_EQ(vec[1], 20);
}

TEST_F(VectorTest, Clear) {
    TestVector vec{ 1, 2, 3, 4, 5 };
    vec.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(vec));
    EXPECT_EQ(AoL::GetContainerSize(vec), 0);
}

TEST_F(VectorTest, Reserve) {
    TestVector vec;
    vec.reserve(100);
    EXPECT_GE(vec.capacity(), 100);
}

TEST_F(VectorTest, IterationForward) {
    TestVector vec{ 1, 2, 3, 4, 5 };
    int sum = 0;
    for (auto it = AoL::GetBeginIt(vec); it != AoL::GetEndIt(vec); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 15);
}

TEST_F(VectorTest, RangeIteration) {
    TestVector vec{ 10, 20, 30 };
    int count = 0;
    for (auto& elem : vec) {
        count++;
    }
    EXPECT_EQ(count, 3);
}

// ===================================================================
// KEY-ORDERED MAP TESTS
// ===================================================================

class KeyOrderMapTest : public ::testing::Test {
protected:
    using TestMap = AoL::KeyOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(KeyOrderMapTest, InsertAndFind) {
    TestMap map;
    map[1] = "one";
    map[2] = "two";
    map[3] = "three";

    EXPECT_EQ(AoL::GetContainerSize(map), 3);
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
}

TEST_F(KeyOrderMapTest, KeyOrdering) {
    TestMap map;
    map[3] = "three";
    map[1] = "one";
    map[2] = "two";

    auto it = AoL::GetBeginIt(map);
    EXPECT_EQ(it->first, 1);  // Should be ordered by key
    ++it;
    EXPECT_EQ(it->first, 2);
    ++it;
    EXPECT_EQ(it->first, 3);
}

TEST_F(KeyOrderMapTest, Contains) {
    TestMap map;
    map[5] = "five";

    EXPECT_NE(map.find(5), AoL::GetEndIt(map));
    EXPECT_EQ(map.find(10), AoL::GetEndIt(map));
}

TEST_F(KeyOrderMapTest, Erase) {
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

class HashMapTest : public ::testing::Test {
protected:
    using TestMap = AoL::HashMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(HashMapTest, InsertAndRetrieve) {
    TestMap map;
    map[1] = "one";
    map[2] = "two";
    map[42] = "answer";

    EXPECT_EQ(AoL::GetContainerSize(map), 3);
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[42], "answer");
}

TEST_F(HashMapTest, Find) {
    TestMap map;
    map[100] = "hundred";

    auto it = map.find(100);
    EXPECT_NE(it, AoL::GetEndIt(map));
    EXPECT_EQ(it->second, "hundred");

    auto it_not_found = map.find(999);
    EXPECT_EQ(it_not_found, AoL::GetEndIt(map));
}

TEST_F(HashMapTest, ClearAndEmpty) {
    TestMap map;
    map[1] = "one";
    map[2] = "two";

    EXPECT_FALSE(AoL::IsContainerEmpty(map));
    map.clear();
    EXPECT_TRUE(AoL::IsContainerEmpty(map));
}

TEST_F(HashMapTest, Erase) {
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

class InsertOrderMapTest : public ::testing::Test {
protected:
    using TestMap = AoL::InsertOrderMap<int, std::string>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(InsertOrderMapTest, InsertionOrder) {
    TestMap map;
    map[3] = "three";
    map[1] = "one";
    map[2] = "two";

    auto it = AoL::GetBeginIt(map);
    EXPECT_EQ(it->first, 3);  // Order of insertion, not key order
    ++it;
    EXPECT_EQ(it->first, 1);
    ++it;
    EXPECT_EQ(it->first, 2);
}

TEST_F(InsertOrderMapTest, Find) {
    TestMap map;
    map[42] = "answer";

    auto it = map.find(42);
    EXPECT_NE(it, AoL::GetEndIt(map));
    EXPECT_EQ(it->second, "answer");
}

// ===================================================================
// KEY-ORDERED SET TESTS
// ===================================================================

class KeyOrderSetTest : public ::testing::Test {
protected:
    using TestSet = AoL::KeyOrderSet<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(KeyOrderSetTest, InsertAndFind) {
    TestSet set;
    set.insert(10);
    set.insert(5);
    set.insert(15);

    EXPECT_EQ(AoL::GetContainerSize(set), 3);
    EXPECT_NE(set.find(10), AoL::GetEndIt(set));
}

TEST_F(KeyOrderSetTest, KeyOrdering) {
    TestSet set;
    set.insert(30);
    set.insert(10);
    set.insert(20);

    auto it = AoL::GetBeginIt(set);
    EXPECT_EQ(*it, 10);  // Ordered by key
    ++it;
    EXPECT_EQ(*it, 20);
    ++it;
    EXPECT_EQ(*it, 30);
}

TEST_F(KeyOrderSetTest, NoDuplicates) {
    TestSet set;
    set.insert(5);
    set.insert(5);
    set.insert(5);

    EXPECT_EQ(AoL::GetContainerSize(set), 1);
}

TEST_F(KeyOrderSetTest, Erase) {
    TestSet set{ 1, 2, 3, 4, 5 };
    set.erase(3);

    EXPECT_EQ(AoL::GetContainerSize(set), 4);
    EXPECT_EQ(set.find(3), AoL::GetEndIt(set));
}

// ===================================================================
// HASH SET TESTS
// ===================================================================

class HashSetTest : public ::testing::Test {
protected:
    using TestSet = AoL::HashSet<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(HashSetTest, InsertAndFind) {
    TestSet set;
    set.insert(42);
    set.insert(100);
    set.insert(7);

    EXPECT_EQ(AoL::GetContainerSize(set), 3);
    EXPECT_NE(set.find(42), AoL::GetEndIt(set));
}

TEST_F(HashSetTest, NoDuplicates) {
    TestSet set;
    set.insert(1);
    set.insert(1);
    set.insert(1);

    EXPECT_EQ(AoL::GetContainerSize(set), 1);
}

TEST_F(HashSetTest, Erase) {
    TestSet set{ 10, 20, 30 };
    set.erase(20);

    EXPECT_EQ(AoL::GetContainerSize(set), 2);
    EXPECT_EQ(set.find(20), AoL::GetEndIt(set));
}

// ===================================================================
// CYCLIC BUFFER FIXED TESTS
// ===================================================================

class CyclicBufferFixedTest : public ::testing::Test {
protected:
    static constexpr AoL::SizeT BUFFER_SIZE = 8;
    using TestBuffer = AoL::CyclicBufferF<int, BUFFER_SIZE>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CyclicBufferFixedTest, PushAndSize) {
    TestBuffer buf;
    buf.push_back(10);
    buf.push_back(20);
    buf.push_back(30);

    EXPECT_EQ(buf.size(), 3);
}

TEST_F(CyclicBufferFixedTest, FIFOOrder) {
    TestBuffer buf;
    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);

    // Verify FIFO order (front gets oldest elements)
    EXPECT_EQ(buf.front(), 1);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 2);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 3);
    buf.pop_front();

    EXPECT_TRUE(buf.empty());
}

TEST_F(CyclicBufferFixedTest, Overflow) {
    TestBuffer buf;
    // Fill completely (size = 8)
    for (int i = 0; i < 8; ++i) {
        buf.push_back(i);
    }
    EXPECT_EQ(buf.size(), 8);
    EXPECT_TRUE(buf.full());

    // Add one more (should overwrite oldest, which is 0)
    buf.push_back(99);
    EXPECT_EQ(buf.size(), 8);  // Still at max size

    // Front should now be 1 (oldest remaining after 0 was overwritten)
    EXPECT_EQ(buf.front(), 1);
}

TEST_F(CyclicBufferFixedTest, Empty) {
    TestBuffer buf;
    EXPECT_TRUE(buf.empty());

    buf.push_back(5);
    EXPECT_FALSE(buf.empty());

    buf.pop_front();
    EXPECT_TRUE(buf.empty());
}

// ===================================================================
// CYCLIC BUFFER DYNAMIC TESTS
// ===================================================================

class CyclicBufferDynamicTest : public ::testing::Test {
protected:
    using TestBuffer = AoL::CyclicBufferD<int>;

    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CyclicBufferDynamicTest, GrowsAsNeeded) {
    TestBuffer buf(128);  // Initialize with capacity of 128 (power of 2)
    for (int i = 0; i < 100; ++i) {
        buf.push_back(i);
    }
    EXPECT_EQ(buf.size(), 100);
}

TEST_F(CyclicBufferDynamicTest, FIFOOrder) {
    TestBuffer buf(16);  // Initialize with capacity of 16 (power of 2)
    buf.push_back(5);
    buf.push_back(10);
    buf.push_back(15);

    EXPECT_EQ(buf.front(), 5);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 10);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 15);
    buf.pop_front();

    EXPECT_TRUE(buf.empty());
}

// ===================================================================
// CONTAINER QUERY FUNCTIONS TESTS
// ===================================================================

class ContainerQueriesTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ContainerQueriesTest, GetBeginItAndEndIt) {
    AoL::Vector<int> vec{ 1, 2, 3 };
    auto begin = AoL::GetBeginIt(vec);
    auto end = AoL::GetEndIt(vec);

    EXPECT_NE(begin, end);
    EXPECT_EQ(*begin, 1);
}

TEST_F(ContainerQueriesTest, GetContainerSize) {
    AoL::Vector<double> vec{ 1.1, 2.2, 3.3, 4.4 };
    EXPECT_EQ(AoL::GetContainerSize(vec), 4);
}

TEST_F(ContainerQueriesTest, IsContainerEmpty) {
    AoL::Vector<int> vec;
    EXPECT_TRUE(AoL::IsContainerEmpty(vec));

    vec.push_back(42);
    EXPECT_FALSE(AoL::IsContainerEmpty(vec));
}

TEST_F(ContainerQueriesTest, GetContainerData) {
    AoL::Vector<int> vec{ 1, 2, 3, 4 };
    auto data = AoL::GetContainerData(vec);
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0], 1);
    EXPECT_EQ(data[3], 4);
}

TEST_F(ContainerQueriesTest, ReverseIterators) {
    AoL::Vector<int> vec{ 1, 2, 3 };
    auto rbegin = AoL::GetBeginReverseIt(vec);
    EXPECT_EQ(*rbegin, 3);
}

} // namespace