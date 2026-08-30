/********************************************************************
* Partition container tests
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

// ===================================================================
// REFERENCE MODEL
// ===================================================================
// Mirrors Internal partition mechanics 1:1 on a std::deque so tests and
// fuzzers can assert exact observable state:
//   - raw storage only moves via rotations caused by pop_front/erase
//     inside a window
//   - windows tile contiguously: begin_{i+1} == end_i
//   - live elements occupy the FRONT of each window; slack trails behind
//   - erase_partition shifts later windows left by the erased partition's
//     live size (window length preserved)
// ===================================================================

namespace
{

struct RefWin
{
    AoL::SizeT begin;
    AoL::SizeT size;
    AoL::SizeT max; // end_offset - begin_offset
};

class RefModel
{
public:
    std::deque<int> raw;
    std::vector<RefWin> wins;

    void Reset(AoL::SizeT begin, AoL::SizeT size, AoL::SizeT max)
    {
        raw.clear();
        wins.assign(1, RefWin{ begin, size, max });
    }

    RefWin& Win(AoL::SizeT i) { return wins[i]; }
    RefWin& Back() { return wins.back(); }

    void PushBack(AoL::SizeT i, int v)
    {
        const RefWin& w = wins[i];
        raw[(size_t)(w.begin + w.size)] = v;
        wins[i].size++;
    }

    void GrowRaw(int v) // vector hosts only: extends default window slack
    {
        raw.push_back(v);
        // Header sets back.end_offset = container.size() unconditionally, so
        // the back window's max snaps forward across dead space left by any
        // earlier erase_partition gaps. Recompute; do NOT just increment.
        wins.back().max = TotalRaw() - wins.back().begin;
    }

    void PopFront(AoL::SizeT i)
    {
        const RefWin& w = wins[i];
        std::rotate(
            raw.begin() + (ptrdiff_t)w.begin,
            raw.begin() + (ptrdiff_t)(w.begin + 1),
            raw.begin() + (ptrdiff_t)(w.begin + w.size));
        wins[i].size--;
    }

    void Erase(AoL::SizeT i, AoL::SizeT at, AoL::SizeT count)
    {
        const RefWin& w = wins[i];
        std::rotate(
            raw.begin() + (ptrdiff_t)(w.begin + at),
            raw.begin() + (ptrdiff_t)(w.begin + at + count),
            raw.begin() + (ptrdiff_t)(w.begin + w.size));
        wins[i].size -= count;
    }

    // Mirrors create_partition(n, start_empty) splitting the BACK window.
    void CreateByCount(AoL::SizeT n, bool start_empty)
    {
        const RefWin back = wins.back();
        const AoL::SizeT old_size = back.size;
        const bool smaller = old_size <= n;

        wins.back() = RefWin{ back.begin, start_empty ? 0 : (smaller ? old_size : n), n };

        // Header semantics: the new partition's starting size is
        // has_smaller_old_size ? 0 : old_parti_size - partition_size,
        // REGARDLESS of start_empty. With start_empty=true the overflow
        // elements still land in the new partition; only the old default
        // window is emptied.
        const AoL::SizeT fresh_size = (!smaller) ? (old_size - n) : 0;
        wins.push_back(RefWin{ back.begin + n, fresh_size, 0 });
        wins.back().max = TotalRaw() - wins.back().begin;
    }

    // Mirrors create_partition(pred, stable): caller has ALREADY applied the
    // identical std::partition/stable_partition to model.raw's window slice,
    // so only the bookkeeping split remains.
    void CreateByPredicate(AoL::SizeT n_true)
    {
        CreateByCount(n_true, false);
    }

    void ErasePartition(AoL::SizeT i)
    {
        const AoL::SizeT shift = wins[i].size;
        for (size_t j = i + 1; j < wins.size(); ++j)
        {
            wins[j].begin -= shift;
        }
        wins.erase(wins.begin() + (ptrdiff_t)i);
    }

    AoL::SizeT TotalRaw() const { return (AoL::SizeT)raw.size(); }

    template<typename HOST>
    void ExpectMatches(const HOST& host) const
    {
        ASSERT_EQ(host.sub_partitions.size(), wins.size());
        // NOTE: windows are NOT assumed contiguous here: erase_partition can
        // leave gaps when the victim carried dead slack (shift is by live
        // size, not window width).
        for (size_t i = 0; i < wins.size(); ++i)
        {
            const auto& sp = host.sub_partitions[i];
            SCOPED_TRACE(testing::Message() << "partition " << i);
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

// Non-trivially-copyable probe: IsCheapToCopy == false, so SubPartition gains
// the value_type&& push_back overload (ints route through the by-value one).
struct BigProbe
{
    int v = 0;
    std::string tag;

    explicit BigProbe(int x) :
        v{ x },
        tag{ "probe" }
    {
    }
};

static_assert(AoL::Traits::IsCheapToCopy<int>, "int must route through the by-value push_back");
static_assert(!AoL::Traits::IsCheapToCopy<BigProbe>, "BigProbe must enable the rvalue push_back overload");

using TestPV = AoL::PartitionVector<int>;
using TestPA = AoL::PartitionArray<int, 8>;
using FuzzPA = AoL::PartitionArray<int, 64>;

// ===================================================================
// SUITE: SUB-PARTITION WINDOW SEMANTICS (vector-backed host)
// ===================================================================

// Verifies create_partition(n, false) splits the first n slots off the
// default partition keeping both halves' visible contents correct.
// Importance: the offset bookkeeping here underpins every other operation.
TEST(SubPartitionExTest, CreateSplitsDefaultWindowInPlace)
{
    TestPV pv{ 1, 2, 3, 4, 5 };
    auto& p0 = pv.create_partition(2, false);

    EXPECT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(p0.size(), 2u);
    EXPECT_EQ(p0.max_size(), 2u);
    EXPECT_EQ(pv.get_default_partition().size(), 3u);
    EXPECT_EQ(pv.get_default_partition().max_size(), 3u);
    EXPECT_EQ(p0[0], 1);
    EXPECT_EQ(p0[1], 2);
    EXPECT_EQ(pv.get_default_partition()[0], 3);
    EXPECT_EQ(pv.get_default_partition()[2], 5);
}

// Verifies push_back accepts lvalues and, for non-cheap-to-copy types,
// rvalues, appending at the window's live end. Importance: primary write
// path; also proves overload selection follows ConstRefOrCopyType.
TEST(SubPartitionExTest, PushBackAcceptsLvaluesAndRvalues)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p1 = pv.create_partition(2);

    const int lv = 7;
    EXPECT_TRUE(p1.push_back(lv));
    EXPECT_EQ(p1[0], 7);
    EXPECT_EQ(p1.size(), 1u);

    AoL::PartitionVector<BigProbe> pb;
    pb.emplace_back(BigProbe{ 1 });
    auto& dp = pb.get_default_partition();
    BigProbe source{ 42 };
    EXPECT_TRUE(dp.push_back(std::move(source)));
    EXPECT_EQ(dp.back().v, 42);
}

// Verifies a full partition rejects pushes with false and stays untouched.
// Importance: documented no-op contract; callers rely on the boolean instead
// of exceptions or truncation.
TEST(SubPartitionExTest, PushBackRejectsWhenFull)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p1 = pv.create_partition(2);

    EXPECT_TRUE(p1.push_back(10));
    EXPECT_TRUE(p1.push_back(11));
    ASSERT_TRUE(p1.full());
    EXPECT_FALSE(p1.push_back(12));
    EXPECT_EQ(p1.size(), 2u);
    EXPECT_EQ(p1[0], 10);
    EXPECT_EQ(p1[1], 11);
}

// Verifies emplace_back returns a valid pointer to the constructed element,
// or nullptr when full. Importance: in-place contract used by callers that
// branch on success without checking full() first.
TEST(SubPartitionExTest, EmplaceBackReturnsPointerOrNullptr)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p1 = pv.create_partition(2);

    int* slot = p1.emplace_back(99);
    ASSERT_NE(slot, nullptr);
    EXPECT_EQ(*slot, 99);
    EXPECT_EQ(slot, std::addressof(p1.back()));

    p1.emplace_back(100);
    EXPECT_EQ(p1.emplace_back(101), nullptr);
    EXPECT_EQ(p1.size(), 2u);
}

// Verifies pop_back shrinks without moving data while pop_front rotates the
// window's live range left. Importance: different costs and different effects
// on raw storage; confusing them corrupts sibling partitions' views.
TEST(SubPartitionExTest, PopBackShrinksPopFrontRotates)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6 };
    auto& p0 = pv.create_partition(4, false);
    auto& def = pv.get_default_partition();

    p0.pop_back();
    EXPECT_EQ(p0.size(), 3u);
    EXPECT_EQ(p0[2], 3);
    EXPECT_EQ(pv[3], 4); // popped slot still physically present

    def.pop_front();
    EXPECT_EQ(def.size(), 1u);
    EXPECT_EQ(def[0], 6);
    EXPECT_EQ(pv[4], 6); // rotation moved 6 into the window front
    EXPECT_EQ(pv[3], 4); // P0's popped slot: dead but untouched
}

// Verifies single-index erase rotates only the tail after the erased slot and
// preserves relative order of survivors. Importance: most-used removal op;
// must not leak writes past the partition's own window.
TEST(SubPartitionExTest, EraseSingleShiftsTailWithinWindow)
{
    TestPV pv{ 10, 20, 30, 40 };
    auto& dp = pv.get_default_partition();

    dp.erase(1);
    EXPECT_EQ(dp.size(), 3u);
    EXPECT_EQ(dp[0], 10);
    EXPECT_EQ(dp[1], 30);
    EXPECT_EQ(dp[2], 40);
}

// Verifies ranged erase removes a contiguous run in one rotation.
// Importance: bulk variant must match repeated single erases in result while
// staying a single shift.
TEST(SubPartitionExTest, EraseRangeRemovesContiguousRun)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6 };
    auto& dp = pv.get_default_partition();

    dp.erase(1, 3);
    EXPECT_EQ(dp.size(), 3u);
    EXPECT_EQ(dp[0], 1);
    EXPECT_EQ(dp[1], 5);
    EXPECT_EQ(dp[2], 6);
}

// Verifies clear() empties the partition logically while raw storage keeps
// its values, and the window is reusable afterwards. Importance: cheap-reset
// contract; hidden elements stay reachable through main-container accessors.
TEST(SubPartitionExTest, ClearHidesElementsNotDestroysThem)
{
    TestPV pv{ 1, 2, 3 };
    auto& dp = pv.get_default_partition();

    dp.clear();
    EXPECT_TRUE(dp.empty());
    EXPECT_EQ(dp.size(), 0u);
    EXPECT_EQ(pv.size(), 3u);
    EXPECT_EQ(pv[0], 1); // raw storage untouched

    EXPECT_TRUE(dp.push_back(9));
    EXPECT_EQ(dp[0], 9);
}

// Verifies forward, const, and reverse iteration yield exactly the subscript
// order. Importance: iterators are raw-container iterators offset by the
// window; a mismatch breaks range-for and std algorithms silently.
TEST(SubPartitionExTest, IteratorsMatchSubscriptOrder)
{
    TestPV pv{ 1, 2, 3, 4 };
    auto& p0 = pv.create_partition(2, false);
    const auto& cdp = pv.get_default_partition();

    int sum = 0;
    for (int v : p0)
    {
        sum += v;
    }
    EXPECT_EQ(sum, 3); // 1 + 2

    sum = 0;
    for (const int v : cdp)
    {
        sum += v;
    }
    EXPECT_EQ(sum, 7); // 3 + 4

    std::deque<int> seen;
    for (auto it = p0.rbegin(); it != p0.rend(); ++it)
    {
        seen.push_back(*it);
    }
    EXPECT_EQ(seen, (std::deque<int>{ 2, 1 }));
    EXPECT_TRUE(p0.crbegin().base() == p0.cend());
}

// Verifies front/back agree with subscripts for mutable and const views.
// Importance: convenience accessors must respect the live window, not the
// raw bounds.
TEST(SubPartitionExTest, FrontBackTrackLiveWindow)
{
    TestPV pv{ 5, 6, 7, 8 };
    auto& p1 = pv.create_partition(2, false);
    const auto& cp1 = pv.get_partition(0);

    EXPECT_EQ(p1.front(), 5);
    EXPECT_EQ(p1.back(), 6);
    EXPECT_EQ(cp1.front(), 5);
    EXPECT_EQ(cp1.back(), 6);

    p1.front() = 50;
    EXPECT_EQ(cp1[0], 50);
}

// Verifies a staged (start_empty) partition reports clean state and accepts
// pushes afterwards. Importance: staging new regions is the standard
// producer/consumer pattern over this structure.
TEST(SubPartitionExTest, CreatedEmptyPartitionIsUsable)
{
    TestPV pv{ 1, 2, 3, 4, 5 };
    auto& staged = pv.create_partition(3);

    EXPECT_TRUE(staged.empty());
    EXPECT_EQ(staged.size(), 0u);
    EXPECT_EQ(staged.max_size(), 3u);
    EXPECT_FALSE(staged.full());

    for (int i = 0; i < 3; ++i)
    {
        EXPECT_TRUE(staged.push_back(i));
    }
    EXPECT_TRUE(staged.full());
    EXPECT_FALSE(staged.push_back(9));
}

// Documents that whole-container accessors (operator[], front/back, size,
// iteration) bypass partitioning and expose raw storage including hidden
// slots. Importance: intentional escape hatch; readers must know these reads
// are NOT limited to live partition contents.
TEST(SubPartitionExTest, ContainerAccessorsExposeRawStorage)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6 };
    auto& p0 = pv.create_partition(2, false);
    pv.get_default_partition().clear();

    EXPECT_EQ((AoL::SizeT)(p0.size() + pv.get_default_partition().size()), 2u); // visible only
    EXPECT_EQ(pv.size(), 6u);                                                   // raw still 6
    EXPECT_EQ(pv[5], 6);
    EXPECT_EQ(pv.back(), 6);
    EXPECT_EQ(pv.front(), 1);
}

// Verifies erase_partition slides later windows left by the erased
// partition's LIVE size; contents follow the windows, not element identity.
// Importance: pins the real mechanic (later partitions observe shifted-over
// raw bytes) so nobody assumes a value-preserving merge.
TEST(SubPartitionExTest, ErasePartitionSlidesLaterWindowsLeft)
{
    TestPV pv{ 1, 2, 3, 4, 5 };
    pv.create_partition(2, false); // P0=[1,2]  P1=[3,4,5]
    pv.create_partition(2, false); // P0=[1,2]  P1=[3,4]  P2=[5]

    pv.erase_partition(1);

    ASSERT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(pv.size_of_partition(1), 1u);  // window slid left, length kept
    EXPECT_EQ(pv.get_partition(1)[0], 3);    // sees former P1 leading byte
    EXPECT_EQ(pv[0], 1);                     // raw storage untouched
    EXPECT_EQ(pv[4], 5);
}

// ===================================================================
// SUITE: PARTITION VECTOR LIFECYCLE
// ===================================================================

// Verifies default construction yields an empty container with exactly one
// empty default partition. Importance: class invariant #1; every method
// assumes a default partition exists.
TEST(PartitionVectorExTest, DefaultStartsWithOneEmptyDefault)
{
    TestPV pv;
    EXPECT_TRUE(pv.empty());
    EXPECT_EQ(pv.size(), 0u);
    EXPECT_EQ(pv.number_of_partitions(), 1u);
    EXPECT_TRUE(pv.get_default_partition().empty());
    EXPECT_EQ(pv.get_default_partition().max_size(), 0u);
}

// Verifies initializer-list construction produces one default partition
// spanning the whole payload. Importance: most common construction path.
TEST(PartitionVectorExTest, InitListCoversEverythingWithOneDefault)
{
    TestPV pv{ 10, 20, 30 };
    EXPECT_EQ(pv.size(), 3u);
    EXPECT_EQ(pv.number_of_partitions(), 1u);
    EXPECT_EQ(pv.size_of_partition(0), 3u);
    EXPECT_EQ(pv.get_default_partition()[2], 30);
}

// Verifies count and count+value constructors mark the full range visible.
// Importance: parity with AoL::Vector constructors keeps drop-in usage
// predictable.
TEST(PartitionVectorExTest, CountAndFillCtorsSpanFullRange)
{
    TestPV a(4);
    EXPECT_EQ(a.size(), 4u);
    EXPECT_EQ(a.size_of_partition(0), 4u);

    // The SizeT cast is load-bearing: with bare int literals the iterator-pair
    // template ctor matches exactly (It = int) and beats the count/value ctor
    // (int -> SizeT is only a conversion), then fails to compile. Known header
    // sharp edge; cast to route through the intended constructor.
    TestPV b((AoL::SizeT)4, 7);
    EXPECT_EQ(b.size(), 4u);
    for (AoL::SizeT i = 0; i < b.size(); ++i)
    {
        EXPECT_EQ(b[i], 7);
    }
}

// Verifies the iterator-pair constructor accepts foreign sources and wraps
// them in one default partition. Importance: documented "any valid iterator
// of the same value_type" ingestion path.
TEST(PartitionVectorExTest, IteratorPairCtorTakesAnySource)
{
    std::deque<int> src{ 1, 2, 3, 4 };
    TestPV pv(src.begin(), src.end());

    EXPECT_EQ(pv.size(), 4u);
    EXPECT_EQ(pv.number_of_partitions(), 1u);
    EXPECT_EQ(pv[0], 1);
    EXPECT_EQ(pv[3], 4);
}

// Verifies container-level push_back/emplace_back grow storage and extend
// the default window WITHOUT making elements visible, and that the next
// partition push consumes the reserved slot in place. Importance: pins the
// reserve-vs-fill design; assuming visibility causes phantom-element bugs.
TEST(PartitionVectorExTest, ContainerPushBackExtendsDefaultSlackOnly)
{
    TestPV pv{ 1, 2 };
    pv.push_back(3);
    pv.emplace_back(4);

    EXPECT_EQ(pv.size(), 4u);
    auto& dp = pv.get_default_partition();
    EXPECT_EQ(dp.size(), 2u);     // still only originals visible
    EXPECT_EQ(dp.max_size(), 4u); // window grew though

    EXPECT_TRUE(dp.push_back(30)); // consume reserved slack
    EXPECT_EQ(dp.size(), 3u);
    EXPECT_EQ(dp.back(), 30);
    EXPECT_EQ(pv[2], 30);          // reserved slot was overwritten in place
}

// Verifies assign() replaces storage and collapses back to one full-span
// default partition. Importance: recovery/reset path after clear_all or for
// wholesale reloads.
TEST(PartitionVectorExTest, AssignResetsToSingleDefault)
{
    TestPV pv{ 1, 2, 3, 4 };
    pv.create_partition(2, false);

    pv.assign(TestPV::container_type{ 9, 8, 7 });
    EXPECT_EQ(pv.number_of_partitions(), 1u);
    EXPECT_EQ(pv.size(), 3u);
    EXPECT_EQ(pv.size_of_partition(0), 3u);
    EXPECT_EQ(pv[0], 9);
}

// Verifies reserve passes through to the underlying vector and leaves
// partition bookkeeping untouched. Importance: allocation control promise.
TEST(PartitionVectorExTest, ReserveOnlyGrowsCapacity)
{
    TestPV pv{ 1, 2, 3 };
    pv.reserve(64);
    EXPECT_GE(pv.container_obj.capacity(), 64u);
    EXPECT_EQ(pv.size(), 3u);
    EXPECT_EQ(pv.size_of_partition(0), 3u);
}

// Verifies resizing up extends only the default window (slack), matching
// container push_back semantics. Importance: growth must not silently change
// what any partition reports as visible.
TEST(PartitionVectorExTest, ResizeUpExtendsDefaultSlackOnly)
{
    TestPV pv{ 1, 2 };
    pv.resize(5);

    EXPECT_EQ(pv.size(), 5u);
    auto& dp = pv.get_default_partition();
    EXPECT_EQ(dp.size(), 2u);
    EXPECT_EQ(dp.max_size(), 5u);
    EXPECT_EQ(pv[4], 0); // vector zero-fills new slots
}

// Verifies resizing down drops partitions fully beyond the cut and truncates
// the straddling one while lower partitions stay intact.
// Importance: only API that removes multiple partitions implicitly; an
// off-by-one strands records or hides live data.
TEST(PartitionVectorExTest, ResizeDownDropsAndTruncatesPartitions)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6, 7, 8 };
    pv.create_partition(3, false); // [1..3][4..8]
    pv.create_partition(2, false); // [1..3][4..5][6..8]

    ASSERT_EQ(pv.number_of_partitions(), 3u);
    pv.resize(5);                  // cut mid-second-partition

    EXPECT_EQ(pv.size(), 5u);
    EXPECT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(pv.size_of_partition(0), 3u);
    EXPECT_EQ(pv.size_of_partition(1), 2u);
    EXPECT_EQ(pv.get_partition(1)[0], 4);
    EXPECT_EQ(pv.get_partition(1)[1], 5);
}

// Verifies clear_partitions zeroes every partition's visible size but keeps
// records and raw storage. Importance: structural reset preserving the
// tiling so new staging can reuse the same shape.
TEST(PartitionVectorExTest, ClearPartitionsKeepsShapeAndStorage)
{
    TestPV pv{ 1, 2, 3, 4 };
    pv.create_partition(2, false);

    pv.clear_partitions();
    EXPECT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(pv.size(), 4u);
    EXPECT_EQ(pv.size_of_partition(0), 0u);
    EXPECT_EQ(pv.size_of_partition(1), 0u);
    EXPECT_EQ(pv[3], 4);
}

// Documents that clear_all() leaves ZERO partition records. Importance:
// get_default_partition() afterwards asserts; callers must assign() to
// recover. Pins the sharp edge so any future fix updates this deliberately.
TEST(PartitionVectorExTest, ClearAllLeavesNoDefaultUntilAssign)
{
    TestPV pv{ 1, 2, 3 };
    pv.clear_all();
    EXPECT_EQ(pv.number_of_partitions(), 0u);

    pv.assign(TestPV::container_type{});
    EXPECT_EQ(pv.number_of_partitions(), 1u);
    EXPECT_TRUE(pv.get_default_partition().empty());
}

// Verifies deep copy: independent storage, windows rebound to the copy's
// container, layout identical. Importance: dangling parent_storage pointers
// here would corrupt the source on writes through the copy.
TEST(PartitionVectorExTest, CopyRebindsWindowsToNewStorage)
{
    TestPV src{ 1, 2, 3, 4, 5 };
    src.create_partition(2, false);

    TestPV dst{ src };
    ASSERT_EQ(dst.number_of_partitions(), 2u);
    EXPECT_EQ(dst.get_partition(0)[0], 1);
    EXPECT_EQ(dst.get_partition(1)[0], 3);

    dst.get_partition(0)[0] = 100;
    dst.get_default_partition().pop_back();
    EXPECT_EQ(src.get_partition(0)[0], 1);          // source untouched
    EXPECT_EQ(src.size(), 5u);
    EXPECT_EQ(dst.size(), 5u);                      // sub-partition pop never shrinks raw storage
    EXPECT_EQ(dst.get_default_partition().size(), 2u); // only its window shrank
}

// Verifies move steals storage/rebinding and leaves the source with one
// fresh empty default partition. Importance: moved-from must remain usable
// ("valid but empty state") or move pipelines break.
TEST(PartitionVectorExTest, MoveLeavesSourceValidButEmpty)
{
    TestPV src{ 1, 2, 3, 4 };
    src.create_partition(2, false);

    TestPV dst{ std::move(src) };
    EXPECT_EQ(dst.number_of_partitions(), 2u);
    EXPECT_EQ(dst.get_partition(0).size(), 2u);
    EXPECT_EQ(dst.get_partition(1)[0], 3);

    EXPECT_EQ(src.number_of_partitions(), 1u);
    EXPECT_TRUE(src.get_default_partition().empty());
    EXPECT_EQ(src.size(), 0u);

    src = std::move(dst);
    EXPECT_EQ(src.number_of_partitions(), 2u);
    EXPECT_EQ(dst.number_of_partitions(), 1u);
    EXPECT_TRUE(dst.get_default_partition().empty());
}

// Verifies stable predicate partitioning splits the default in place with
// relative order preserved inside each group, trues staying in the old
// default. Importance: core high-level feature; ordering guarantees are why
// callers pay for stable mode.
TEST(PartitionVectorExTest, PredicateCreateStablePreservesGroupOrder)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6 };
    auto& evens = pv.create_partition([](const int& x) { return (x % 2) == 0; });

    ASSERT_EQ(pv.number_of_partitions(), 2u);
    EXPECT_EQ(evens.size(), 3u);
    EXPECT_EQ(evens[0], 2);
    EXPECT_EQ(evens[1], 4);
    EXPECT_EQ(evens[2], 6);

    auto& odds = pv.get_default_partition();
    ASSERT_EQ(odds.size(), 3u);
    EXPECT_EQ(odds[0], 1);
    EXPECT_EQ(odds[1], 3);
    EXPECT_EQ(odds[2], 5);
}

// Verifies unstable mode groups identically; order within groups is
// unspecified, so compare sorted. Importance: locks grouping semantics
// without over-constraining implementation freedom.
TEST(PartitionVectorExTest, PredicateCreateUnstableGroupsCorrectly)
{
    TestPV pv{ 6, 1, 5, 2, 4, 3 };
    auto& smalls = pv.create_partition(
        [](const int& x) { return x <= 3; },
        false);

    ASSERT_EQ(smalls.size(), 3u);
    std::deque<int> got(smalls.begin(), smalls.end());
    std::sort(got.begin(), got.end());
    EXPECT_EQ(got, (std::deque<int>{ 1, 2, 3 }));

    auto& bigs = pv.get_default_partition();
    ASSERT_EQ(bigs.size(), 3u);
    got.assign(bigs.begin(), bigs.end());
    std::sort(got.begin(), got.end());
    EXPECT_EQ(got, (std::deque<int>{ 4, 5, 6 }));
}

// Verifies chained creates tile the storage with contiguous non-overlapping
// windows whose max_sizes sum to the container size.
// Importance: tiling is THE invariant the design rests on.
TEST(PartitionVectorExTest, ChainedCreatesTileStorageExactly)
{
    TestPV pv{ 1, 2, 3, 4, 5, 6, 7, 8 };
    pv.create_partition(2, false); // [1,2][3..8]
    pv.create_partition(3, false); // [1,2][3,4,5][6..8]

    ASSERT_EQ(pv.number_of_partitions(), 3u);
    AoL::SizeT covered = 0;
    for (AoL::SizeT i = 0; i < pv.number_of_partitions(); ++i)
    {
        covered += pv.get_partition(i).max_size();
    }
    EXPECT_EQ(covered, pv.size());
    EXPECT_EQ(pv.get_partition(0)[1], 2);
    EXPECT_EQ(pv.get_partition(1)[0], 3);
    EXPECT_EQ(pv.get_partition(2)[0], 6);
}

// ===================================================================
// SUITE: PARTITION ARRAY (fixed storage)
// ===================================================================

// Verifies array-backed default construction: full-capacity window, zero
// visible elements. Importance: fixed-storage twin of the vector default;
// capacity is compile-time here.
TEST(PartitionArrayExTest, DefaultHasFullCapacityEmptyWindow)
{
    TestPA pa;
    EXPECT_EQ(pa.size(), 8u);
    EXPECT_EQ(pa.number_of_partitions(), 1u);
    auto& dp = pa.get_default_partition();
    EXPECT_TRUE(dp.empty());
    EXPECT_EQ(dp.max_size(), 8u);
}

// Verifies the fill constructor marks every slot visible in one default
// partition. Importance: bridges raw array init and partition bookkeeping.
TEST(PartitionArrayExTest, FillValueCtorCoversAllSlots)
{
    AoL::PartitionArray<int, 5> pa(7);
    EXPECT_EQ(pa.size_of_partition(0), 5u);
    for (AoL::SizeT i = 0; i < 5; ++i)
    {
        EXPECT_EQ(pa[i], 7);
    }
    EXPECT_TRUE(pa.get_default_partition().full());
}

// Verifies the exact-N argument pack constructor aggregates the array and
// spans the default window fully. Importance: compile-time arity requirement
// (sizeof...(Args) == S) must accept exactly S convertible args.
TEST(PartitionArrayExTest, ElementPackCtorSpansWholeArray)
{
    AoL::PartitionArray<int, 4> pa(1, 2, 3, 4);
    EXPECT_EQ(pa.size_of_partition(0), 4u);
    EXPECT_EQ(pa[0], 1);
    EXPECT_EQ(pa[3], 4);
}

// Verifies iterator-pair construction copies ranges up to capacity.
// Only ranges within S are fed: the constructor sets end_offset to the FULL
// distance, so longer inputs would claim more window than physical storage.
// Importance: documents the input contract callers must uphold.
TEST(PartitionArrayExTest, IteratorPairCtorCopiesUpToCapacity)
{
    std::deque<int> src{ 1, 2, 3 };
    AoL::PartitionArray<int, 8> pa(src.begin(), src.end());
    EXPECT_EQ(pa.size_of_partition(0), 3u);
    EXPECT_EQ(pa[2], 3);

    std::deque<int> exact{ 1, 2, 3, 4 };
    AoL::PartitionArray<int, 4> pb(exact.begin(), exact.end());
    EXPECT_EQ(pb.size_of_partition(0), 4u);
    EXPECT_EQ(pb[3], 4);
}

// Verifies sub-partition operations work identically over fixed storage and
// that full() trips exactly at S. Importance: proves window math is
// storage-agnostic across the vector/array specializations.
TEST(PartitionArrayExTest, WindowOpsOnFixedStorageMatchVectorBehavior)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();

    for (int i = 0; i < 8; ++i)
    {
        EXPECT_NE(dp.emplace_back(i), nullptr);
    }
    EXPECT_TRUE(dp.full());
    EXPECT_EQ(dp.emplace_back(99), nullptr);
    EXPECT_EQ(dp.size(), 8u);

    dp.pop_front();
    EXPECT_EQ(dp.size(), 7u);
    EXPECT_EQ(dp[0], 1);

    dp.erase(2, 2); // drops {3,4} -> {1,2,5,6,7}
    EXPECT_EQ(dp.size(), 5u);
    EXPECT_EQ(dp[2], 5);
}

// Verifies create_partition tiles fixed storage exactly like the vector host.
// Importance: cross-host uniformity is the point of the shared base.
TEST(PartitionArrayExTest, CreatePartitionTilesFixedStorage)
{
    TestPA pa;
    auto& dp = pa.get_default_partition();
    for (int i = 0; i < 6; ++i)
    {
        dp.emplace_back(i);
    }

    auto& p0 = pa.create_partition(3, false);
    EXPECT_EQ(pa.number_of_partitions(), 2u);
    EXPECT_EQ(p0.size(), 3u);
    EXPECT_EQ(p0[0], 0);
    EXPECT_EQ(p0[2], 2);
    EXPECT_EQ(pa.get_default_partition().size(), 3u);
    EXPECT_EQ(pa.get_default_partition()[0], 3);

    AoL::SizeT covered = 0;
    for (AoL::SizeT i = 0; i < pa.number_of_partitions(); ++i)
    {
        covered += pa.get_partition(i).max_size();
    }
    EXPECT_EQ(covered, pa.size());
}

// Verifies array-host copy independence: windows rebind to the copy's fixed
// buffer. Importance: same dangling-pointer risk as the vector flavor.
TEST(PartitionArrayExTest, CopyIsIndependent)
{
    TestPA src;
    src.get_default_partition().push_back(1);
    src.get_default_partition().push_back(2);

    TestPA dst{ src };
    dst.get_default_partition()[0] = 100;
    dst.get_default_partition().clear();

    EXPECT_EQ(src.get_default_partition().size(), 2u);
    EXPECT_EQ(src.get_default_partition()[0], 1);
}

// ===================================================================
// SUITE: TYPE-LEVEL CONTRACTS
// ===================================================================

// Verifies iterator categories and aliases surface correctly.
// Importance: contiguous_iterator enables pointer-style interop callers depend
// on; breaking aliases breaks generic code at compile time, cheapest caught
// here.
TEST(PartitionTypeTest, IteratorTraitsAreContiguous)
{
    using Sub = TestPV::sub_partition_type;

    static_assert(std::contiguous_iterator<typename Sub::iterator>);
    static_assert(std::contiguous_iterator<typename Sub::const_iterator>);
    static_assert(std::same_as<typename Sub::value_type, int>);
    static_assert(std::same_as<typename Sub::size_type, AoL::SizeT>);
    static_assert(std::contiguous_iterator<typename TestPV::iterator>);
    static_assert(std::contiguous_iterator<typename TestPA::iterator>);

    TestPV pv{ 1, 2, 3 };
    auto it = pv.get_default_partition().begin();
    static_assert(std::contiguous_iterator<decltype(it)>);
    EXPECT_EQ(it[2], 3);
}

// ===================================================================
// SUITE: MODEL-CHECKED FUZZ
// ===================================================================
// Drives a host through thousands of mixed operations while a RefModel
// replays identical mechanics on std::deque; full state compared every step.
// Importance: safety net for interaction bugs (erase + split + slide
// combinations) targeted tests cannot enumerate.
// ===================================================================

template<typename HOST, bool CAN_GROW>
void RunPartitionModelLoop(HOST& host, RefModel& model, AoL::SizeT steps, unsigned seed)
{
    unsigned lcg = seed;
    auto roll = [&lcg]() -> unsigned
    {
        lcg = lcg * 1664525u + 1013904223u;
        return (lcg >> 16) & 0x7FF;
    };

    for (AoL::SizeT step = 0; step < steps; ++step)
    {
        SCOPED_TRACE(testing::Message() << "step " << step);
        const unsigned r = roll();
        const AoL::SizeT last = host.number_of_partitions() - 1;
        const AoL::SizeT pick = (r % 4 == 0 && last > 0) ? (roll() % last) : last;
        auto& picked = host.sub_partitions[pick];

        if (r % 100 < 25 && !picked.full())
        {
            const int v = (int)(r % 97);
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
            const bool start_empty = (roll() & 1u) == 1u;
            SCOPED_TRACE(testing::Message()
                << "create n=" << n << " se=" << start_empty);
            host.create_partition(n, start_empty);
            model.CreateByCount(n, start_empty);
        }
        else if (r % 100 < 68 && host.number_of_partitions() > 1)
        {
            const AoL::SizeT victim = roll() % last; // never the default
            host.erase_partition(victim);
            model.ErasePartition(victim);
        }
        else if (r % 100 < 73)
        {
            RefWin& back_model = model.Back();
            if (back_model.size >= 2)
            {
                const bool stable = (roll() & 1u) == 1u;
                auto pred = [](const int& x) { return (x % 2) != 0; };

                // Dry-run the split on a copy first: only when the resulting
                // count is strictly interior do we commit BOTH sides, keeping
                // model and host in lockstep.
                std::vector<int> probe(
                    model.raw.begin() + (ptrdiff_t)back_model.begin,
                    model.raw.begin() + (ptrdiff_t)(back_model.begin + back_model.size));
                auto psplit = stable
                    ? std::stable_partition(probe.begin(), probe.end(), pred)
                    : std::partition(probe.begin(), probe.end(), pred);
                const AoL::SizeT n_true = (AoL::SizeT)(psplit - probe.begin());

                if (n_true > 0 && n_true < back_model.size)
                {
                    auto first = model.raw.begin() + (ptrdiff_t)back_model.begin;
                    auto last_it = first + (ptrdiff_t)back_model.size;
                    if (stable)
                    {
                        std::stable_partition(first, last_it, pred);
                    }
                    else
                    {
                        std::partition(first, last_it, pred);
                    }
                    host.create_partition(pred, stable);
                    model.CreateByPredicate(n_true);
                }
            }
        }
        else if constexpr (CAN_GROW)
        {
            if (r % 100 < 78 && model.TotalRaw() < 128)
            {
                const int v = (int)(r % 53);
                host.push_back(v);
                model.GrowRaw(v);
            }
        }

        // Full compare early (catches seeding/sync bugs fast), then sample
        // every 4th step to keep Debug-build runtime sane.
        if (step < 256 || step % 4 == 0)
        {
            model.ExpectMatches(host);
        }
    }
}

// Vector host fuzz: growth, churn, predicate splits, window slides over 5000
// mixed ops. Importance: broad-spectrum regression net for PartitionVectorEx.
TEST(PartitionFuzzTest, FuzzAgainstDequeModel_Vector)
{
    TestPV host;
    for (int i = 1; i <= 6; ++i)
    {
        host.push_back(i);
        host.get_default_partition().push_back(i);
    }

    RefModel model;
    model.Reset(0, 6, 6);
    for (int i = 1; i <= 6; ++i)
    {
        model.raw.push_back(i);
    }

    RunPartitionModelLoop<TestPV, true>(host, model, 5000, 0xC0FFEEu);
}

// Array host fuzz: same harness over fixed storage (starts fully visible,
// never grows). Importance: proves specialization parity under stress.
TEST(PartitionFuzzTest, FuzzAgainstDequeModel_Array)
{
    FuzzPA host(0); // all 64 slots visible, zero-filled

    RefModel model;
    model.Reset(0, 64, 64);
    model.raw.assign(64, 0);

    RunPartitionModelLoop<FuzzPA, false>(host, model, 2000, 0xF00Du);
}

} // namespace

#endif // AOL_TEST_CONTAINERS_PARTITION
