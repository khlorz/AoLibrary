/********************************************************************
* Partition container tests - exhaustive
*
* Covers PartitionVector and PartitionArray via SubPartition windows.
* File contents:
* - Reference model (RefModel) mirroring window mechanics on deque
* - SubPartition window semantics (vector host)
* - PartitionVector lifecycle (constructors, assign, push, reserve, resize)
* - PartitionArray fixed-storage parity
* - Type-level contracts
* - Model-checked fuzz (vector + array)
* - Additional exhaustive edge cases and swap
*
* Conventions (same as cyclic-buffer / flat-key-order-map):
* - Scale/capacity numbers are named constants with why-comments;
*   payload literals like {1,2,3} stay inline (self-documenting).
* - Every TEST has a one-line what + why comment.
* - Helpers live in anonymous namespace.
********************************************************************/


#include "pch.h"
#include "config.h"
#if AOL_TEST_CONTAINERS_PARTITION

#include "aol/partitions.h"
#include "aol/utilities.h"

#include <algorithm>
#include <deque>
#include <memory>
#include <numeric>
#include <string>
#include <type_traits>
#include <utility>

namespace
{

// ===================================================================
// SHARED SCALES AND CONSTANTS
// ===================================================================
// Small enough for Debug speed, large enough to force splits/slides.

constexpr AoL::SizeT kVecSmallInit = 4;      // minimal vector split (needs max>1)
constexpr AoL::SizeT kVecMediumInit = 6;     // default fuzz seed payload
constexpr AoL::SizeT kArrayCap8 = 8;         // fixed array PartitionArray<int,8>
constexpr AoL::SizeT kFuzzArrayCap64 = 64;   // fuzz array (many windows)
constexpr AoL::SizeT kFuzzStepsVec = 5000;   // vector fuzz length
constexpr AoL::SizeT kFuzzStepsArray = 2000; // array fuzz (no growth, shorter)
constexpr AoL::SizeT kReserve64 = 64;        // reserve() probe
constexpr AoL::SizeT kResizeUpTo5 = 5;       // resize-up target
constexpr AoL::SizeT kChainedTileTotal = 8;  // sum of max_sizes must equal 8
constexpr AoL::SizeT kPartitionSmall = 2;    // small split size
constexpr AoL::SizeT kPartitionMedium = 3;   // medium split size
constexpr AoL::SizeT kPack4 = 4;             // pack ctor arity
constexpr AoL::SizeT kPack5 = 5;             // pack ctor arity

constexpr unsigned kFuzzSeedVec = 0xC0FFEEu;
constexpr unsigned kFuzzSeedArray = 0xF00Du;

// ===================================================================
// REFERENCE MODEL - mirrors Internal mechanics on deque
// ===================================================================

struct RefWin
{
    AoL::SizeT begin = 0;
    AoL::SizeT size = 0;
    AoL::SizeT max = 0;
};

class RefModel
{
public:
    std::deque<int> raw;
    std::vector<RefWin> wins;

    void Reset(AoL::SizeT b, AoL::SizeT s, AoL::SizeT m)
    {
        raw.clear();
        wins.assign(1, RefWin{ b, s, m });
    }

    RefWin& Win(AoL::SizeT i) { return wins[i]; }
    RefWin& Back() { return wins.back(); }
    AoL::SizeT TotalRaw() const { return (AoL::SizeT)raw.size(); }

    void PushBack(AoL::SizeT i, int v)
    {
        raw[(size_t)(wins[i].begin + wins[i].size)] = v;
        wins[i].size++;
    }

    void GrowRaw(int v)
    {
        raw.push_back(v);
        wins.back().max = TotalRaw() - wins.back().begin;
    }

    void PopFront(AoL::SizeT i)
    {
        const auto& w = wins[i];
        std::rotate(
            raw.begin() + (ptrdiff_t)w.begin,
            raw.begin() + (ptrdiff_t)(w.begin + 1),
            raw.begin() + (ptrdiff_t)(w.begin + w.size));
        wins[i].size--;
    }

    void Erase(AoL::SizeT i, AoL::SizeT at, AoL::SizeT cnt)
    {
        const auto& w = wins[i];
        std::rotate(
            raw.begin() + (ptrdiff_t)(w.begin + at),
            raw.begin() + (ptrdiff_t)(w.begin + at + cnt),
            raw.begin() + (ptrdiff_t)(w.begin + w.size));
        wins[i].size -= cnt;
    }

    void CreateByCount(AoL::SizeT n, bool start_empty)
    {
        const RefWin back = wins.back();
        const bool small = back.size <= n;
        wins.back() = RefWin{ back.begin, start_empty ? 0 : (small ? back.size : n), n };
        const AoL::SizeT fresh = (!small) ? (back.size - n) : 0;
        wins.push_back(RefWin{ back.begin + n, fresh, 0 });
        wins.back().max = TotalRaw() - wins.back().begin;
    }

    void CreateByPredicate(AoL::SizeT n_true) { CreateByCount(n_true, false); }

    void ErasePartition(AoL::SizeT i)
    {
        const AoL::SizeT sh = wins[i].size;
        for (size_t j = i + 1; j < wins.size(); ++j)
        {
            wins[j].begin -= sh;
        }
        wins.erase(wins.begin() + (ptrdiff_t)i);
    }

    template<typename HOST>
    void ExpectMatches(const HOST& host) const
    {
        ASSERT_EQ(host.sub_partitions.size(), wins.size());
        for (size_t i = 0; i < wins.size(); ++i)
        {
            SCOPED_TRACE(testing::Message() << "partition " << i);
            const auto& sp = host.sub_partitions[i];
            ASSERT_EQ(sp.size(), wins[i].size);
            ASSERT_EQ(sp.max_size(), wins[i].max);
            for (AoL::SizeT k = 0; k < wins[i].size; ++k)
            {
                ASSERT_EQ(sp[k], raw[(size_t)(wins[i].begin + k)]) << "elem " << k;
            }
        }
        ASSERT_EQ(host.container_obj.size(), raw.size());
        for (size_t k = 0; k < raw.size() && k < 4; ++k)
        {
            ASSERT_EQ(host.container_obj[k], raw[k]) << "raw " << k;
        }
    }
};

struct BigProbe
{
    int v = 0;
    std::string tag;
    explicit BigProbe(int x) : v{ x }, tag{ "probe" } {}
};

static_assert(AoL::Traits::IsCheapToCopy<int>, "int must be cheap");
static_assert(!AoL::Traits::IsCheapToCopy<BigProbe>, "BigProbe must be non-cheap");

using TestPV = AoL::PartitionVector<int>;
using TestPA = AoL::PartitionArray<int, kArrayCap8>;
using FuzzPA = AoL::PartitionArray<int, kFuzzArrayCap64>;

} // namespace

// ===================================================================
// SUITE: SUB-PARTITION WINDOW SEMANTICS
// ===================================================================

// Splits default window keeping both halves visible
TEST(SubPartitionExTest, CreateSplitsDefaultWindowInPlace)
{
    TestPV pv{ 1, 2, 3, 4, 5 };
    auto& p0 = pv.create_partition(kPartitionSmall, false);
    EXPECT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(p0.size(), kPartitionSmall);
    EXPECT_EQ(pv.get_default_partition().size(), kVecSmallInit - kPartitionSmall + 1);
    EXPECT_EQ(p0[0], 1);
    EXPECT_EQ(pv.get_default_partition()[0], 3);
}

// Primary write path: lvalue and rvalue (non-cheap) at window end
TEST(SubPartitionExTest, PushBackAcceptsLvaluesAndRvalues)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p1 = pv.create_partition(kPartitionSmall);
    const int lv = 7;
    EXPECT_TRUE(p1.push_back(lv));
    EXPECT_EQ(p1[0], 7);
    AoL::PartitionVector<BigProbe> pb;
    pb.emplace_back(BigProbe{ 1 });
    auto& dp = pb.get_default_partition();
    BigProbe src{ 42 };
    EXPECT_TRUE(dp.push_back(std::move(src)));
    EXPECT_EQ(dp.back().v, 42);
}

// Full partition rejects push
TEST(SubPartitionExTest, PushBackRejectsWhenFull)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p1 = pv.create_partition(kPartitionSmall);
    EXPECT_TRUE(p1.push_back(10));
    EXPECT_TRUE(p1.push_back(11));
    ASSERT_TRUE(p1.full());
    EXPECT_FALSE(p1.push_back(12));
    EXPECT_EQ(p1.size(), kPartitionSmall);
}

// In-place construct returns pointer or nullptr when full
TEST(SubPartitionExTest, EmplaceBackReturnsPointerOrNullptr)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p1 = pv.create_partition(kPartitionSmall);
    int* s = p1.emplace_back(99);
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(*s, 99);
    p1.emplace_back(100);
    EXPECT_EQ(p1.emplace_back(101), nullptr);
}

// pop_back shrinks, pop_front rotates
TEST(SubPartitionExTest, PopBackShrinksPopFrontRotates)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6 };
    auto& p0 = pv.create_partition(kPartitionMedium + 1, false);
    auto& def = pv.get_default_partition();
    p0.pop_back();
    EXPECT_EQ(p0.size(), kPartitionMedium);
    EXPECT_EQ(pv[3], 4);
    def.pop_front();
    EXPECT_EQ(def.size(), 1u);
    EXPECT_EQ(def[0], 6);
}

// Single erase shifts tail within window
TEST(SubPartitionExTest, EraseSingleShiftsTailWithinWindow)
{
    TestPV pv{ 10, 20, 30, 40 };
    auto& dp = pv.get_default_partition();
    dp.erase(1);
    EXPECT_EQ(dp.size(), 3u);
    EXPECT_EQ(dp[1], 30);
}

// Ranged erase in one rotate
TEST(SubPartitionExTest, EraseRangeRemovesContiguousRun)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6 };
    auto& dp = pv.get_default_partition();
    dp.erase(1, kPartitionMedium);
    EXPECT_EQ(dp.size(), kPartitionMedium);
    EXPECT_EQ(dp[1], 5);
}

// clear() hides, raw untouched, reusable
TEST(SubPartitionExTest, ClearHidesElementsNotDestroysThem)
{
    TestPV pv{ 1, 2, 3 };
    auto& dp = pv.get_default_partition();
    dp.clear();
    EXPECT_TRUE(dp.empty());
    EXPECT_EQ(pv.size(), 3u);
    EXPECT_TRUE(dp.push_back(9));
    EXPECT_EQ(dp[0], 9);
}

// Iteration matches subscript order
TEST(SubPartitionExTest, IteratorsMatchSubscriptOrder)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p0 = pv.create_partition(kPartitionSmall, false);
    const auto& cdp = pv.get_default_partition();
    int s = 0;
    for (int v : p0) s += v;
    EXPECT_EQ(s, 3);
    s = 0;
    for (const int v : cdp) s += v;
    EXPECT_EQ(s, 7);
    std::deque<int> seen;
    for (auto it = p0.rbegin(); it != p0.rend(); ++it) seen.push_back(*it);
    EXPECT_EQ(seen, (std::deque<int>{ 2, 1 }));
}

// front/back respect live window
TEST(SubPartitionExTest, FrontBackTrackLiveWindow)
{
    TestPV pv{ 5, 6, 7, 8 };
    auto& p1 = pv.create_partition(kPartitionSmall, false);
    const auto& cp1 = pv.get_partition(0);
    EXPECT_EQ(p1.front(), 5);
    EXPECT_EQ(cp1.back(), 6);
    p1.front() = 50;
    EXPECT_EQ(cp1[0], 50);
}

// Staged empty partition is usable
TEST(SubPartitionExTest, CreatedEmptyPartitionIsUsable)
{
    TestPV pv{ 1, 2, 3, 4, 5 };
    auto& st = pv.create_partition(kPartitionMedium);
    EXPECT_TRUE(st.empty());
    EXPECT_EQ(st.max_size(), kPartitionMedium);
    for (int i = 0; i < (int)kPartitionMedium; ++i) EXPECT_TRUE(st.push_back(i));
    EXPECT_TRUE(st.full());
}

// Whole-container accessors expose raw storage
TEST(SubPartitionExTest, ContainerAccessorsExposeRawStorage)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6 };
    auto& p0 = pv.create_partition(kPartitionSmall, false);
    pv.get_default_partition().clear();
    EXPECT_EQ(p0.size() + pv.get_default_partition().size(), kPartitionSmall);
    EXPECT_EQ(pv.size(), kVecMediumInit);
    EXPECT_EQ(pv.back(), 6);
}

// erase_partition slides later windows by live size
TEST(SubPartitionExTest, ErasePartitionSlidesLaterWindowsLeft)
{
    TestPV pv{ 1, 2, 3, 4, 5 };
    pv.create_partition(kPartitionSmall, false);
    pv.create_partition(kPartitionSmall, false);
    pv.erase_partition(1);
    ASSERT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(pv.get_partition(1)[0], 3);
}

// Const view matches mutable
TEST(SubPartitionExTest, ConstViewMatchesMutable)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p0 = pv.create_partition(kPartitionSmall, false);
    const auto& cp0 = pv.get_partition(0);
    EXPECT_EQ(cp0.size(), p0.size());
    std::deque<int> a(p0.begin(), p0.end()), b(cp0.begin(), cp0.end());
    EXPECT_EQ(a, b);
}

// Max_size invariant after mixed ops
TEST(SubPartitionExTest, MaxSizeInvariantHoldsAfterOps)
{
    TestPV pv{ 1, 2, 3, 4, 5 };
    auto& p0 = pv.create_partition(kPartitionSmall, false);
    p0.pop_back();
    EXPECT_LE(p0.size(), p0.max_size());
    p0.clear();
    EXPECT_EQ(p0.max_size(), kPartitionSmall);
    pv.push_back(6);
    EXPECT_EQ(pv.get_default_partition().max_size(), kPartitionMedium + 1);
}

// ===================================================================
// SUITE: PARTITION VECTOR LIFECYCLE
// ===================================================================

// Default: one empty default partition
TEST(PartitionVectorExTest, DefaultStartsWithOneEmptyDefault)
{
    TestPV pv;
    EXPECT_TRUE(pv.empty());
    EXPECT_EQ(pv.number_of_partitions(), 1u);
    EXPECT_TRUE(pv.get_default_partition().empty());
}

// Initializer-list spans whole payload
TEST(PartitionVectorExTest, InitListCoversEverythingWithOneDefault)
{
    TestPV pv{ 10, 20, 30 };
    EXPECT_EQ(pv.size(), kPartitionMedium);
    EXPECT_EQ(pv.number_of_partitions(), 1u);
    EXPECT_EQ(pv.get_default_partition()[2], 30);
}

// Count and count+value ctors
TEST(PartitionVectorExTest, CountAndFillCtorsSpanFullRange)
{
    TestPV a(kPack4);
    EXPECT_EQ(a.size_of_partition(0), kPack4);
    TestPV b((AoL::SizeT)kPack4, 7);
    for (AoL::SizeT i = 0; i < b.size(); ++i) EXPECT_EQ(b[i], 7);
}

// Iterator-pair ctor from any source
TEST(PartitionVectorExTest, IteratorPairCtorTakesAnySource)
{
    std::deque<int> src{ 1, 2, 3, 4 };
    TestPV pv(src.begin(), src.end());
    EXPECT_EQ(pv.size(), kVecSmallInit);
    EXPECT_EQ(pv[0], 1);
}

// Container push grows storage, default window slack only
TEST(PartitionVectorExTest, ContainerPushBackExtendsDefaultSlackOnly)
{
    TestPV pv{ 1, 2 };
    pv.push_back(3);
    pv.emplace_back(4);
    EXPECT_EQ(pv.size(), kVecSmallInit);
    auto& dp = pv.get_default_partition();
    EXPECT_EQ(dp.size(), kPartitionSmall);
    EXPECT_EQ(dp.max_size(), kVecSmallInit);
}

// assign() collapses to one default
TEST(PartitionVectorExTest, AssignResetsToSingleDefault)
{
    TestPV pv{ 1, 2, 3, 4 };
    pv.create_partition(kPartitionSmall, false);
    pv.assign(TestPV::container_type{ 9, 8, 7 });
    EXPECT_EQ(pv.number_of_partitions(), 1u);
    EXPECT_EQ(pv[0], 9);
}

// reserve only grows capacity
TEST(PartitionVectorExTest, ReserveOnlyGrowsCapacity)
{
    TestPV pv{ 1, 2, 3 };
    pv.reserve(kReserve64);
    EXPECT_GE(pv.container_obj.capacity(), kReserve64);
    EXPECT_EQ(pv.size(), kPartitionMedium);
}

// resize up extends default slack only
TEST(PartitionVectorExTest, ResizeUpExtendsDefaultSlackOnly)
{
    TestPV pv{ 1, 2 };
    pv.resize(kResizeUpTo5);
    EXPECT_EQ(pv.size(), kResizeUpTo5);
    EXPECT_EQ(pv.get_default_partition().size(), kPartitionSmall);
}

// resize down drops partitions beyond cut
TEST(PartitionVectorExTest, ResizeDownDropsAndTruncatesPartitions)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6, 7, 8 };
    pv.create_partition(kPartitionMedium, false);
    pv.create_partition(kPartitionSmall, false);
    pv.resize(kResizeUpTo5);
    EXPECT_EQ(pv.size(), kResizeUpTo5);
    EXPECT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(pv.get_partition(1)[0], 4);
}

// clear_partitions keeps shape and raw storage
TEST(PartitionVectorExTest, ClearPartitionsKeepsShapeAndStorage)
{
    TestPV pv{ 1, 2, 3, 4 };
    pv.create_partition(kPartitionSmall, false);
    pv.clear_partitions();
    EXPECT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(pv.size(), kVecSmallInit);
    EXPECT_EQ(pv.size_of_partition(0), 0u);
}

// clear_all leaves zero partitions until assign
TEST(PartitionVectorExTest, ClearAllLeavesNoDefaultUntilAssign)
{
    TestPV pv{ 1, 2, 3 };
    pv.clear_all();
    EXPECT_EQ(pv.number_of_partitions(), 0u);
    pv.assign(TestPV::container_type{});
    EXPECT_EQ(pv.number_of_partitions(), 1u);
}

// Deep copy rebinds windows
TEST(PartitionVectorExTest, CopyRebindsWindowsToNewStorage)
{
    TestPV src{ 1, 2, 3, 4, 5 };
    src.create_partition(kPartitionSmall, false);
    TestPV dst{ src };
    EXPECT_EQ(dst.get_partition(0)[0], 1);
    dst.get_partition(0)[0] = 100;
    EXPECT_EQ(src.get_partition(0)[0], 1);
}

// Move leaves source with one empty default
TEST(PartitionVectorExTest, MoveLeavesSourceValidButEmpty)
{
    TestPV src{ 1, 2, 3, 4 };
    src.create_partition(kPartitionSmall, false);
    TestPV dst{ std::move(src) };
    EXPECT_EQ(dst.number_of_partitions(), 2u);
    EXPECT_EQ(src.number_of_partitions(), 1u);
    EXPECT_TRUE(src.get_default_partition().empty());
}

// Stable predicate split preserves order
TEST(PartitionVectorExTest, PredicateCreateStablePreservesGroupOrder)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6 };
    auto& ev = pv.create_partition([](const int& x) { return (x % 2) == 0; });
    EXPECT_EQ(ev.size(), kPartitionMedium);
    EXPECT_EQ(ev[0], 2);
    EXPECT_EQ(pv.get_default_partition()[0], 1);
}

// Unstable predicate groups correctly
TEST(PartitionVectorExTest, PredicateCreateUnstableGroupsCorrectly)
{
    TestPV pv{ 6, 1, 5, 2, 4, 3 };
    auto& sm = pv.create_partition([](const int& x) { return x <= 3; }, false);
    std::deque<int> g(sm.begin(), sm.end());
    std::sort(g.begin(), g.end());
    EXPECT_EQ(g, (std::deque<int>{ 1, 2, 3 }));
}

// Chained creates tile storage exactly
TEST(PartitionVectorExTest, ChainedCreatesTileStorageExactly)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6, 7, 8 };
    pv.create_partition(kPartitionSmall, false);
    pv.create_partition(kPartitionMedium, false);
    AoL::SizeT cov = 0;
    for (AoL::SizeT i = 0; i < pv.number_of_partitions(); ++i) cov += pv.get_partition(i).max_size();
    EXPECT_EQ(cov, pv.size());
}

// Self copy assign is no-op (regression for clear-then-copy bug)
TEST(PartitionVectorExTest, CopySelfAssignIsNoOp)
{
    TestPV pv{ 1, 2, 3, 4 };
    pv.create_partition(kPartitionSmall, false);
    const auto n = pv.number_of_partitions();
    const auto sz = pv.size();
    pv = pv;
    EXPECT_EQ(pv.number_of_partitions(), n);
    EXPECT_EQ(pv.size(), sz);
}

// Self move is safe
TEST(PartitionVectorExTest, MoveSelfAssignIsSafe)
{
    TestPV pv{ 5, 6, 7 };
    pv.create_partition(1, false);
    pv = std::move(pv);
    EXPECT_GE(pv.number_of_partitions(), 1u);
}

// Copy assign overwrites shape
TEST(PartitionVectorExTest, CopyAssignOverwritesShape)
{
    TestPV src{ 10, 20, 30, 40 };
    src.create_partition(kPartitionSmall, false);
    TestPV dst{ 1, 2 };
    dst = src;
    EXPECT_EQ(dst.number_of_partitions(), 2u);
    EXPECT_EQ(dst.get_partition(0)[1], 20);
}

// Allocator ctor rebinds
TEST(PartitionVectorExTest, AllocatorCtorRebinds)
{
    TestPV src{ 1, 2, 3 };
    src.create_partition(1, false);
    typename TestPV::container_type::allocator_type al;
    TestPV cpy(src, al);
    EXPECT_EQ(cpy.get_partition(0)[0], 1);
    TestPV moved(std::move(src), al);
    EXPECT_EQ(moved.number_of_partitions(), 2u);
}

// clear_all then assign and push
TEST(PartitionVectorExTest, ClearAllThenAssignAndPush)
{
    TestPV pv{ 1, 2, 3 };
    pv.clear_all();
    pv.assign(TestPV::container_type{ 7, 8 });
    pv.push_back(9);
    EXPECT_EQ(pv.size(), kPartitionMedium);
    EXPECT_EQ(pv.get_default_partition().max_size(), kPartitionMedium);
}

// Erasing first partition slides later windows
TEST(PartitionVectorExTest, EraseFirstPartitionSlides)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6 };
    pv.create_partition(kPartitionSmall, false);
    pv.create_partition(kPartitionSmall, false);
    pv.erase_partition(0);
    ASSERT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(pv.get_partition(0)[0], 1);
    EXPECT_EQ(pv.get_partition(1)[0], 3);
}

// Erasing empty partition is no shift
TEST(PartitionVectorExTest, EraseEmptyPartitionIsNoShift)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p0 = pv.create_partition(kPartitionSmall, false);
    p0.clear();
    pv.erase_partition(0);
    EXPECT_EQ(pv.number_of_partitions(), 1u);
    EXPECT_EQ(pv.get_default_partition().size(), kPartitionSmall);
}

// Small old window case
TEST(PartitionVectorExTest, CreateStartEmptyVsRetainWithSmallOld)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p0 = pv.create_partition(kPartitionSmall, false);
    p0.clear();
    pv.get_default_partition().pop_back();
    auto& p1 = pv.create_partition(1, true);
    EXPECT_TRUE(p1.empty());
    TestPV pv2{ 10, 20, 30, 40 };
    pv2.create_partition(kPartitionSmall, false);
    pv2.get_partition(0).clear();
    pv2.get_default_partition().pop_back();
    auto& q1 = pv2.create_partition(1, false);
    EXPECT_FALSE(q1.empty());
    EXPECT_TRUE(pv2.get_default_partition().empty());
}

// String payload survives splits
TEST(PartitionVectorExTest, StringPayloadSurvivesSplits)
{
    AoL::PartitionVector<std::string> pv{ std::string("a"), std::string("b"), std::string("c") };
    auto& p0 = pv.create_partition(1, false);
    EXPECT_EQ(p0[0], "a");
    p0.push_back(std::string("x"));
    EXPECT_EQ(p0[0], "a");
}

// Partition counts after chained creates
TEST(PartitionVectorExTest, PartitionCountsAfterChainedCreates)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    for (int i = 0; i < 4; ++i) pv.create_partition(kPartitionSmall, false);
    EXPECT_EQ(pv.number_of_partitions(), 5u);
    AoL::SizeT tot = 0;
    for (AoL::SizeT i = 0; i < pv.number_of_partitions(); ++i) tot += pv.get_partition(i).max_size();
    EXPECT_EQ(tot, pv.size());
}

// Container-level iteration is raw order
TEST(PartitionVectorExTest, ContainerIterationIsRawOrder)
{
    TestPV pv{ 3, 1, 2 };
    pv.create_partition(1, false);
    std::vector<int> raw(pv.begin(), pv.end());
    EXPECT_EQ(raw, (std::vector<int>{ 3, 1, 2 }));
}

// ===================================================================
// SUITE: PARTITION ARRAY
// ===================================================================

// Array default: full capacity, empty window
TEST(PartitionArrayExTest, DefaultHasFullCapacityEmptyWindow)
{
    TestPA pa;
    EXPECT_EQ(pa.size(), kArrayCap8);
    EXPECT_TRUE(pa.get_default_partition().empty());
    EXPECT_EQ(pa.get_default_partition().max_size(), kArrayCap8);
}

// Fill ctor covers all slots
TEST(PartitionArrayExTest, FillValueCtorCoversAllSlots)
{
    AoL::PartitionArray<int, kPack5> pa(7);
    for (AoL::SizeT i = 0; i < kPack5; ++i) EXPECT_EQ(pa[i], 7);
    EXPECT_TRUE(pa.get_default_partition().full());
}

// Pack ctor spans whole array
TEST(PartitionArrayExTest, ElementPackCtorSpansWholeArray)
{
    AoL::PartitionArray<int, kPack4> pa(1, 2, 3, 4);
    EXPECT_EQ(pa.size_of_partition(0), kPack4);
    EXPECT_EQ(pa[3], 4);
}

// Iterator-pair ctor copies up to capacity
TEST(PartitionArrayExTest, IteratorPairCtorCopiesUpToCapacity)
{
    std::deque<int> src{ 1, 2, 3 };
    AoL::PartitionArray<int, kArrayCap8> pa(src.begin(), src.end());
    EXPECT_EQ(pa.size_of_partition(0), kPartitionMedium);
}

// Window ops on fixed storage
TEST(PartitionArrayExTest, WindowOpsOnFixedStorageMatchVectorBehavior)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();
    for (int i = 0; i < (int)kArrayCap8; ++i) EXPECT_NE(dp.emplace_back(i), nullptr);
    EXPECT_TRUE(dp.full());
    dp.pop_front();
    EXPECT_EQ(dp[0], 1);
    dp.erase(kPartitionSmall, kPartitionSmall);
    EXPECT_EQ(dp[2], 5);
}

// Create tiles fixed storage
TEST(PartitionArrayExTest, CreatePartitionTilesFixedStorage)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();
    for (int i = 0; i < 6; ++i) dp.emplace_back(i);
    auto& p0 = pa.create_partition(kPartitionMedium, false);
    EXPECT_EQ(p0.size(), kPartitionMedium);
    EXPECT_EQ(p0[0], 0);
    AoL::SizeT cov = 0;
    for (AoL::SizeT i = 0; i < pa.number_of_partitions(); ++i) cov += pa.get_partition(i).max_size();
    EXPECT_EQ(cov, pa.size());
}

// Copy is independent
TEST(PartitionArrayExTest, CopyIsIndependent)
{
    TestPA src;
    src.get_default_partition().push_back(1);
    src.get_default_partition().push_back(2);
    TestPA dst{ src };
    dst.get_default_partition()[0] = 100;
    EXPECT_EQ(src.get_default_partition()[0], 1);
}

// Copy/move assign parity
TEST(PartitionArrayExTest, CopyMoveAssignParity)
{
    TestPA a;
    a.get_default_partition().push_back(1);
    a.get_default_partition().push_back(2);
    TestPA b;
    b = a;
    EXPECT_EQ(b.get_default_partition()[0], 1);
    b.get_default_partition()[0] = 99;
    EXPECT_EQ(a.get_default_partition()[0], 1);
    TestPA c;
    c = std::move(b);
    EXPECT_EQ(c.get_default_partition()[0], 99);
}

// Array clear keeps shape
TEST(PartitionArrayExTest, ClearPartitionsKeepsShapeArray)
{
    TestPA pa;
    pa.get_default_partition().push_back(5);
    pa.get_default_partition().clear();
    EXPECT_EQ(pa.number_of_partitions(), 1u);
    EXPECT_TRUE(pa.get_default_partition().empty());
    EXPECT_EQ(pa.size(), kArrayCap8);
}

// Empty range ctor
TEST(PartitionArrayExTest, IteratorPairCtorEmptyRange)
{
    std::vector<int> empty;
    AoL::PartitionArray<int, kPack4> pa(empty.begin(), empty.end());
    EXPECT_TRUE(pa.get_default_partition().empty());
}

// ===================================================================
// SUITE: TYPE-LEVEL CONTRACTS
// ===================================================================

TEST(PartitionTypeTest, IteratorTraitsAreContiguous)
{
    using Sub = TestPV::sub_partition_type;
    static_assert(std::contiguous_iterator<typename Sub::iterator>);
    static_assert(std::same_as<typename Sub::value_type, int>);
    TestPV pv{ 1, 2, 3 };
    EXPECT_EQ(pv.get_default_partition().begin()[2], 3);
}

// ===================================================================
// SUITE: MODEL-CHECKED FUZZ
// ===================================================================

template<typename HOST, bool CAN_GROW>
void RunPartitionModelLoop(HOST& host, RefModel& model, AoL::SizeT steps, unsigned seed)
{
    unsigned lcg = seed;
    auto roll = [&]() -> unsigned { lcg = lcg * 1664525u + 1013904223u; return (lcg >> 16) & 0x7FF; };
    for (AoL::SizeT step = 0; step < steps; ++step)
    {
        SCOPED_TRACE(testing::Message() << "step " << step);
        const unsigned r = roll();
        const AoL::SizeT last = host.number_of_partitions() - 1;
        const AoL::SizeT pick = (r % 4 == 0 && last > 0) ? (roll() % last) : last;
        auto& picked = host.sub_partitions[pick];
        if (r % 100 < 25 && !picked.full())
        {
            int v = int(r % 97);
            picked.push_back(v);
            model.PushBack(pick, v);
        }
        else if (r % 100 < 35 && picked.size() > 0)
        {
            picked.pop_front();
            model.PopFront(pick);
        }
        else if (r % 100 < 43 && picked.size() > 0)
        {
            picked.pop_back();
            model.Win(pick).size--;
        }
        else if (r % 100 < 51 && picked.size() > 0)
        {
            const AoL::SizeT at = roll() % picked.size();
            picked.erase(at);
            model.Erase(pick, at, 1);
        }
        else if (r % 100 < 55)
        {
            picked.clear();
            model.Win(pick).size = 0;
        }
        else if (r % 100 < 63 && host.get_default_partition().max_size() > 1)
        {
            auto& back = host.get_default_partition();
            const AoL::SizeT n = 1 + roll() % (back.max_size() - 1);
            const bool se = (roll() & 1u) == 1u;
            SCOPED_TRACE(testing::Message() << "create n=" << n << " se=" << se);
            host.create_partition(n, se);
            model.CreateByCount(n, se);
        }
        else if (r % 100 < 68 && host.number_of_partitions() > 1)
        {
            const AoL::SizeT v = roll() % last;
            host.erase_partition(v);
            model.ErasePartition(v);
        }
        else if (r % 100 < 73)
        {
            RefWin& bm = model.Back();
            if (bm.size >= 2)
            {
                const bool st = (roll() & 1u) == 1u;
                auto pred = [](const int& x) { return (x % 2) != 0; };
                std::vector<int> probe(model.raw.begin() + (ptrdiff_t)bm.begin, model.raw.begin() + (ptrdiff_t)(bm.begin + bm.size));
                auto ps = st ? std::stable_partition(probe.begin(), probe.end(), pred) : std::partition(probe.begin(), probe.end(), pred);
                const AoL::SizeT nt = (AoL::SizeT)(ps - probe.begin());
                if (nt > 0 && nt < bm.size)
                {
                    auto f = model.raw.begin() + (ptrdiff_t)bm.begin;
                    auto l = f + (ptrdiff_t)bm.size;
                    if (st) std::stable_partition(f, l, pred); else std::partition(f, l, pred);
                    host.create_partition(pred, st);
                    model.CreateByPredicate(nt);
                }
            }
        }
        else if constexpr (CAN_GROW)
        {
            if (r % 100 < 78 && model.TotalRaw() < 128)
            {
                int v = int(r % 53);
                host.push_back(v);
                model.GrowRaw(v);
            }
        }
        if (step < 256 || step % 4 == 0) model.ExpectMatches(host);
    }
}

TEST(PartitionFuzzTest, FuzzAgainstDequeModel_Vector)
{
    TestPV host;
    for (int i = 1; i <= (int)kVecMediumInit; ++i)
    {
        host.push_back(i);
        host.get_default_partition().push_back(i);
    }
    RefModel m;
    m.Reset(0, kVecMediumInit, kVecMediumInit);
    for (int i = 1; i <= (int)kVecMediumInit; ++i) m.raw.push_back(i);
    RunPartitionModelLoop<TestPV, true>(host, m, kFuzzStepsVec, kFuzzSeedVec);
}

TEST(PartitionFuzzTest, FuzzAgainstDequeModel_Array)
{
    FuzzPA host(0);
    RefModel m;
    m.Reset(0, kFuzzArrayCap64, kFuzzArrayCap64);
    m.raw.assign(kFuzzArrayCap64, 0);
    RunPartitionModelLoop<FuzzPA, false>(host, m, kFuzzStepsArray, kFuzzSeedArray);
}

// Swap exchanges storage and rebinds window pointers
TEST(PartitionVectorExTest, SwapExchangesVectorContents)
{
    TestPV a{ 1, 2, 3, 4 };
    a.create_partition(kPartitionSmall, false);
    TestPV b{ 10, 20, 30 };
    b.create_partition(1, false);
    a.get_partition(0).clear();
    b.get_partition(0).clear();
    const auto a_sz = a.get_partition(1).size();
    const auto b_sz = b.get_partition(1).size();
    a.swap(b);
    EXPECT_EQ(a.size(), kPartitionMedium);
    EXPECT_EQ(b.size(), kVecSmallInit);
    EXPECT_EQ(a.get_partition(1).size(), b_sz);
    EXPECT_EQ(b.get_partition(1).size(), a_sz);
    EXPECT_TRUE(a.get_partition(0).push_back(99));
    EXPECT_TRUE(b.get_partition(0).push_back(77));
}

TEST(PartitionVectorExTest, SwapSelfIsNoOp)
{
    TestPV pv{ 1, 2, 3 };
    pv.create_partition(1, false);
    const auto n = pv.number_of_partitions();
    pv.swap(pv);
    EXPECT_EQ(pv.number_of_partitions(), n);
}

TEST(PartitionVectorExTest, SwapViaADL)
{
    TestPV a{ 1, 2 };
    TestPV b{ 10, 20, 30 };
    swap(a, b);
    EXPECT_EQ(a.size(), kPartitionMedium);
    std::swap(a, b);
    EXPECT_EQ(a.size(), kPartitionSmall);
}

TEST(PartitionArrayExTest, SwapExchangesArrayContents)
{
    AoL::PartitionArray<int, kPack4> a{ 1, 2, 3, 4 };
    AoL::PartitionArray<int, kPack4> b{ 5, 6, 7, 8 };
    a.create_partition(kPartitionSmall, false);
    b.create_partition(1, false);
    a.swap(b);
    EXPECT_EQ(a.get_partition(0)[0], 5);
    EXPECT_EQ(b.get_partition(0)[0], 1);
}

TEST(PartitionArrayExTest, SwapSelfIsNoOpArray)
{
    AoL::PartitionArray<int, kPack4> a{ 1, 2, 3, 4 };
    a.create_partition(kPartitionSmall, false);
    const auto n = a.number_of_partitions();
    a.swap(a);
    EXPECT_EQ(a.number_of_partitions(), n);
}

TEST(PartitionArrayExTest, SwapViaADLArray)
{
    AoL::PartitionArray<int, kPack4> a{ 1, 2, 3, 4 };
    AoL::PartitionArray<int, kPack4> b{ 10, 20, 30, 40 };
    a.create_partition(1, false);
    b.create_partition(kPartitionMedium, false);
    swap(a, b);
    EXPECT_EQ(a.get_partition(0).max_size(), kPartitionMedium);
    EXPECT_EQ(b.get_partition(0).max_size(), 1u);
    std::swap(a, b);
    EXPECT_EQ(a.get_partition(0).max_size(), 1u);
}

TEST(PartitionArrayExTest, SwapArrayWithEmptyPartitions)
{
    AoL::PartitionArray<int, kPack4> a{ 1, 2, 3, 4 };
    AoL::PartitionArray<int, kPack4> b;
    a.create_partition(kPartitionSmall, false);
    a.get_partition(0).clear();
    const auto a0_max = a.get_partition(0).max_size();
    b.swap(a);
    EXPECT_TRUE(b.get_partition(0).empty());
    EXPECT_EQ(b.get_partition(0).max_size(), a0_max);
}

#endif // AOL_TEST_CONTAINERS_PARTITION
