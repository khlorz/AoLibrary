/********************************************************************
* Cyclic buffer tests: fixed and dynamic
********************************************************************/


#include "pch.h"
#include "config.h"
#if AOL_TEST_CONTAINERS_CYCLICBUFFER

#include "aol/cyclic_buffer.h"
#include "aol/utilities.h"
#include "aol/algorithms.h"

#include <bit>
#include <deque>
#include <iterator>
#include <memory>
#include <string>


namespace
{

// ===================================================================
// CYCLIC BUFFER FIXED TESTS
// ===================================================================

class CyclicBufferFixedTest : public ::testing::Test
{
protected:
    static constexpr AoL::SizeT buffer_size = 8;
    using TestBuffer = AoL::CyclicBufferF<int, buffer_size>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

}

TEST_F(CyclicBufferFixedTest, PushAndSize)
{
    TestBuffer buf;
    buf.push_back(10);
    buf.push_back(20);
    buf.push_back(30);

    EXPECT_EQ(buf.size(), 3);
}

TEST_F(CyclicBufferFixedTest, FIFOOrder)
{
    TestBuffer buf;
    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);

    EXPECT_EQ(buf.front(), 1);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 2);
    buf.pop_front();

    EXPECT_EQ(buf.front(), 3);
    buf.pop_front();

    EXPECT_TRUE(buf.empty());
}

TEST_F(CyclicBufferFixedTest, Overflow)
{
    TestBuffer buf;
    for (int i = 0; i < 8; ++i)
    {
        buf.push_back(i);
    }
    EXPECT_EQ(buf.size(), 8);
    EXPECT_TRUE(buf.full());

    buf.push_back(99);
    EXPECT_EQ(buf.size(), 8);

    EXPECT_EQ(buf.front(), 1);
}

TEST_F(CyclicBufferFixedTest, Empty)
{
    TestBuffer buf;
    EXPECT_TRUE(buf.empty());

    buf.push_back(5);
    EXPECT_FALSE(buf.empty());

    buf.pop_front();
    EXPECT_TRUE(buf.empty());
}

TEST_F(CyclicBufferFixedTest, BackAccess)
{
    TestBuffer buf;
    buf.push_back(10);
    buf.push_back(20);
    buf.push_back(30);

    EXPECT_EQ(buf.back(), 30);

    buf.pop_back();
    EXPECT_EQ(buf.back(), 20);
}

TEST_F(CyclicBufferFixedTest, Clear)
{
    TestBuffer buf;
    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);

    EXPECT_FALSE(buf.empty());

    buf.clear();
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), 0);
}

TEST_F(CyclicBufferFixedTest, RandomAccess)
{
    TestBuffer buf;

    for (int i = 0; i < 5; ++i)
    {
        buf.push_back(i * 10);
    }

    EXPECT_EQ(buf[0], 0);
    EXPECT_EQ(buf[2], 20);
    EXPECT_EQ(buf[4], 40);
}

TEST_F(CyclicBufferFixedTest, ForwardIteration)
{
    TestBuffer buf;

    for (int i = 1; i <= 4; ++i)
    {
        buf.push_back(i);
    }

    int sum = 0;

    for (auto it = buf.begin(); it != buf.end(); ++it)
    {
        sum += *it;
    }

    EXPECT_EQ(sum, 10);
}

// ===================================================================
// CYCLIC BUFFER DYNAMIC TESTS
// ===================================================================

class CyclicBufferDynamicTest : public ::testing::Test
{
protected:
    using TestBuffer = AoL::CyclicBufferD<int>;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(CyclicBufferDynamicTest, GrowsAsNeeded)
{
    TestBuffer buf(128);
    for (int i = 0; i < 100; ++i)
    {
        buf.push_back(i);
    }
    EXPECT_EQ(buf.size(), 100);
}

TEST_F(CyclicBufferDynamicTest, FIFOOrder)
{
    TestBuffer buf(16);
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

TEST_F(CyclicBufferDynamicTest, Overflow)
{
    TestBuffer buf(8);

    for (int i = 0; i < 10; ++i)
    {
        buf.push_back(i);
    }

    EXPECT_EQ(buf.size(), 8);
    EXPECT_TRUE(buf.full());

    EXPECT_EQ(buf.front(), 2);
}

TEST_F(CyclicBufferDynamicTest, EmplaceBack)
{
    TestBuffer buf(4);
    buf.emplace_back(42);
    buf.emplace_back(99);

    EXPECT_EQ(buf.size(), 2);
    EXPECT_EQ(buf[0], 42);
    EXPECT_EQ(buf[1], 99);
}

TEST_F(CyclicBufferDynamicTest, BackAndPopBack)
{
    TestBuffer buf(8);
    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);

    EXPECT_EQ(buf.back(), 3);

    buf.pop_back();
    EXPECT_EQ(buf.size(), 2);
    EXPECT_EQ(buf.back(), 2);
}

TEST_F(CyclicBufferDynamicTest, Clear)
{
    TestBuffer buf(8);
    buf.push_back(10);
    buf.push_back(20);

    EXPECT_FALSE(buf.empty());

    buf.clear();
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), 0);
}

TEST_F(CyclicBufferDynamicTest, RandomAccess)
{
    TestBuffer buf(8);

    for (int i = 0; i < 6; ++i)
    {
        buf.push_back(i * 5);
    }

    EXPECT_EQ(buf[0], 0);
    EXPECT_EQ(buf[3], 15);
}

TEST_F(CyclicBufferDynamicTest, ForwardIteration)
{
    TestBuffer buf(8);

    for (int i = 1; i <= 3; ++i)
    {
        buf.push_back(i * 10);
    }

    int sum = 0;

    for (auto it = buf.begin(); it != buf.end(); ++it)
    {
        sum += *it;
    }

    EXPECT_EQ(sum, 60);
}

TEST_F(CyclicBufferDynamicTest, IncreaseThenDecreaseCapacity)
{
    TestBuffer buf(4);
    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);
    buf.push_back(4);

    EXPECT_TRUE(buf.full());

    buf.increase_capacity(8);

    buf.push_back(5);
    buf.push_back(6);
    buf.push_back(7);
    buf.push_back(8);

    EXPECT_TRUE(buf.full());
    EXPECT_EQ(buf.capacity(), 8);
    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[7], 8);

    buf.decrease_capacity(4);
    EXPECT_EQ(buf.capacity(), 4);
    EXPECT_EQ(buf.size(), 4);
    EXPECT_EQ(buf[0], 1);
}

// ===================================================================
// EXHAUSTIVE SUITE: HELPERS
// ===================================================================

namespace
{

struct CbPair2
{
    int a;
    int b;

    CbPair2() :
        a{},
        b{}
    {}

    CbPair2(int x, int y) :
        a{ x },
        b{ y }
    {
    }
};

struct CbElem
{
    int x = 0;
};

class CbLiveObj
{
public:
    static inline int live = 0;

    int v;

    CbLiveObj() :
        v{ 0 }
    {
        ++live;
    }

    explicit CbLiveObj(int x) :
        v{ x }
    {
        ++live;
    }

    CbLiveObj(const CbLiveObj& other) :
        v{ other.v }
    {
        ++live;
    }

    CbLiveObj(CbLiveObj&& other) noexcept :
        v{ other.v }
    {
        ++live;
    }

    CbLiveObj& operator = (const CbLiveObj& other) = default;
    CbLiveObj& operator = (CbLiveObj&& other) noexcept = default;

    ~CbLiveObj()
    {
        --live;
    }
};

struct CbRng
{
    AoL::U64 s;

    explicit CbRng(AoL::U64 seed) :
        s{ seed * 6364136223846793005ULL + 1442695040888963407ULL }
    {
    }

    AoL::U32 next()
    {
        s = s * 6364136223846793005ULL + 1442695040888963407ULL;
        return static_cast<AoL::U32>(s >> 33);
    }

    AoL::U32 below(AoL::U32 n)
    {
        return next() % n;
    }
};

template<typename BufT>
void ExpectMatchesDeque(const BufT& buf, const std::deque<int>& ref)
{
    ASSERT_EQ(buf.size(), ref.size());
    for (size_t i = 0; i < ref.size(); ++i)
    {
        ASSERT_EQ(buf[i], ref[i]) << "mismatch at logical index " << i;
    }
}

}

// ===================================================================
// EXHAUSTIVE SUITE: TYPE-LEVEL CHECKS
// ===================================================================

TEST(CyclicBufferTypeTest, TypeTraitsAndIteratorCategories)
{
    using FixedBuf = AoL::CyclicBufferF<int, 8>;
    using DynBuf = AoL::CyclicBufferD<int>;

    static_assert(std::is_same_v<typename FixedBuf::value_type, int>);
    static_assert(std::is_same_v<typename DynBuf::value_type, int>);
    static_assert(std::is_same_v<typename FixedBuf::iterator::iterator_category, std::random_access_iterator_tag>);
    static_assert(std::is_same_v<typename FixedBuf::const_iterator::iterator_category, std::random_access_iterator_tag>);
    static_assert(std::random_access_iterator<typename FixedBuf::iterator>);
    static_assert(std::random_access_iterator<typename FixedBuf::const_iterator>);
    static_assert(std::random_access_iterator<typename DynBuf::iterator>);
    static_assert(std::random_access_iterator<typename DynBuf::const_iterator>);

    SUCCEED();
}

// ===================================================================
// EXHAUSTIVE SUITE: FIXED CONSTRUCTION AND STATE
// ===================================================================

TEST(CyclicBufferFixedExTest, DefaultInitialState)
{
    AoL::CyclicBufferF<int, 8> buf;

    EXPECT_TRUE(buf.empty());
    EXPECT_FALSE(buf.full());
    EXPECT_EQ(buf.size(), (AoL::SizeT)0);
    EXPECT_EQ(buf.capacity(), (AoL::SizeT)8);
    EXPECT_NE(buf.data(), nullptr);
}

TEST(CyclicBufferFixedExTest, CapacityTwoWrapCycle)
{
    AoL::CyclicBufferF<int, 2> buf;

    buf.push_back(1);
    EXPECT_EQ(buf.front(), 1);

    buf.push_back(2);
    EXPECT_EQ(buf.front(), 1);
    EXPECT_EQ(buf.back(), 2);

    buf.push_back(3);
    EXPECT_EQ(buf.size(), (AoL::SizeT)2);
    EXPECT_EQ(buf[0], 2);
    EXPECT_EQ(buf[1], 3);

    buf.push_back(4);
    EXPECT_EQ(buf[0], 3);
    EXPECT_EQ(buf[1], 4);

    buf.push_back(5);
    EXPECT_EQ(buf[0], 4);
    EXPECT_EQ(buf[1], 5);
    EXPECT_EQ(buf.front(), 4);
}

TEST(CyclicBufferFixedExTest, FullOnlyAfterCapacityPushes)
{
    AoL::CyclicBufferF<int, 4> buf;

    for (int i = 1; i <= 3; ++i)
    {
        buf.push_back(i);
        EXPECT_FALSE(buf.full());
    }

    buf.push_back(4);
    EXPECT_TRUE(buf.full());

    buf.pop_front();
    EXPECT_FALSE(buf.full());

    buf.push_back(5);
    EXPECT_TRUE(buf.full());
}

// ===================================================================
// EXHAUSTIVE SUITE: FIXED PUSH AND EMPLACE
// ===================================================================

TEST(CyclicBufferFixedExTest, PushBackValueCategories)
{
    AoL::CyclicBufferF<std::string, 4> buf;

    std::string lv = "lvalue";
    const std::string clv = "const_lvalue";

    buf.push_back(lv);
    buf.push_back(clv);
    buf.push_back(std::move(lv));
    buf.push_back("temporary");

    EXPECT_EQ(buf[0], "lvalue");
    EXPECT_EQ(buf[1], "const_lvalue");
    EXPECT_EQ(buf[2], "lvalue");
    EXPECT_EQ(buf[3], "temporary");
}

TEST(CyclicBufferFixedExTest, EmplaceBackMultiArg)
{
    AoL::CyclicBufferF<CbPair2, 4> buf;

    buf.emplace_back(11, 22);
    buf.emplace_back(33, 44);

    EXPECT_EQ(buf.size(), (AoL::SizeT)2);
    EXPECT_EQ(buf[0].a, 11);
    EXPECT_EQ(buf[0].b, 22);
    EXPECT_EQ(buf[1].a, 33);
    EXPECT_EQ(buf[1].b, 44);
}

TEST(CyclicBufferFixedExTest, EmplaceBackReturnsReferenceToNewest)
{
    AoL::CyclicBufferF<int, 4> buf;

    int& r = buf.emplace_back(41);
    EXPECT_EQ(&r, &buf[0]);

    r = 50;
    EXPECT_EQ(buf[0], 50);

    buf.emplace_back(1);
    buf.emplace_back(2);
    buf.emplace_back(3);

    int& rf = buf.emplace_back(77);
    EXPECT_EQ(rf, 77);
    EXPECT_EQ(buf.back(), 77);
    EXPECT_EQ(buf.front(), 1);
    EXPECT_EQ(buf.size(), (AoL::SizeT)4);
}

// ===================================================================
// EXHAUSTIVE SUITE: FIXED OVERFLOW AND WRAPAROUND
// ===================================================================

TEST(CyclicBufferFixedExTest, SlidingWindowExactContent)
{
    AoL::CyclicBufferF<int, 8> buf;
    std::deque<int> ref;

    for (int v = 0; v < 40; ++v)
    {
        buf.push_back(v);
        ref.push_back(v);
        if ((int)ref.size() > 8)
        {
            ref.pop_front();
        }
        ExpectMatchesDeque(buf, ref);
    }
}

TEST(CyclicBufferFixedExTest, InterleavedPopFrontPushModelChecked)
{
    AoL::CyclicBufferF<int, 8> buf;
    std::deque<int> ref;
    int v = 0;

    for (int step = 0; step < 200; ++step)
    {
        if (step % 4 != 3)
        {
            if ((int)ref.size() == 8)
            {
                ref.pop_front();
            }
            buf.push_back(v);
            ref.push_back(v);
            ++v;
        }
        else if (!ref.empty())
        {
            buf.pop_front();
            ref.pop_front();
        }
        ExpectMatchesDeque(buf, ref);
    }
}

// ===================================================================
// EXHAUSTIVE SUITE: FIXED POPS
// ===================================================================

TEST(CyclicBufferFixedExTest, PopFrontDrainRefillCycles)
{
    AoL::CyclicBufferF<int, 8> buf;

    for (int cycle = 0; cycle < 3; ++cycle)
    {
        for (int i = 1; i <= 5; ++i)
        {
            buf.push_back(cycle * 100 + i);
        }
        for (int i = 1; i <= 5; ++i)
        {
            ASSERT_EQ(buf.front(), cycle * 100 + i);
            ASSERT_EQ(buf.back(), cycle * 100 + 5);
            buf.pop_front();
        }
        EXPECT_TRUE(buf.empty());
    }
}

TEST(CyclicBufferFixedExTest, PopBackThenPushOverwritesLastSlot)
{
    AoL::CyclicBufferF<int, 8> buf;

    for (int i = 1; i <= 4; ++i)
    {
        buf.push_back(i);
    }

    buf.pop_back();
    EXPECT_EQ(buf.size(), (AoL::SizeT)3);
    EXPECT_EQ(buf.back(), 3);

    buf.push_back(99);
    EXPECT_EQ(buf.size(), (AoL::SizeT)4);
    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[1], 2);
    EXPECT_EQ(buf[2], 3);
    EXPECT_EQ(buf[3], 99);
    EXPECT_EQ(buf.back(), 99);
}

TEST(CyclicBufferFixedExTest, PopBackToEmptyThenReuse)
{
    AoL::CyclicBufferF<int, 8> buf;

    buf.push_back(7);
    buf.push_back(8);
    buf.pop_back();
    buf.pop_back();
    EXPECT_TRUE(buf.empty());

    buf.push_back(42);
    EXPECT_EQ(buf.size(), (AoL::SizeT)1);
    EXPECT_EQ(buf.front(), 42);
    EXPECT_EQ(buf.back(), 42);
}

// ===================================================================
// EXHAUSTIVE SUITE: FIXED ELEMENT ACCESS
// ===================================================================

TEST(CyclicBufferFixedExTest, SubscriptLogicalOrderAfterWrap)
{
    AoL::CyclicBufferF<int, 8> buf;

    for (int i = 0; i < 16; ++i)
    {
        buf.push_back(i);
    }

    EXPECT_EQ(buf.size(), (AoL::SizeT)8);
    for (int i = 0; i < 8; ++i)
    {
        EXPECT_EQ(buf[i], 8 + i) << "index " << i;
    }
    EXPECT_EQ(buf.front(), 8);
    EXPECT_EQ(buf.back(), 15);
}

TEST(CyclicBufferFixedExTest, ConstElementAccess)
{
    AoL::CyclicBufferF<int, 8> buf_m;

    for (int i = 0; i < 6; ++i)
    {
        buf_m.push_back(i * 3);
    }

    const AoL::CyclicBufferF<int, 8>& buf = buf_m;

    EXPECT_EQ(buf[0], 0);
    EXPECT_EQ(buf[2], 6);
    EXPECT_EQ(buf[5], 15);
    EXPECT_EQ(buf.front(), 0);
    EXPECT_EQ(buf.back(), 15);

    const int* p = buf.data();
    EXPECT_EQ(p[0], 0);
}

TEST(CyclicBufferFixedExTest, MutateThroughAccessors)
{
    AoL::CyclicBufferF<int, 8> buf;

    for (int i = 1; i <= 5; ++i)
    {
        buf.push_back(i * 10);
    }

    buf[1] = 99;
    buf.front() = -1;
    buf.back() = 77;

    EXPECT_EQ(buf[0], -1);
    EXPECT_EQ(buf[1], 99);
    EXPECT_EQ(buf[2], 30);
    EXPECT_EQ(buf[3], 40);
    EXPECT_EQ(buf[4], 77);
}

TEST(CyclicBufferFixedExTest, DataPointerBasic)
{
    AoL::CyclicBufferF<int, 4> buf;

    buf.push_back(5);
    buf.push_back(6);

    int* p = buf.data();
    EXPECT_NE(p, nullptr);
    EXPECT_EQ(p[0], 5);
    EXPECT_EQ(p[1], 6);
}

// ===================================================================
// EXHAUSTIVE SUITE: FIXED ITERATORS
// ===================================================================

TEST(CyclicBufferFixedIterTest, ForwardTraversalAllPhases)
{
    using Buf = AoL::CyclicBufferF<int, 8>;

    auto sum_of = [](const Buf& b)
    {
        int sum = 0;
        for (int v : b)
        {
            sum += v;
        }
        return sum;
    };

    Buf buf;
    EXPECT_EQ(sum_of(buf), 0);

    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);
    EXPECT_EQ(sum_of(buf), 6);

    for (int i = 4; i <= 8; ++i)
    {
        buf.push_back(i);
    }
    EXPECT_EQ(sum_of(buf), 36);

    buf.push_back(9);
    buf.push_back(10);
    EXPECT_EQ(sum_of(buf), 52);

    for (int i = 0; i < 8; ++i)
    {
        EXPECT_EQ(buf[i], 3 + i);
    }
}

TEST(CyclicBufferFixedIterTest, RangeForAndConstRangeFor)
{
    AoL::CyclicBufferF<int, 8> buf_m;

    for (int i = 1; i <= 4; ++i)
    {
        buf_m.push_back(i);
    }

    int sum_m = 0;
    for (int& v : buf_m)
    {
        sum_m += v;
        v += 100;
    }
    EXPECT_EQ(sum_m, 10);
    EXPECT_EQ(buf_m[0], 101);
    EXPECT_EQ(buf_m[3], 104);

    const AoL::CyclicBufferF<int, 8>& buf_c = buf_m;
    int sum_c = 0;
    for (int v : buf_c)
    {
        sum_c += v;
    }
    EXPECT_EQ(sum_c, 414);
}

TEST(CyclicBufferFixedIterTest, ConstIteratorTraversal)
{
    AoL::CyclicBufferF<int, 8> buf_m;

    for (int i = 10; i <= 60; i += 10)
    {
        buf_m.push_back(i);
    }

    const AoL::CyclicBufferF<int, 8>& buf = buf_m;

    int sum = 0;
    for (auto it = buf.cbegin(); it != buf.cend(); ++it)
    {
        sum += *it;
    }
    EXPECT_EQ(sum, 210);

    auto d = std::distance(buf.cbegin(), buf.cend());
    EXPECT_EQ(d, 6);
}

TEST(CyclicBufferFixedIterTest, ReverseIterators)
{
    AoL::CyclicBufferF<int, 8> buf;

    for (int i = 1; i <= 5; ++i)
    {
        buf.push_back(i * 7);
    }

    EXPECT_EQ(*buf.rbegin(), 35);

    int sum = 0;
    for (auto it = buf.rbegin(); it != buf.rend(); ++it)
    {
        sum += *it;
    }
    EXPECT_EQ(sum, 105);

    int csum = 0;
    for (auto it = buf.crbegin(); it != buf.crend(); ++it)
    {
        csum += *it;
    }
    EXPECT_EQ(csum, 105);

    EXPECT_EQ(std::distance(buf.rbegin(), buf.rend()), 5);
}

TEST(CyclicBufferFixedIterTest, IteratorArithmeticExhaustive)
{
    AoL::CyclicBufferF<int, 8> buf;

    for (int i = 1; i <= 7; ++i)
    {
        buf.push_back(i * 10);
    }

    auto it = buf.begin();

    auto it3 = it + 3;
    EXPECT_EQ(*it3, 40);

    auto it3b = 3 + it;
    EXPECT_TRUE(it3 == it3b);

    EXPECT_EQ(*(it3 - 2), 20);
    EXPECT_EQ(it3[-1], 20);
    EXPECT_EQ(it[5], 60);
    EXPECT_EQ(it3 - it, 3);
    EXPECT_EQ(it - it3, -3);

    auto post = it++;
    EXPECT_EQ(*post, 10);
    EXPECT_EQ(*it, 20);

    --it;
    EXPECT_EQ(*it, 10);

    ++it;
    EXPECT_EQ(*it, 20);

    auto postd = it--;
    EXPECT_EQ(*postd, 20);
    EXPECT_EQ(*it, 10);

    ++it;

    auto seek = it;
    seek += 4;
    EXPECT_EQ(*seek, 60);

    seek -= 2;
    EXPECT_EQ(*seek, 40);

    EXPECT_TRUE(it < it3);
    EXPECT_TRUE(it <= it3);
    EXPECT_TRUE(it3 > it);
    EXPECT_TRUE(it3 >= it);
    EXPECT_TRUE(it != it3);
    EXPECT_TRUE(it == it);
    EXPECT_TRUE((it <=> it3) < 0);
    EXPECT_TRUE((it3 <=> it) > 0);

    auto e = buf.end();
    EXPECT_EQ(e - buf.begin(), 7);
    EXPECT_TRUE(buf.begin() + 7 == e);
    EXPECT_TRUE(e > it3);
}

TEST(CyclicBufferFixedIterTest, IteratorArrowOperator)
{
    AoL::CyclicBufferF<CbElem, 4> buf;

    buf.push_back(CbElem{ 7 });
    buf.push_back(CbElem{ 9 });

    auto it = buf.begin();
    EXPECT_EQ(it->x, 7);

    ++it;
    EXPECT_EQ(it->x, 9);

    it->x = 12;
    EXPECT_EQ(buf[1].x, 12);
}

TEST(CyclicBufferFixedIterTest, DefaultConstructedIteratorsEqual)
{
    using It = AoL::CyclicBufferF<int, 4>::iterator;

    It a;
    It b;

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(CyclicBufferFixedIterTest, StdAlgorithmInterop)
{
    AoL::CyclicBufferF<int, 8> buf;

    for (int i = 1; i <= 5; ++i)
    {
        buf.push_back(i * 10);
    }

    auto found = std::find(buf.begin(), buf.end(), 30);
    EXPECT_TRUE(found != buf.end());
    EXPECT_EQ(*found, 30);

    EXPECT_EQ(std::count_if(buf.begin(), buf.end(), [](int v) { return v > 25; }), 3);
    EXPECT_EQ(std::distance(buf.begin(), buf.end()), 5);

    auto fb = AoL::FindBrute(buf.begin(), buf.end(), 40);
    EXPECT_TRUE(fb != buf.end());
    EXPECT_EQ(*fb, 40);
}

TEST(CyclicBufferFixedIterTest, EmptyBufferIterationNoOp)
{
    AoL::CyclicBufferF<int, 8> buf;

    EXPECT_TRUE(buf.begin() == buf.end());
    EXPECT_EQ(std::distance(buf.begin(), buf.end()), 0);

    int visits = 0;
    for (int v : buf)
    {
        (void)v;
        ++visits;
    }
    EXPECT_EQ(visits, 0);

    const AoL::CyclicBufferF<int, 8>& cb = buf;
    EXPECT_TRUE(cb.begin() == cb.end());
    for (int v : cb)
    {
        (void)v;
        ++visits;
    }
    EXPECT_EQ(visits, 0);
}

// ===================================================================
// EXHAUSTIVE SUITE: FIXED COPY AND MOVE
// ===================================================================

TEST(CyclicBufferFixedCopyMoveTest, CopyIsIndependentWhenWrapped)
{
    std::deque<int> ref;
    AoL::CyclicBufferF<int, 8> orig;

    for (int i = 1; i <= 10; ++i)
    {
        orig.push_back(i);
        ref.push_back(i);
        if ((int)ref.size() > 8)
        {
            ref.pop_front();
        }
    }

    auto copy = orig;
    ExpectMatchesDeque(copy, ref);

    copy[0] = 999;

    ExpectMatchesDeque(orig, ref);
    EXPECT_EQ(copy[0], 999);

    copy.push_back(11);
    EXPECT_EQ(copy.back(), 11);
    EXPECT_EQ(copy[0], 4);
}

TEST(CyclicBufferFixedCopyMoveTest, MoveTransfersStateAndEmptiesSource)
{
    AoL::CyclicBufferF<int, 8> src;

    for (int i = 1; i <= 6; ++i)
    {
        src.push_back(i);
    }

    AoL::CyclicBufferF<int, 8> dst(std::move(src));

    EXPECT_EQ(dst.size(), (AoL::SizeT)6);
    for (int i = 0; i < 6; ++i)
    {
        EXPECT_EQ(dst[i], i + 1);
    }

    EXPECT_TRUE(src.empty());
    EXPECT_EQ(src.size(), (AoL::SizeT)0);
}

TEST(CyclicBufferFixedCopyMoveTest, CopyAssignAndMoveAssign)
{
    AoL::CyclicBufferF<int, 8> a;
    for (int i = 1; i <= 9; ++i)
    {
        a.push_back(i);
    }

    AoL::CyclicBufferF<int, 8> b;
    b.push_back(500);
    b = a;

    EXPECT_EQ(b.size(), a.size());
    for (unsigned i = 0; i < 8; ++i)
    {
        EXPECT_EQ(b[i], 2 + i);
    }

    AoL::CyclicBufferF<int, 8> c;
    c.push_back(700);
    c = std::move(b);

    EXPECT_EQ(c.size(), (AoL::SizeT)8);
    for (unsigned i = 0; i < 8; ++i)
    {
        EXPECT_EQ(c[i], 2 + i);
    }
    EXPECT_TRUE(b.empty());
}

// ===================================================================
// EXHAUSTIVE SUITE: FIXED CLEAR
// ===================================================================

TEST(CyclicBufferFixedClearTest, ClearKeepsCapacityAndReusable)
{
    AoL::CyclicBufferF<int, 8> buf;

    for (int i = 1; i <= 10; ++i)
    {
        buf.push_back(i);
    }
    EXPECT_FALSE(buf.empty());

    buf.clear();
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), (AoL::SizeT)0);
    EXPECT_EQ(buf.capacity(), (AoL::SizeT)8);

    for (int i = 1; i <= 3; ++i)
    {
        buf.push_back(i * 5);
    }
    EXPECT_EQ(buf.size(), (AoL::SizeT)3);
    EXPECT_EQ(buf[0], 5);
    EXPECT_EQ(buf[1], 10);
    EXPECT_EQ(buf[2], 15);
}

TEST(CyclicBufferFixedClearTest, ClearWhenEmptyOrTwice)
{
    AoL::CyclicBufferF<int, 8> buf;

    buf.clear();
    EXPECT_TRUE(buf.empty());

    buf.clear();
    EXPECT_TRUE(buf.empty());

    buf.push_back(1);
    buf.clear();
    buf.clear();
    EXPECT_TRUE(buf.empty());
}

// ===================================================================
// EXHAUSTIVE SUITE: DYNAMIC CONSTRUCTION AND GROWTH
// ===================================================================

TEST(CyclicBufferDynamicExTest, DefaultHasZeroCapacity)
{
    AoL::CyclicBufferD<int> buf;

    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), (AoL::SizeT)0);
    EXPECT_EQ(buf.capacity(), (AoL::SizeT)0);
}

TEST(CyclicBufferDynamicExTest, GrowthPhaseCapacityTransitions)
{
    AoL::CyclicBufferD<int> buf(8);
    std::deque<int> ref;

    for (int k = 1; k <= 12; ++k)
    {
        buf.push_back(k);
        ref.push_back(k);
        if ((int)ref.size() > 8)
        {
            ref.pop_front();
        }

        EXPECT_EQ(buf.size(), (AoL::SizeT)(k < 8 ? k : 8));
        EXPECT_EQ(buf.capacity(), (AoL::SizeT)(k < 8 ? k : 8));
        ExpectMatchesDeque(buf, ref);
    }
}

TEST(CyclicBufferDynamicExTest, CustomAllocatorInstantiation)
{
    AoL::CyclicBufferD<int, std::allocator<int>> buf(4);

    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);
    buf.push_back(4);
    buf.push_back(5);

    EXPECT_EQ(buf.size(), (AoL::SizeT)4);
    EXPECT_EQ(buf.front(), 2);
    EXPECT_EQ(buf.back(), 5);
}

// ===================================================================
// EXHAUSTIVE SUITE: DYNAMIC CONTENT OPERATIONS
// ===================================================================

TEST(CyclicBufferDynamicExTest, SlidingWindowExactContent)
{
    AoL::CyclicBufferD<int> buf(8);
    std::deque<int> ref;

    for (int v = 0; v < 40; ++v)
    {
        buf.push_back(v);
        ref.push_back(v);
        if ((int)ref.size() > 8)
        {
            ref.pop_front();
        }
        ExpectMatchesDeque(buf, ref);
    }
}

TEST(CyclicBufferDynamicExTest, EmplaceMultiArg)
{
    AoL::CyclicBufferD<CbPair2> buf(4);

    buf.emplace_back(5, 6);
    buf.emplace_back(7, 8);

    EXPECT_EQ(buf[0].a, 5);
    EXPECT_EQ(buf[0].b, 6);
    EXPECT_EQ(buf[1].a, 7);
    EXPECT_EQ(buf[1].b, 8);
}

TEST(CyclicBufferDynamicExTest, PushValueCategories)
{
    AoL::CyclicBufferD<std::string> buf(4);

    std::string lv = "lv";
    const std::string clv = "clv";

    buf.push_back(lv);
    buf.push_back(clv);
    buf.push_back(std::move(lv));
    buf.push_back("tmp");

    EXPECT_EQ(buf[0], "lv");
    EXPECT_EQ(buf[1], "clv");
    EXPECT_EQ(buf[2], "lv");
    EXPECT_EQ(buf[3], "tmp");
}

TEST(CyclicBufferDynamicExTest, StringPipelineWithWrapAndDrain)
{
    AoL::CyclicBufferD<std::string> buf(4);

    buf.push_back("alpha");
    buf.emplace_back("beta");
    buf.push_back("gamma");
    buf.push_back("delta");

    EXPECT_EQ(buf.front(), "alpha");
    EXPECT_EQ(buf.back(), "delta");

    buf.push_back("epsilon");
    EXPECT_EQ(buf.front(), "beta");
    EXPECT_EQ(buf.back(), "epsilon");

    buf.pop_front();
    EXPECT_EQ(buf.front(), "gamma");

    buf.clear();
    EXPECT_TRUE(buf.empty());

    buf.push_back("zeta");
    EXPECT_EQ(buf.front(), "zeta");
    EXPECT_EQ(buf.back(), "zeta");
}

// ===================================================================
// EXHAUSTIVE SUITE: DYNAMIC CLEAR QUIRK
// ===================================================================

TEST(CyclicBufferDynamicExTest, ClearReleasesStorageThenRegrows)
{
    AoL::CyclicBufferD<int> buf(8);

    for (int i = 1; i <= 5; ++i)
    {
        buf.push_back(i);
    }

    buf.clear();
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), (AoL::SizeT)0);
    EXPECT_EQ(buf.capacity(), (AoL::SizeT)0);

    buf.push_back(42);
    EXPECT_EQ(buf.capacity(), (AoL::SizeT)1);

    buf.push_back(43);
    EXPECT_EQ(buf.capacity(), (AoL::SizeT)2);

    EXPECT_EQ(buf[0], 42);
    EXPECT_EQ(buf[1], 43);
}

// ===================================================================
// EXHAUSTIVE SUITE: DYNAMIC CAPACITY MANAGEMENT
// ===================================================================

TEST(CyclicBufferDynamicCapTest, IncreaseUnwrappedPreservesOrder)
{
    AoL::CyclicBufferD<int> buf(8);

    for (int i = 1; i <= 3; ++i)
    {
        buf.push_back(i);
    }

    buf.increase_capacity(16);

    EXPECT_EQ(buf.capacity(), (AoL::SizeT)16);
    EXPECT_EQ(buf.size(), (AoL::SizeT)3);
    EXPECT_EQ(buf.head, (AoL::SizeT)0);
    for (int i = 0; i < 3; ++i)
    {
        ASSERT_EQ(buf[i], i + 1);
    }

    for (int i = 4; i <= 10; ++i)
    {
        buf.push_back(i);
    }
    EXPECT_EQ(buf.size(), (AoL::SizeT)10);
    for (int i = 0; i < 10; ++i)
    {
        ASSERT_EQ(buf[i], i + 1);
    }
}

TEST(CyclicBufferDynamicCapTest, IncreaseWhileWrappedRotatesOrder)
{
    AoL::CyclicBufferD<int> buf(8);

    for (int i = 1; i <= 10; ++i)
    {
        buf.push_back(i);
    }
    ASSERT_EQ(buf.head, (AoL::SizeT)2);

    buf.increase_capacity(16);

    EXPECT_EQ(buf.head, (AoL::SizeT)0);
    EXPECT_EQ(buf.size(), (AoL::SizeT)10);
    for (int i = 0; i < 10; ++i)
    {
        ASSERT_EQ(buf[i], i + 1) << "index " << i;
    }

    for (int i = 11; i <= 18; ++i)
    {
        buf.push_back(i);
    }
    EXPECT_TRUE(buf.full());
    for (int i = 0; i < 16; ++i)
    {
        ASSERT_EQ(buf[i], i + 3);
    }
}

TEST(CyclicBufferDynamicCapTest, ChainedIncreasesWrappedBetween)
{
    AoL::CyclicBufferD<int> buf(2);

    buf.push_back(1);
    buf.push_back(2);
    buf.push_back(3);
    ASSERT_EQ(buf.front(), 2);

    buf.increase_capacity(4);
    ASSERT_EQ(buf[0], 2);
    ASSERT_EQ(buf[1], 3);

    buf.push_back(4);
    buf.push_back(5);
    ASSERT_EQ(buf.front(), 2);
    ASSERT_EQ(buf.back(), 5);

    buf.increase_capacity(16);
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(buf[i], 2 + i);
    }

    for (int i = 6; i <= 16; ++i)
    {
        buf.push_back(i);
    }
    EXPECT_TRUE(buf.full());
    for (int i = 0; i < 16; ++i)
    {
        ASSERT_EQ(buf[i], 2 + i) << "index " << i;
    }
}

TEST(CyclicBufferDynamicCapTest, DecreaseUnwrappedTruncatesEarliest)
{
    AoL::CyclicBufferD<int> buf(8);

    for (int i = 1; i <= 6; ++i)
    {
        buf.push_back(i);
    }

    buf.decrease_capacity(4);

    EXPECT_EQ(buf.capacity(), (AoL::SizeT)4);
    EXPECT_EQ(buf.size(), (AoL::SizeT)4);
    EXPECT_EQ(buf.head, (AoL::SizeT)0);
    EXPECT_EQ(buf[0], 1);
    EXPECT_EQ(buf[1], 2);
    EXPECT_EQ(buf[2], 3);
    EXPECT_EQ(buf[3], 4);
}

TEST(CyclicBufferDynamicCapTest, DecreaseWhileWrappedTruncatesEarliest)
{
    AoL::CyclicBufferD<int> buf(8);

    for (int i = 1; i <= 10; ++i)
    {
        buf.push_back(i);
    }
    ASSERT_EQ(buf.head, (AoL::SizeT)2);

    buf.decrease_capacity(4);

    EXPECT_EQ(buf.head, (AoL::SizeT)0);
    EXPECT_EQ(buf.capacity(), (AoL::SizeT)4);
    EXPECT_EQ(buf.size(), (AoL::SizeT)4);
    EXPECT_EQ(buf[0], 3);
    EXPECT_EQ(buf[1], 4);
    EXPECT_EQ(buf[2], 5);
    EXPECT_EQ(buf[3], 6);
}

TEST(CyclicBufferDynamicCapTest, DecreaseToMinimumOneThenPush)
{
    AoL::CyclicBufferD<int> buf(8);

    for (int i = 1; i <= 3; ++i)
    {
        buf.push_back(i);
    }

    buf.decrease_capacity(1);

    EXPECT_EQ(buf.capacity(), (AoL::SizeT)1);
    EXPECT_EQ(buf.size(), (AoL::SizeT)1);
    EXPECT_EQ(buf.front(), 1);

    buf.push_back(99);
    EXPECT_EQ(buf.size(), (AoL::SizeT)1);
    EXPECT_EQ(buf.front(), 99);
    EXPECT_EQ(buf.back(), 99);
}

TEST(CyclicBufferDynamicCapTest, IncreaseDecreaseRoundTripWrapped)
{
    AoL::CyclicBufferD<int> buf(4);

    for (int i = 1; i <= 6; ++i)
    {
        buf.push_back(i);
    }
    ASSERT_EQ(buf.head, (AoL::SizeT)2);

    buf.increase_capacity(8);
    ASSERT_EQ(buf.head, (AoL::SizeT)0);
    for (int i = 0; i < 6; ++i)
    {
        ASSERT_EQ(buf[i], i + 3);
    }

    for (int i = 7; i <= 10; ++i)
    {
        buf.push_back(i);
    }
    ASSERT_TRUE(buf.full());
    for (int i = 0; i < 8; ++i)
    {
        ASSERT_EQ(buf[i], i + 3);
    }

    buf.decrease_capacity(4);
    EXPECT_EQ(buf.size(), (AoL::SizeT)4);
    for (int i = 0; i < 4; ++i)
    {
        EXPECT_EQ(buf[i], i + 3);
    }
}

// ===================================================================
// EXHAUSTIVE SUITE: ELEMENT LIFECYCLE
// ===================================================================

TEST(CyclicBufferLifecycleTest, FixedPreconstructsAllSlots)
{
    CbLiveObj::live = 0;

    {
        AoL::CyclicBufferF<CbLiveObj, 4> buf;

        EXPECT_EQ(CbLiveObj::live, 4);

        buf.push_back(CbLiveObj(9));
        EXPECT_EQ(buf[0].v, 9);
        EXPECT_EQ(CbLiveObj::live, 4);

        buf.pop_front();
        EXPECT_EQ(CbLiveObj::live, 4);

        buf.clear();
        EXPECT_EQ(CbLiveObj::live, 4);
    }

    EXPECT_EQ(CbLiveObj::live, 0);
}

TEST(CyclicBufferLifecycleTest, CopyAdjustsLiveCount)
{
    CbLiveObj::live = 0;

    AoL::CyclicBufferF<CbLiveObj, 2> a;
    ASSERT_EQ(CbLiveObj::live, 2);

    {
        AoL::CyclicBufferF<CbLiveObj, 2> b(a);
        EXPECT_EQ(CbLiveObj::live, 4);
    }

    EXPECT_EQ(CbLiveObj::live, 2);
}

TEST(CyclicBufferLifecycleTest, DynamicLiveTracksPhysicalSlots)
{
    CbLiveObj::live = 0;

    AoL::CyclicBufferD<CbLiveObj> buf(4);
    EXPECT_EQ(CbLiveObj::live, 0);

    buf.push_back(CbLiveObj(1));
    EXPECT_EQ(CbLiveObj::live, 1);

    buf.emplace_back(2);
    EXPECT_EQ(CbLiveObj::live, 2);

    buf.push_back(CbLiveObj(3));
    EXPECT_EQ(CbLiveObj::live, 3);
    EXPECT_EQ(buf.container_obj.size(), (size_t)3);

    buf.pop_front();
    EXPECT_EQ(CbLiveObj::live, 3);

    buf.pop_back();
    EXPECT_EQ(CbLiveObj::live, 2);

    buf.clear();
    EXPECT_EQ(CbLiveObj::live, 0);
}

// ===================================================================
// EXHAUSTIVE SUITE: REFERENCE-MODEL FUZZ
// ===================================================================

TEST(CyclicBufferStressTest, FuzzAgainstDeque_Fixed)
{
    AoL::CyclicBufferF<int, 64> buf;
    std::deque<int> ref;
    int v = 0;
    CbRng rng(987654321);

    for (int step = 0; step < 8000; ++step)
    {
        AoL::U32 roll = rng.below(1000);

        if (roll < 550 || ref.empty())
        {
            if ((AoL::SizeT)ref.size() == 64)
            {
                ref.pop_front();
            }
            buf.push_back(v);
            ref.push_back(v);
            ++v;
        }
        else if (roll < 800)
        {
            buf.pop_front();
            ref.pop_front();
        }
        else if (roll < 980)
        {
            buf.pop_back();
            ref.pop_back();
        }
        else
        {
            buf.clear();
            ref.clear();
        }

        if (buf.size() != ref.size())
        {
            FAIL() << "size mismatch at step " << step;
        }

        if (!ref.empty())
        {
            ASSERT_EQ(buf.front(), ref.front()) << "front at step " << step;
            ASSERT_EQ(buf.back(), ref.back()) << "back at step " << step;
        }

        if (step % 512 == 0)
        {
            for (size_t i = 0; i < ref.size(); ++i)
            {
                ASSERT_EQ(buf[i], ref[i]) << "element at step " << step;
            }
        }
    }

    ExpectMatchesDeque(buf, ref);
}

TEST(CyclicBufferStressTest, FuzzAgainstDeque_Dynamic)
{
    AoL::CyclicBufferD<int> buf(64);
    std::deque<int> ref;
    int v = 0;
    CbRng rng(24680);

    for (int step = 0; step < 10000; ++step)
    {
        AoL::U32 roll = rng.below(1000);

        if (roll < 450)
        {
            buf.push_back(v);
            ref.push_back(v);
            ++v;
            if ((AoL::SizeT)ref.size() > buf.capacity())
            {
                ref.pop_front();
            }
        }
        else if (roll < 550)
        {
            buf.emplace_back(v);
            ref.push_back(v);
            ++v;
            if ((AoL::SizeT)ref.size() > buf.capacity())
            {
                ref.pop_front();
            }
        }
        else if (roll < 750)
        {
            if (!ref.empty())
            {
                buf.pop_front();
                ref.pop_front();
            }
        }
        else if (roll < 900)
        {
            if (!ref.empty())
            {
                buf.pop_back();
                ref.pop_back();
            }
        }
        else if (roll < 905)
        {
            buf.clear();
            ref.clear();
        }
        else if (roll < 908 && buf.capacity() >= 1 && buf.capacity() < 512)
        {
            AoL::SizeT ncap = std::bit_ceil(buf.capacity() + 1);
            buf.increase_capacity(ncap);
        }
        else if (roll < 911 && buf.capacity() >= 2)
        {
            AoL::SizeT ncap = std::bit_floor(buf.capacity() - 1);
            buf.decrease_capacity(ncap);
            if ((AoL::SizeT)ref.size() > ncap)
            {
                ref.erase(ref.begin() + (ptrdiff_t)ncap, ref.end());
            }
        }

        if (buf.size() != ref.size())
        {
            FAIL() << "size mismatch at step " << step << ": buf=" << buf.size() << " ref=" << ref.size();
        }

        if (!ref.empty())
        {
            ASSERT_EQ(buf.front(), ref.front()) << "front at step " << step;
            ASSERT_EQ(buf.back(), ref.back()) << "back at step " << step;
        }

        if (step % 512 == 0)
        {
            for (size_t i = 0; i < ref.size(); ++i)
            {
                ASSERT_EQ(buf[i], ref[i]) << "element at step " << step;
            }
        }
    }

    ExpectMatchesDeque(buf, ref);
}

#endif // AOL_TEST_CONTAINERS_CYCLICBUFFER
