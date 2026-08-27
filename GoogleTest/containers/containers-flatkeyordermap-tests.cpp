/********************************************************************
* FlatKeyOrderMap tests: all container operations
*
* Conventions:
* - Scale/capacity/boundary numbers are named constants below, each
*   with a comment explaining why that value exists.
* - Fixture key/value literals that document themselves (e.g. 42 with
*   "answer") are intentionally left inline.
* - Tests found to be redundant are kept but disabled with #if 0 and
*   labeled with the test that supersedes them.
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

// ===================================================================
// SHARED SCALES AND PROBES
// ===================================================================
// Large enough to exercise interior shifts and growth reallocations,
// small enough to keep Debug builds fast.

constexpr int kReserveProbe = 100;         // arbitrary non-zero capacity handed to the capacity ctor; must stay empty
constexpr int kMissingProbe = 999;         // lookup key chosen to never collide with any inserted fixture data
constexpr int kStaleKey = 99;              // leftover entry proving assignment replaces (not merges) contents
constexpr int kSmallCount = 10;            // small ascending/descending fills
constexpr int kMediumCount = 100;          // multi-realloc fill for find/contains sweeps
constexpr int kHalfMediumCount = 50;       // mid-scale sweep for contains
constexpr int kLargeKeyBase = 1000000;     // well-separated large key, far from typical fixture keys
constexpr int kLargeScale = 10000;         // reserve hint and full-sweep bound for the large stress tests
constexpr int kLargeSplitPoint = 5000;     // key where the large test flips from descending to ascending insertion
constexpr int kLookupScale = 1000;         // built-map size for bulk lookup checks
constexpr int kIterationScale = 100;       // element count for the iteration-aggregate check
constexpr int kStressBuildCount = 20000;   // largest build_start/build_add/build_end run
constexpr int kStressProbeStride = 500;    // sample every 500th key of the stress build (40 probes)
constexpr int kClearCycleCount = 10;       // insert/clear repetition count
constexpr int kIteratorSampleCount = 5;    // element count for iterator arithmetic checks
constexpr int kDescendingBuildFloor = 100; // lowest key of the 100..0 descending bulk build
constexpr int kBuildReserveHint = 1000;    // capacity hint for the descending bulk build test

// Probe keys sampled at roughly 0/25/50/75/100% of their respective range.
constexpr int kMediumProbes[] = { 1, 25, 50, 75, 100 };
constexpr int kLargeProbes[] = { 0, 2500, 5000, 7500, 9999 };

// Orders pairs by .first only; mirrors FlatKeyOrderMap's key ordering.
constexpr auto ByKeyOrder = [](const auto& lhs, const auto& rhs)
{
    return lhs.first < rhs.first;
};

// Asserts the map's flat storage is sorted by key: THE core invariant.
template<typename Map>
void ExpectSortedByKey(const Map& map)
{
    EXPECT_TRUE(std::is_sorted(map.begin(), map.end(), ByKeyOrder));
}

// Canonical "value_N" payload used by range-filling tests.
std::string ValueTag(int i)
{
    return "value_" + std::to_string(i);
}

// Typed fixture root so every suite shares the TestMap alias without
// duplicating boilerplate. Suite and test names below are unchanged.
template<typename Key, typename Value>
class FlatKeyOrderMapTestBase : public ::testing::Test
{
protected:
    using TestMap = AoL::FlatKeyOrderMap<Key, Value>;
};

}

// ===================================================================
// FLAT KEY ORDER MAP BASIC TESTS
// ===================================================================

class FlatKeyOrderMapBasicTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// Default construction must yield an empty, zero-sized map.
TEST_F(FlatKeyOrderMapBasicTest, DefaultConstruction)
{
    TestMap map;
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

// Reserving capacity up front must not materialize any elements.
TEST_F(FlatKeyOrderMapBasicTest, ConstructionWithCapacity)
{
    TestMap map(kReserveProbe);
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

// A zero capacity hint is a legal no-op reserve; the map must remain
// fully usable afterwards.
TEST_F(FlatKeyOrderMapBasicTest, ConstructionWithZeroCapacity)
{
    TestMap map(0);

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);

    map.insert(1, "one");
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[1], "one");
}

// The allocator-only constructor must yield a working empty map.
TEST_F(FlatKeyOrderMapBasicTest, AllocatorConstructorStartsEmpty)
{
    typename TestMap::container_type::allocator_type alloc;
    TestMap map(alloc);

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);

    map.insert(7, "seven");
    EXPECT_EQ(map[7], "seven");
}

// Constructing from an unsorted iterator range must sort by key.
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
// TYPE-LEVEL CHECKS
// ===================================================================

class FlatKeyOrderMapTypeTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// Compile-time contract: exposed type aliases and the flat storage's
// random-access iterator category. Zero runtime cost; guards API
// regressions.
TEST_F(FlatKeyOrderMapTypeTest, TypeAliasesAndRandomAccessIterators)
{
    static_assert(std::is_same_v<typename TestMap::key_type, int>);
    static_assert(std::is_same_v<typename TestMap::mapped_type, std::string>);
    static_assert(std::is_same_v<typename TestMap::value_type, AoL::FlatKeyOrderMapPair<int, std::string>>);
    static_assert(std::is_same_v<typename TestMap::iterator::iterator_category, std::random_access_iterator_tag>);
    static_assert(std::is_same_v<typename TestMap::const_iterator::iterator_category, std::random_access_iterator_tag>);

    SUCCEED();
}

// ===================================================================
// BUILD PATTERN TESTS
// ===================================================================

class FlatKeyOrderMapBuildPatternTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// build_start/build_add/build_end must sort arbitrarily ordered adds.
TEST_F(FlatKeyOrderMapBuildPatternTest, BuildStartAddEnd)
{
    TestMap map;
    map.build_start();

    map.build_add(3, "three");
    map.build_add(1, "one");
    map.build_add(2, "two");

    map.build_end();

    EXPECT_EQ(map.size(), 3);
    ExpectSortedByKey(map);
}

// Bulk descending build of 101 elements must come out sorted; also
// exercises the capacity-hint constructor during build.
TEST_F(FlatKeyOrderMapBuildPatternTest, BuildWithManyElements)
{
    TestMap map(kBuildReserveHint);
    map.build_start();

    for (int i = kDescendingBuildFloor; i >= 0; --i)
    {
        map.build_add(i, ValueTag(i));
    }

    map.build_end();

    EXPECT_EQ(map.size(), kDescendingBuildFloor + 1);
    ExpectSortedByKey(map);
}

// Exact key sequence check: 1, 2, 5, 8 in insertion-value order.
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

class FlatKeyOrderMapInsertTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// A single insert must store and expose its payload.
TEST_F(FlatKeyOrderMapInsertTest, InsertSingleElement)
{
    TestMap map;
    map.insert(5, "five");

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[5], "five");
}

// Several inserts must keep flat storage sorted.
TEST_F(FlatKeyOrderMapInsertTest, InsertMultipleElements)
{
    TestMap map;
    map.insert(5, "five");
    map.insert(2, "two");
    map.insert(8, "eight");

    EXPECT_EQ(map.size(), 3);
    ExpectSortedByKey(map);
}

// Interior inserts must land at exact sorted positions.
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

// Ascending inserts: the cheapest insertion path must stay sorted.
TEST_F(FlatKeyOrderMapInsertTest, InsertInOrder)
{
    TestMap map;

    for (int i = 1; i <= kSmallCount; ++i)
    {
        map.insert(i, ValueTag(i));
    }

    EXPECT_EQ(map.size(), kSmallCount);
    ExpectSortedByKey(map);
}

// Descending inserts: worst case (every insert shifts the storage).
TEST_F(FlatKeyOrderMapInsertTest, InsertReverseOrder)
{
    TestMap map;

    for (int i = kSmallCount; i >= 1; --i)
    {
        map.insert(i, ValueTag(i));
    }

    EXPECT_EQ(map.size(), kSmallCount);
    ExpectSortedByKey(map);
}

// ===================================================================
// ACCESS OPERATIONS TESTS
// ===================================================================

class FlatKeyOrderMapAccessTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// operator[] must return the mapped value for present keys.
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

// Read-only access through a const map via at_ref (suite name is
// historical).
TEST_F(FlatKeyOrderMapAccessTest, ConstOperatorBracketAccess)
{
    TestMap map;
    map.insert(5, "five");

    const TestMap& const_map = map;
    EXPECT_EQ(const_map.at_ref(5), "five");
}

// at_ref must hand back a writable reference that writes through.
TEST_F(FlatKeyOrderMapAccessTest, AtRefAccess)
{
    TestMap map;
    map.insert(10, "ten");

    auto& ref = map.at_ref(10);
    ref = "TEN";

    EXPECT_EQ(map[10], "TEN");
}

// at_ptr must yield a valid pointer to the mapped value.
TEST_F(FlatKeyOrderMapAccessTest, AtPtrAccess)
{
    TestMap map;
    map.insert(20, "twenty");

    auto ptr = map.at_ptr(20);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, "twenty");
}

// at_ptr must return nullptr for absent keys.
TEST_F(FlatKeyOrderMapAccessTest, AtPtrNonExistent)
{
    TestMap map;
    map.insert(5, "five");

    auto ptr = map.at_ptr(kMissingProbe);
    EXPECT_EQ(ptr, nullptr);
}

// ===================================================================
// FIND OPERATIONS TESTS
// ===================================================================

class FlatKeyOrderMapFindTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// find() must return an iterator with the full pair payload.
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

// find() must return end() for absent keys.
TEST_F(FlatKeyOrderMapFindTest, FindNonExistentKey)
{
    TestMap map;
    map.insert(5, "five");

    auto ptr = map.find(kMissingProbe);
    EXPECT_EQ(ptr, map.end());
}

// find() on an empty map must safely return end().
TEST_F(FlatKeyOrderMapFindTest, FindEmptyMap)
{
    TestMap map;
    auto ptr = map.find(5);

    EXPECT_EQ(ptr, map.end());
}

// find() must work through a const-qualified map.
TEST_F(FlatKeyOrderMapFindTest, FindConstMap)
{
    TestMap map;
    map.insert(7, "seven");

    const TestMap& const_map = map;
    auto ptr = const_map.find(7);

    EXPECT_NE(ptr, const_map.end());
    EXPECT_EQ(ptr->first, 7);
}

// Binary search correctness across the whole key range at quarter marks.
TEST_F(FlatKeyOrderMapFindTest, FindMultipleElements)
{
    TestMap map;

    for (int i = 1; i <= kMediumCount; ++i)
    {
        map.insert(i, ValueTag(i));
    }

    for (int i : kMediumProbes)
    {
        auto ptr = map.find(i);
        EXPECT_NE(ptr, map.end());
        EXPECT_EQ(ptr->first, i);
    }
}

// find() must return end() for ANY absent key, including interior
// and below-all misses. The lower-bound behavior now lives in
// find_impl(), which is the direct FindLowerBound wrapper.
TEST_F(FlatKeyOrderMapFindTest, FindReturnsEndOnAnyMiss)
{
    TestMap map;
    map.insert(5, "five");
    map.insert(10, "ten");

    EXPECT_EQ(map.find(7), map.end());  // interior miss
    EXPECT_EQ(map.find(0), map.end());  // below-all miss
    EXPECT_EQ(map.find(11), map.end()); // above-all miss

    // Exact hits still resolve.
    EXPECT_EQ(map.find(5)->first, 5);
    EXPECT_EQ(map.find(10)->first, 10);

    // Const overload agrees.
    const TestMap& cmap = map;
    EXPECT_EQ(cmap.find(7), cmap.end());
    EXPECT_EQ(cmap.find(0), cmap.end());
    EXPECT_NE(cmap.find(5), cmap.end());
}

// find_impl() is the lower-bound query: misses between keys return
// the successor, below-all returns begin(), above-all returns end().
TEST_F(FlatKeyOrderMapFindTest, FindImplReturnsLowerBoundOnMiss)
{
    TestMap map;
    map.insert(5, "five");
    map.insert(10, "ten");

    auto interior = map.find_impl(7);
    EXPECT_NE(interior, map.end());
    EXPECT_EQ(interior->first, 10);

    auto below = map.find_impl(0);
    EXPECT_NE(below, map.end());
    EXPECT_EQ(below->first, 5);

    EXPECT_EQ(map.find_impl(5)->first, 5);
    EXPECT_EQ(map.find_impl(10)->first, 10);
    EXPECT_EQ(map.find_impl(11), map.end());

    const TestMap& cmap = map;
    auto c_interior = cmap.find_impl(7);
    EXPECT_NE(c_interior, cmap.end());
    EXPECT_EQ(c_interior->first, 10);
    EXPECT_EQ(cmap.find_impl(0)->first, 5);
    EXPECT_EQ(cmap.find_impl(11), cmap.cend());
}

// ===================================================================
// CONTAINS TESTS
// ===================================================================

class FlatKeyOrderMapContainsTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// contains() must report true for a present key.
TEST_F(FlatKeyOrderMapContainsTest, ContainsExisting)
{
    TestMap map;
    map.insert(42, "answer");

    EXPECT_TRUE(map.contains(42));
}

// contains() must report false for an absent key.
TEST_F(FlatKeyOrderMapContainsTest, ContainsNonExistent)
{
    TestMap map;
    map.insert(42, "answer");

    EXPECT_FALSE(map.contains(kMissingProbe));
}

// contains() on an empty map must be false without crashing.
TEST_F(FlatKeyOrderMapContainsTest, ContainsEmptyMap)
{
    TestMap map;

    EXPECT_FALSE(map.contains(5));
}

// Every inserted key must be found; one past-the-end key must not.
TEST_F(FlatKeyOrderMapContainsTest, ContainsMultipleKeys)
{
    TestMap map;

    for (int i = 0; i < kHalfMediumCount; ++i)
    {
        map.insert(i, "val");
    }

    for (int i = 0; i < kHalfMediumCount; ++i)
    {
        EXPECT_TRUE(map.contains(i));
    }

    EXPECT_FALSE(map.contains(kHalfMediumCount * 2));
}

// ===================================================================
// ITERATION TESTS
// ===================================================================

class FlatKeyOrderMapIterationTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// Range-for must visit keys in ascending order.
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

// Reverse iterators must visit keys in descending order.
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

// Iterating an empty map must be a clean no-op for forward and
// reverse ranges alike, with begin==end and rbegin==rend.
TEST_F(FlatKeyOrderMapIterationTest, EmptyMapIterationNoOp)
{
    const TestMap map;

    EXPECT_TRUE(map.begin() == map.end());
    EXPECT_TRUE(map.crbegin() == map.crend());

    int visits = 0;
    for (const auto& pair : map)
    {
        (void)pair;
        ++visits;
    }
    for (auto it = map.rbegin(); it != map.rend(); ++it)
    {
        ++visits;
    }
    EXPECT_EQ(visits, 0);
}

#if 0
// DISABLED: duplicate of FlatKeyOrderMapIteratorCategoryTest.CBeginCEnd
// (same cbegin/cend walk-and-count coverage). Kept for reference.
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
#endif // disabled: ConstIteration

// After 100 descending inserts, forward iteration must be strictly
// increasing: strongest single statement of the ordering invariant.
TEST_F(FlatKeyOrderMapIterationTest, IterationOrderIsKeyOrder)
{
    TestMap map;

    for (int i = kMediumCount; i >= 1; --i)
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

class FlatKeyOrderMapClearTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// clear() on an empty map must remain a valid no-op.
TEST_F(FlatKeyOrderMapClearTest, ClearEmptyMap)
{
    TestMap map;
    map.clear();

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

// clear() must empty a populated map.
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

// The map must stay reusable for inserts after clear().
TEST_F(FlatKeyOrderMapClearTest, InsertAfterClear)
{
    TestMap map;
    map.insert(5, "five");
    map.clear();

    map.insert(10, "ten");

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[10], "ten");
}

// size() must track every insert exactly, step by step.
TEST_F(FlatKeyOrderMapClearTest, SizeTracking)
{
    TestMap map;
    EXPECT_EQ(map.size(), 0);

    for (int i = 1; i <= kSmallCount; ++i)
    {
        map.insert(i, "val");
        EXPECT_EQ(map.size(), i);
    }
}

// ===================================================================
// DATA ACCESS TESTS
// ===================================================================

class FlatKeyOrderMapDataTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// data() must expose the flat storage in key order (contiguity check).
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

// data() must also be reachable through a const map.
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

class FlatKeyOrderMapCustomTypeTest : public FlatKeyOrderMapTestBase<int, TestData> {};

// Non-trivial mapped types must survive insert and operator[] reads.
TEST_F(FlatKeyOrderMapCustomTypeTest, InsertCustomType)
{
    TestMap map;
    map.insert(1, TestData(1, "first"));
    map.insert(2, TestData(2, "second"));

    EXPECT_EQ(map[1].id, 1);
    EXPECT_EQ(map[1].description, "first");
}

// The build path must handle struct values too.
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

class FlatKeyOrderMapEdgeCasesTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// Single-element state: not empty, size 1, value readable.
TEST_F(FlatKeyOrderMapEdgeCasesTest, SingleElement)
{
    TestMap map;
    map.insert(42, "answer");

    EXPECT_FALSE(map.empty());
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[42], "answer");
}

// Negative, zero, and positive keys must sort correctly together.
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

// Keys near a million must not trip any narrowing or comparison bug.
TEST_F(FlatKeyOrderMapEdgeCasesTest, LargeNumbers)
{
    TestMap map;
    int large = kLargeKeyBase;
    map.insert(large, "large");
    map.insert(large - 1, "smaller");

    EXPECT_EQ(map[large], "large");
    EXPECT_EQ(map[large - 1], "smaller");
}

// Empty-string values are legal payloads and must round-trip.
TEST_F(FlatKeyOrderMapEdgeCasesTest, EmptyStrings)
{
    TestMap map;
    map.insert(1, "");
    map.insert(2, "non-empty");

    EXPECT_EQ(map[1], "");
    EXPECT_EQ(map[2], "non-empty");
}

// Stress test: reserve once, fill descending to the midpoint then
// ascending to full scale, verify order and spot-find at quarter marks.
TEST_F(FlatKeyOrderMapEdgeCasesTest, LargeMapPerformance)
{
    TestMap map(kLargeScale);

    for (int i = kLargeSplitPoint; i >= 0; --i)
    {
        map.insert(i, ValueTag(i));
    }

    for (int i = kLargeSplitPoint + 1; i < kLargeScale; ++i)
    {
        map.insert(i, ValueTag(i));
    }

    EXPECT_EQ(map.size(), kLargeScale);

    ExpectSortedByKey(map);

    for (int i : kLargeProbes)
    {
        auto ptr = map.find(i);
        EXPECT_NE(ptr, map.end());
        EXPECT_EQ(ptr->first, i);
    }
}

// Building an empty range must produce an empty map.
TEST_F(FlatKeyOrderMapEdgeCasesTest, BuildEmptyMap)
{
    TestMap map;
    map.build_start();
    map.build_end();

    EXPECT_TRUE(map.empty());
}

// Sequential build sessions (with a clear between) must not corrupt
// state; the 't' probe pins actual payload bytes.
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
// Despite the suite name these assert correctness at scale, not timing.

class FlatKeyOrderMapPerformanceTest : public FlatKeyOrderMapTestBase<int, int> {};

#if 0
// DISABLED: measures nothing (no timing) and its lookups are a subset
// of FlatKeyOrderMapFindTest.FindMultipleElements. Kept for reference.
TEST_F(FlatKeyOrderMapPerformanceTest, LookupInLargeMapVsFill)
{
    TestMap map;
    const int size = kLookupScale;

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
#endif // disabled: LookupInLargeMapVsFill

// Summing all values during iteration must match the analytic total:
// catches dropped/duplicated elements during traversal.
TEST_F(FlatKeyOrderMapPerformanceTest, IterationThroughAllElements)
{
    TestMap map;
    const int size = kIterationScale;

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

class FlatKeyOrderMapCopyTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// Copy-constructing from an empty map must stay empty.
TEST_F(FlatKeyOrderMapCopyTest, CopyConstructEmpty)
{
    TestMap map;
    TestMap copied(map);

    EXPECT_TRUE(copied.empty());
}

// Copy construction must deep-replicate all entries in order.
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

// Copies must not share storage: post-copy inserts stay independent.
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

// Copy assignment must transfer all entries.
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

// Self copy-assignment must be a safe no-op.
TEST_F(FlatKeyOrderMapCopyTest, CopyAssignSelf)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    map = map;

    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map[1], "one");
}

// Copy assignment must replace prior contents, not merge with them.
TEST_F(FlatKeyOrderMapCopyTest, CopyAssignOverwritesOldData)
{
    TestMap map;
    map.insert(1, "one");

    TestMap assigned;
    assigned.insert(kStaleKey, "old");
    assigned = map;

    EXPECT_EQ(assigned.size(), 1);
    EXPECT_EQ(assigned[1], "one");
    EXPECT_FALSE(assigned.contains(kStaleKey));
}

// ===================================================================
// MOVE SEMANTICS TESTS
// ===================================================================

class FlatKeyOrderMapMoveTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// Move-constructing from an empty map must stay empty.
TEST_F(FlatKeyOrderMapMoveTest, MoveConstructEmpty)
{
    TestMap map;
    TestMap moved(std::move(map));

    EXPECT_TRUE(moved.empty());
}

// Move construction must transfer all entries.
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

// The moved-from vector-backed source is left empty by the storage
// steal and must be immediately reusable for fresh inserts.
TEST_F(FlatKeyOrderMapMoveTest, MoveConstructSourceEmptyAndReusable)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    TestMap moved(std::move(map));
    ASSERT_EQ(moved.size(), 2);

    EXPECT_TRUE(map.empty());
    map.insert(9, "nine");

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[9], "nine");
}

// Move assignment must transfer all entries.
TEST_F(FlatKeyOrderMapMoveTest, MoveAssignFilled)
{
    TestMap map;
    map.insert(10, "ten");

    TestMap assigned;
    assigned = std::move(map);

    EXPECT_EQ(assigned.size(), 1);
    EXPECT_EQ(assigned[10], "ten");
}

// Self move-assignment must not corrupt the map.
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

class FlatKeyOrderMapContainerCtorTest : public FlatKeyOrderMapTestBase<int, std::string>
{
protected:
    using PairType = AoL::FlatKeyOrderMapPair<int, std::string>;
};

// Container copy-ctor must adopt and sort an unsorted vector.
TEST_F(FlatKeyOrderMapContainerCtorTest, FromVectorCopy)
{
    typename TestMap::container_type data{ PairType{3, "three"}, PairType{1, "one"}, PairType{2, "two"} };
    TestMap map(data);

    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.begin()->first, 1);
    EXPECT_EQ((map.begin() + 1)->first, 2);
    EXPECT_EQ((map.begin() + 2)->first, 3);
}

// Container move-ctor must steal storage and stay sorted.
TEST_F(FlatKeyOrderMapContainerCtorTest, FromVectorMove)
{
    typename TestMap::container_type data{ PairType{5, "five"}, PairType{4, "four"} };
    TestMap map(std::move(data));

    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map.begin()->first, 4);
    EXPECT_EQ((map.begin() + 1)->first, 5);
}

// The container ctor does NOT deduplicate: equal keys stay adjacent
// after sorting and find() resolves to the first of them. Sharp edge
// worth pinning -- build_add/insert are the deduplicated entry points.
TEST_F(FlatKeyOrderMapContainerCtorTest, FromVectorWithDuplicateKeysKeepsBoth)
{
    typename TestMap::container_type data{ PairType{3, "three"}, PairType{1, "one"}, PairType{1, "uno"} };
    TestMap map(data);

    EXPECT_EQ(map.size(), 3);
    ExpectSortedByKey(map);

    auto dup = map.find(1);
    ASSERT_NE(dup, map.end());
    EXPECT_EQ(dup->first, 1);

    auto last = map.find(3);
    ASSERT_NE(last, map.end());
    EXPECT_EQ(last->first, 3);
}

// ===================================================================
// CONST CORRECTNESS TESTS
// ===================================================================

class FlatKeyOrderMapConstTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// Every const entry point must work on a default (empty) map.
TEST_F(FlatKeyOrderMapConstTest, ConstEmpty)
{
    const TestMap map;
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.find(1), map.end());
    EXPECT_FALSE(map.contains(1));
}

// Const queries (empty/size/contains/find) must agree with mutable
// observations on shared data.
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

// at_ref must be callable on a const map for read access.
TEST_F(FlatKeyOrderMapConstTest, ConstAtRef)
{
    TestMap map;
    map.insert(5, "five");

    const TestMap& const_map = map;
    EXPECT_EQ(const_map.at_ref(5), "five");
}

// at_ptr must return a valid const pointer on a const map.
TEST_F(FlatKeyOrderMapConstTest, ConstAtPtr)
{
    TestMap map;
    map.insert(7, "seven");

    const TestMap& const_map = map;
    const std::string* ptr = const_map.at_ptr(7);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, "seven");
}

// Const at_ptr must return nullptr for absent keys.
TEST_F(FlatKeyOrderMapConstTest, ConstAtPtrNonExistent)
{
    TestMap map;
    map.insert(1, "one");

    const TestMap& const_map = map;
    EXPECT_EQ(const_map.at_ptr(kMissingProbe), nullptr);
}

// Const at_ptr on an empty map must return nullptr without crashing.
TEST_F(FlatKeyOrderMapConstTest, ConstAtPtrEmpty)
{
    const TestMap map;
    EXPECT_EQ(map.at_ptr(1), nullptr);
}

// Const data() must expose the flat storage read-only.
TEST_F(FlatKeyOrderMapConstTest, ConstData)
{
    TestMap map;
    map.insert(3, "three");

    const TestMap& const_map = map;
    const auto* data = const_map.data();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data[0].first, 3);
}

// Writes through the mutable data() pointer must be visible through
// every other accessor: data() aliases the same flat storage.
TEST_F(FlatKeyOrderMapDataTest, WriteThroughDataPointer)
{
    TestMap map;
    map.insert(1, "one");
    map.insert(2, "two");

    auto* raw = map.data();
    ASSERT_NE(raw, nullptr);
    raw[0].second = "ONE";
    raw[1].second = "TWO";

    EXPECT_EQ(map[1], "ONE");
    EXPECT_EQ(map[2], "TWO");
}

// Const forward and reverse iterators must traverse fully and hit
// their respective end sentinels.
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

class FlatKeyOrderMapAtPtrModifyTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// Writes through at_ptr must be visible through every other accessor.
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

class FlatKeyOrderMapBracketTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// operator[] on a missing key must default-insert an entry and return
// a reference to it.
TEST_F(FlatKeyOrderMapBracketTest, CreatesEntryOnNonExistentKey)
{
    TestMap map;
    std::string& ref = map[42];

    EXPECT_EQ(map.size(), 1);
    EXPECT_TRUE(ref.empty());
}

// operator[] must read back previously assigned values.
TEST_F(FlatKeyOrderMapBracketTest, ReturnsExistingEntry)
{
    TestMap map;
    map[1] = "one";
    map[2] = "two";

    EXPECT_EQ(map.size(), 2);
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[2], "two");
}

// Reassigning through operator[] must overwrite, not duplicate.
TEST_F(FlatKeyOrderMapBracketTest, ChainedModification)
{
    TestMap map;
    map[1] = "first";
    map[1] = "second";

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map[1], "second");
}

// operator[] default-inserts must land at their SORTED positions no
// matter the creation order: bracket-building is a legal fill style.
TEST_F(FlatKeyOrderMapBracketTest, BracketOutOfOrderStaysSorted)
{
    TestMap map;

    map[30] = "c";
    map[10] = "a";
    map[20] = "b";

    EXPECT_EQ(map.size(), 3);
    ExpectSortedByKey(map);
    EXPECT_EQ(map.begin()->first, 10);
    EXPECT_EQ(map.rbegin()->first, 30);
}

// ===================================================================
// KEY TYPE VARIATIONS TESTS
// ===================================================================

class FlatKeyOrderMapStringKeyTest : public FlatKeyOrderMapTestBase<std::string, int> {};

// String keys must insert and read back through operator[].
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

// String keys must sort lexicographically regardless of insert order.
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

// find/contains must agree for string keys, present and absent.
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

// The build path must sort string keys alphabetically.
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

// An empty string key is legal and must coexist with non-empty keys.
TEST_F(FlatKeyOrderMapStringKeyTest, EmptyStringKey)
{
    TestMap map;
    map.insert("", 0);
    map.insert("nonempty", 1);

    EXPECT_EQ(map[""], 0);
    EXPECT_EQ(map["nonempty"], 1);
}

// Reading a missing string key through operator[] must default-create
// the entry with mapped_type{} and report the new size.
TEST_F(FlatKeyOrderMapStringKeyTest, BracketMissingKeyDefaultsToZero)
{
    TestMap map;
    map.insert("exists", 1);

    int& created = map["missing"];

    EXPECT_EQ(created, 0);
    EXPECT_EQ(map.size(), 2);
    EXPECT_TRUE(map.contains("missing"));
}

// ===================================================================
// ITERATOR CATEGORY TESTS
// ===================================================================

class FlatKeyOrderMapIteratorCategoryTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// begin()/end() must form a valid forward range covering all elements.
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

// cbegin()/cend() must walk a const map and reach the sentinel.
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

// crbegin()/crend() must walk a const map backwards.
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

// Random-access difference must report the element count.
TEST_F(FlatKeyOrderMapIteratorCategoryTest, IteratorDifference)
{
    TestMap map;
    for (int i = 0; i < kIteratorSampleCount; ++i)
    {
        map.insert(i, "val");
    }

    auto diff = map.end() - map.begin();
    EXPECT_EQ(diff, kIteratorSampleCount);
}

// Reverse iterators support the same random-access arithmetic:
// distance counts all elements and seeks index from the back.
TEST_F(FlatKeyOrderMapIteratorCategoryTest, ReverseIteratorDistanceAndSeek)
{
    TestMap map;

    for (int i = 0; i < kIteratorSampleCount; ++i)
    {
        map.insert(i, "val");
    }

    EXPECT_EQ(std::distance(map.rbegin(), map.rend()), kIteratorSampleCount);
    EXPECT_EQ(map.rbegin()->first, kIteratorSampleCount - 1);
    EXPECT_EQ((map.rbegin() + 2)->first, kIteratorSampleCount - 3);
}

// ===================================================================
// BOUNDARY VALUE TESTS
// ===================================================================

class FlatKeyOrderMapBoundaryTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// INT_MIN and INT_MAX keys must sort to the extreme positions without
// overflow in comparisons.
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

// Mixed-sign keys must interleave into correct sorted positions.
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

// Full-sweep: 10k inserts then 10k contains must all succeed, and the
// next key must be rejected.
TEST_F(FlatKeyOrderMapBoundaryTest, LargeNumberOfElements)
{
    TestMap map;
    const int n = kLargeScale;

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

class FlatKeyOrderMapEmptyTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// data() on an empty map must be callable and leave the map empty.
TEST_F(FlatKeyOrderMapEmptyTest, DataOnEmptyMap)
{
    TestMap map;
    auto* data = map.data();
    (void)data;
    EXPECT_TRUE(map.empty());
}

// Const data() on an empty map must behave identically.
TEST_F(FlatKeyOrderMapEmptyTest, ConstDataOnEmptyMap)
{
    const TestMap map;
    const auto* data = map.data();
    (void)data;
    EXPECT_TRUE(map.empty());
}

// at_ptr on an empty map must return nullptr.
TEST_F(FlatKeyOrderMapEmptyTest, AtPtrOnEmptyMap)
{
    TestMap map;
    EXPECT_EQ(map.at_ptr(1), nullptr);
}

#if 0
// DISABLED: exact duplicate of FlatKeyOrderMapClearTest.ClearEmptyMap.
// Kept for reference.
TEST_F(FlatKeyOrderMapEmptyTest, ClearEmptyMapIsNoOp)
{
    TestMap map;
    map.clear();
    EXPECT_TRUE(map.empty());
}
#endif // disabled: ClearEmptyMapIsNoOp

// clear() must reset cleanly enough that the build path works after it.
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

// Repeated insert/clear cycles with growing payloads must leave the
// map empty every time: catches stale-count bugs.
TEST_F(FlatKeyOrderMapEmptyTest, MultipleClearCycles)
{
    TestMap map;

    for (int cycle = 0; cycle < kClearCycleCount; ++cycle)
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

class FlatKeyOrderMapMixedTest : public FlatKeyOrderMapTestBase<int, int> {};

// insert, build, insert interleaved must converge to one sorted map.
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

// build, clear, build must produce exactly the second batch.
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

// Copying a build-produced map must preserve its contents.
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

// Moving an insert-populated map must transfer its contents.
TEST_F(FlatKeyOrderMapMixedTest, MoveOfInsertedMap)
{
    TestMap map;
    map.insert(1, 10);
    map.insert(2, 20);

    TestMap moved(std::move(map));
    EXPECT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[1], 10);
}

// Largest stress: 20k-element descending build must sort, and probing
// every 500th key must read back the exact payload mapping.
TEST_F(FlatKeyOrderMapMixedTest, LargeBuildWithContiguousKeys)
{
    TestMap map(kStressBuildCount);
    map.build_start();

    for (int i = kStressBuildCount - 1; i >= 0; --i)
    {
        map.build_add(i, i * 2);
    }

    map.build_end();

    EXPECT_EQ(map.size(), kStressBuildCount);

    ExpectSortedByKey(map);

    for (int i = 0; i < kStressBuildCount; i += kStressProbeStride)
    {
        EXPECT_EQ(map[i], i * 2);
    }
}

// ===================================================================
// FORWARDING REFERENCE TESTS — STRING KEY
// ===================================================================
// Verifies that every InKey&& / InValue&& parameter compiles and works
// correctly with lvalues, const lvalues, rvalues, and string literals.

class FlatKeyOrderMapFwdStringKeyTest : public FlatKeyOrderMapTestBase<std::string, int> {};

// insert() with an lvalue string key must store it intact.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertLvalueString)
{
    TestMap map;
    std::string key = "hello";
    map.insert(key, 1);

    EXPECT_EQ(map["hello"], 1);
}

// insert() with a const lvalue string key must compile and store.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertConstLvalueString)
{
    TestMap map;
    const std::string key = "world";
    map.insert(key, 2);

    EXPECT_EQ(map["world"], 2);
}

// insert() with an rvalue string key must move it in.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertRvalueString)
{
    TestMap map;
    map.insert(std::string("temp"), 3);

    EXPECT_EQ(map["temp"], 3);
}

// insert() with a string literal key must convert and store.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertStringLiteral)
{
    TestMap map;
    map.insert("literal", 4);

    EXPECT_EQ(map["literal"], 4);
}

// insert() via const char* lvalue must convert and store.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, InsertConstCharPointer)
{
    TestMap map;
    const char* ptr = "cstring";
    map.insert(ptr, 5);

    EXPECT_EQ(map["cstring"], 5);
}

// operator[] assignment with an lvalue string key.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, BracketLvalueString)
{
    TestMap map;
    std::string key = "alpha";
    map[key] = 10;

    EXPECT_EQ(map["alpha"], 10);
}

// operator[] assignment with an rvalue string key.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, BracketRvalueString)
{
    TestMap map;
    map[std::string("beta")] = 20;

    EXPECT_EQ(map["beta"], 20);
}

// operator[] assignment with a string literal key.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, BracketStringLiteral)
{
    TestMap map;
    map["gamma"] = 30;

    EXPECT_EQ(map["gamma"], 30);
}

// at_ref with an lvalue string key must locate the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtRefLvalueString)
{
    TestMap map;
    map.insert("key", 42);
    std::string key = "key";
    EXPECT_EQ(map.at_ref(key), 42);
}

// at_ref with an rvalue string key must locate the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtRefRvalueString)
{
    TestMap map;
    map.insert("key", 42);
    EXPECT_EQ(map.at_ref(std::string("key")), 42);
}

// at_ref with a string literal key must locate the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtRefStringLiteral)
{
    TestMap map;
    map.insert("key", 42);
    EXPECT_EQ(map.at_ref("key"), 42);
}

// at_ptr with an lvalue string key must find the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtPtrLvalueString)
{
    TestMap map;
    map.insert("key", 42);
    std::string key = "key";
    int* ptr = map.at_ptr(key);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

// at_ptr with an rvalue string key must find the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtPtrRvalueString)
{
    TestMap map;
    map.insert("key", 42);
    int* ptr = map.at_ptr(std::string("key"));
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

// at_ptr with a string literal key must find the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, AtPtrStringLiteral)
{
    TestMap map;
    map.insert("key", 42);
    int* ptr = map.at_ptr("key");
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

// find() with an lvalue string key must hit the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, FindLvalueString)
{
    TestMap map;
    map.insert("target", 99);
    std::string key = "target";
    EXPECT_NE(map.find(key), map.end());
}

// find() with an rvalue string key must hit the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, FindRvalueString)
{
    TestMap map;
    map.insert("target", 99);
    EXPECT_NE(map.find(std::string("target")), map.end());
}

// find() with a string literal key must hit the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, FindStringLiteral)
{
    TestMap map;
    map.insert("target", 99);
    EXPECT_NE(map.find("target"), map.end());
}

// contains() with lvalue strings must accept hits and misses.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, ContainsLvalueString)
{
    TestMap map;
    map.insert("present", 1);
    std::string key = "present";
    EXPECT_TRUE(map.contains(key));
    std::string missing = "absent";
    EXPECT_FALSE(map.contains(missing));
}

// contains() with rvalue strings must accept hits and misses.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, ContainsRvalueString)
{
    TestMap map;
    map.insert("present", 1);
    EXPECT_TRUE(map.contains(std::string("present")));
    EXPECT_FALSE(map.contains(std::string("absent")));
}

// contains() with string literals must accept hits and misses.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, ContainsStringLiteral)
{
    TestMap map;
    map.insert("present", 1);
    EXPECT_TRUE(map.contains("present"));
    EXPECT_FALSE(map.contains("absent"));
}

// build_add with lvalue string keys must sort them.
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

// build_add with rvalue string keys must move and sort them.
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

// build_add with string literal keys must convert and sort them.
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

// Const find() with an lvalue string key must hit; a missing rvalue
// must miss.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, ConstFindLvalueString)
{
    TestMap map;
    map.insert("key", 7);
    const TestMap& cmap = map;
    std::string k = "key";
    EXPECT_NE(cmap.find(k), cmap.end());
    EXPECT_EQ(cmap.find(std::string("nonexistent")), cmap.end());
}

// Const at_ref with a string literal must read the entry.
TEST_F(FlatKeyOrderMapFwdStringKeyTest, ConstAtRefStringLiteral)
{
    TestMap map;
    map.insert("val", 42);
    const TestMap& cmap = map;
    EXPECT_EQ(cmap.at_ref("val"), 42);
}

// Const at_ptr with an rvalue string key must find the entry.
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

class FlatKeyOrderMapFwdStringValueTest : public FlatKeyOrderMapTestBase<int, std::string> {};

// insert() with an lvalue string value must copy it in.
TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertLvalueString)
{
    TestMap map;
    std::string val = "hello";
    map.insert(1, val);

    EXPECT_EQ(map[1], "hello");
}

// insert() with a const lvalue string value must compile and copy.
TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertConstLvalueString)
{
    TestMap map;
    const std::string val = "world";
    map.insert(2, val);

    EXPECT_EQ(map[2], "world");
}

// insert() with an rvalue string value must move it in.
TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertRvalueString)
{
    TestMap map;
    map.insert(3, std::string("temp"));

    EXPECT_EQ(map[3], "temp");
}

// insert() with a string literal value must convert and store.
TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertStringLiteral)
{
    TestMap map;
    map.insert(4, "literal");

    EXPECT_EQ(map[4], "literal");
}

// insert() via const char* lvalue value must convert and store.
TEST_F(FlatKeyOrderMapFwdStringValueTest, InsertConstCharPointer)
{
    TestMap map;
    const char* ptr = "cstring";
    map.insert(5, ptr);

    EXPECT_EQ(map[5], "cstring");
}

// build_add with lvalue string values must copy them in.
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

// build_add with rvalue string values must move them in.
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

// build_add with string literal values must convert and store.
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

class FlatKeyOrderMapFwdStringBothTest : public FlatKeyOrderMapTestBase<std::string, std::string> {};

// insert(lvalue key, lvalue value).
TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLvalueLvalue)
{
    TestMap map;
    std::string k = "key", v = "val";
    map.insert(k, v);
    EXPECT_EQ(map["key"], "val");
}

// insert(lvalue key, rvalue value).
TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLvalueRvalue)
{
    TestMap map;
    std::string k = "key";
    map.insert(k, std::string("val"));
    EXPECT_EQ(map["key"], "val");
}

// insert(rvalue key, lvalue value).
TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertRvalueLvalue)
{
    TestMap map;
    std::string v = "val";
    map.insert(std::string("key"), v);
    EXPECT_EQ(map["key"], "val");
}

// insert(rvalue key, rvalue value).
TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertRvalueRvalue)
{
    TestMap map;
    map.insert(std::string("key"), std::string("val"));
    EXPECT_EQ(map["key"], "val");
}

// insert(literal key, literal value).
TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLiteralLiteral)
{
    TestMap map;
    map.insert("key", "val");
    EXPECT_EQ(map["key"], "val");
}

// insert(literal key, lvalue value).
TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLiteralLvalue)
{
    TestMap map;
    std::string v = "val";
    map.insert("key", v);
    EXPECT_EQ(map["key"], "val");
}

// insert(literal key, rvalue value).
TEST_F(FlatKeyOrderMapFwdStringBothTest, InsertLiteralRvalue)
{
    TestMap map;
    map.insert("key", std::string("val"));
    EXPECT_EQ(map["key"], "val");
}

// build_add mixing lvalue, moved, converted keys and values must sort
// everything.
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

// find/contains across every argument category must agree.
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

// at_ref across every argument category must read the same entry.
TEST_F(FlatKeyOrderMapFwdStringBothTest, AtRefAllCategories)
{
    TestMap map;
    map.insert("key", "val");

    std::string lk = "key";
    EXPECT_EQ(map.at_ref(lk), "val");
    EXPECT_EQ(map.at_ref(std::string("key")), "val");
    EXPECT_EQ(map.at_ref("key"), "val");
}

// Const at_ptr across every argument category must find the entry.
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

// Interleaved insert/build/insert with mixed categories must yield one
// sorted map containing all five entries.
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

// Asserts exact construction counts; `line` carries the caller's
// __LINE__ so failures point at the right statement.
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

class FlatKeyOrderMapTrackedTest : public FlatKeyOrderMapTestBase<TrackedKey, int>
{
protected:
    void SetUp() override
    {
        TrackedKey::ResetCounts();
    }
};

// insert(lvalue TrackedKey) must COPY exactly once, no moves.
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

// insert(rvalue TrackedKey) must MOVE exactly once, no copies.
TEST_F(FlatKeyOrderMapTrackedTest, InsertTrackedKeyRvalue)
{
    TestMap map;
    TrackedKey key("hello");
    TrackedKey::ResetCounts();

    map.insert(std::move(key), 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(0, 0, 1, __LINE__);
}

// insert(string literal) must construct the stored key directly once.
TEST_F(FlatKeyOrderMapTrackedTest, InsertStringLiteral)
{
    TestMap map;
    TrackedKey::ResetCounts();

    map.insert("hello", 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(1, 0, 0, __LINE__);
}

// insert(rvalue std::string) must construct the key directly once.
TEST_F(FlatKeyOrderMapTrackedTest, InsertStdStringRvalue)
{
    TestMap map;
    TrackedKey::ResetCounts();

    map.insert(std::string("hello"), 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(1, 0, 0, __LINE__);
}

// insert(lvalue std::string) must construct the key directly once.
TEST_F(FlatKeyOrderMapTrackedTest, InsertStdStringLvalue)
{
    TestMap map;
    std::string s("hello");
    TrackedKey::ResetCounts();

    map.insert(s, 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(1, 0, 0, __LINE__);
}

// insert(const lvalue std::string) must construct the key directly.
TEST_F(FlatKeyOrderMapTrackedTest, InsertConstStdStringLvalue)
{
    TestMap map;
    const std::string s("hello");
    TrackedKey::ResetCounts();

    map.insert(s, 1);

    EXPECT_EQ(map.size(), 1);
    ExpectCounts(1, 0, 0, __LINE__);
}

// find(lvalue) must construct ZERO keys.
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

// find(rvalue) must construct ZERO keys.
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

// find(literal) must construct ZERO keys.
TEST_F(FlatKeyOrderMapTrackedTest, FindStringLiteralZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey::ResetCounts();

    auto it = map.find("key");
    EXPECT_NE(it, map.end());
    ExpectCounts(0, 0, 0, __LINE__);
}

// find(rvalue std::string) must construct ZERO keys.
TEST_F(FlatKeyOrderMapTrackedTest, FindStdStringRvalueZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey::ResetCounts();

    auto it = map.find(std::string("key"));
    EXPECT_NE(it, map.end());
    ExpectCounts(0, 0, 0, __LINE__);
}

// contains() across all categories must construct ZERO keys.
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

// at_ref across all categories must construct ZERO keys.
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

// at_ptr across all categories must construct ZERO keys.
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

// Const find(rvalue) must construct ZERO keys.
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

// operator[] on an EXISTING key must construct ZERO keys.
TEST_F(FlatKeyOrderMapTrackedTest, OperatorBracketExistingZeroConstructions)
{
    TestMap map;
    map.insert("key", 42);
    TrackedKey::ResetCounts();

    EXPECT_EQ(map["key"], 42);
    ExpectCounts(0, 0, 0, __LINE__);
}

// operator[] creating a NEW key must construct the key exactly once.
TEST_F(FlatKeyOrderMapTrackedTest, OperatorBracketNewKeyOneConstruction)
{
    TestMap map;
    TrackedKey::ResetCounts();

    map["new_key"] = 99;

    EXPECT_EQ(map["new_key"], 99);
    ExpectCounts(1, 0, 0, __LINE__);
}

// Two literal build_adds: 2 direct constructions plus 1 reallocation
// move. Sorted afterwards, which must not construct more keys.
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

// Two lvalue build_adds: 2 copies plus 1 reallocation move.
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

// Two rvalue build_adds: 2 moves plus 1 reallocation move.
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

// Growth ladder: each realloc must MOVE existing keys exactly once;
// lookups afterwards must construct nothing.
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

// After a completed build, find() must construct ZERO keys.
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

// Write via at_ptr then read via find must construct ZERO keys.
TEST_F(FlatKeyOrderMapTrackedTest, AtPtrModifyThenReadNoExtraConstructions)
{
    TestMap map;
    map.insert("data", 0);
    TrackedKey::ResetCounts();

    *map.at_ptr("data") = 77;
    EXPECT_EQ(map.find("data")->second, 77);
    ExpectCounts(0, 0, 0, __LINE__);
}

// Rebuilding after clear() must construct the new key exactly once.
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

// Copying the map must copy each stored key exactly once.
TEST_F(FlatKeyOrderMapTrackedTest, CopyMapNoAdditionalKeyConstructions)
{
    TestMap map;
    map.insert("a", 1);
    map.insert("b", 2);
    TrackedKey::ResetCounts();

    TestMap copied(map);

    ExpectCounts(0, 2, 0, __LINE__);
}

// Moving the map must transfer ownership without touching keys.
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
