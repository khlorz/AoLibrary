#include "all_benchmarks.h"

#include "aol/aol.h"
#include "boost/container/map.hpp"

#include <map>
#include <vector>
#include <unordered_set>
#include <random>

namespace AoL::Benchmark
{

static std::vector<int> BMHelper_GenerateRandomKeys(int N)
{
    std::vector<int> keys(N);
    for (int i = 0; i < N; ++i) keys[i] = i;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(keys.begin(), keys.end(), gen);
    std::unordered_set<int> unique_keys{ std::make_move_iterator(keys.begin()), std::make_move_iterator(keys.end()) };
    keys.insert(keys.end(), std::make_move_iterator(unique_keys.begin()), std::make_move_iterator(unique_keys.end()));

    return keys;
}

void BM_RotLibVector(benchmark::State& state)
{
    const size_t N = state.range(0);

    for (auto _ : state)
    {
        AoL::Vector<int> v;
        //v.reserve(N); // remove reallocation noise if you want

        for (size_t i = 0; i < N; ++i)
            v.push_back(static_cast<int>(i));

        benchmark::DoNotOptimize(v.data());
    }
}

void BM_STDVector(benchmark::State& state)
{
    const size_t N = state.range(0);

    for (auto _ : state)
    {
        std::vector<int> v;
        //v.reserve(N); // remove reallocation noise if you want

        for (size_t i = 0; i < N; ++i)
            v.push_back(static_cast<int>(i));

        benchmark::DoNotOptimize(v.data());
    }
}

void BM_RotLibString(benchmark::State& state)
{
    const size_t N = state.range(0);

    for (auto _ : state)
    {
        AoL::String v;
        //v.reserve(N); // remove reallocation noise if you want

        v.reserve(N);
    }
}

void BM_STDString(benchmark::State& state)
{
    const size_t N = state.range(0);

    for (auto _ : state)
    {
        std::string v;
        //v.reserve(N); // remove reallocation noise if you want

        v.reserve(N);
    }
}

void BM_RotLibMap(benchmark::State& state)
{
    const int N = state.range(0);
    auto keys = BMHelper_GenerateRandomKeys(N); // generate random keys

    for (auto _ : state) {
        state.PauseTiming();
        AoL::KeyOrderMap<int, int> m;
        state.ResumeTiming();

        for (int i = 0; i < keys.size(); ++i) {
            m[keys[i]] = i;
        }

        benchmark::DoNotOptimize(m);
    }
}

void BM_STDMap(benchmark::State& state)
{
    const int N = state.range(0);
    auto keys = BMHelper_GenerateRandomKeys(N); // generate random keys

    for (auto _ : state) {
        state.PauseTiming();
        std::map<int, int> m;
        state.ResumeTiming();

        for (int i = 0; i < keys.size(); ++i) {
            m[keys[i]] = i;
        }

        benchmark::DoNotOptimize(m);
    }
}

void BM_BoostMap(benchmark::State& state)
{
    const int N = state.range(0);
    auto keys = BMHelper_GenerateRandomKeys(N); // generate random keys

    for (auto _ : state) {
        state.PauseTiming();
        boost::container::map<int, int> m;
        state.ResumeTiming();

        for (int i = 0; i < keys.size(); ++i) {
            m[keys[i]] = i;
        }

        benchmark::DoNotOptimize(m);
    }
}

void BM_RotLibMapFind(benchmark::State& state)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    const int N = state.range(0);
    auto keys = BMHelper_GenerateRandomKeys(N); // generate random keys
    AoL::KeyOrderMap<int, int> m;
    for (int i = 0; i < keys.size(); ++i) {
        m[keys[i]] = i;
    }

    for (auto _ : state) {
        state.PauseTiming();
        std::uniform_int_distribution<int> dis(0, keys.size() - 1);
        state.ResumeTiming();

        benchmark::DoNotOptimize(m[keys[dis(gen)]]);
    }
}

void BM_STDMapFind(benchmark::State& state)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    const int N = state.range(0);
    auto keys = BMHelper_GenerateRandomKeys(N); // generate random keys
    std::map<int, int> m;
    for (int i = 0; i < keys.size(); ++i) {
        m[keys[i]] = i;
    }

    for (auto _ : state) {
        state.PauseTiming();
        std::uniform_int_distribution<int> dis(0, keys.size() - 1);
        state.ResumeTiming();

        benchmark::DoNotOptimize(m[keys[dis(gen)]]);
    }
}

void BM_BoostMapFind(benchmark::State& state)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    const int N = state.range(0);
    auto keys = BMHelper_GenerateRandomKeys(N); // generate random keys
    boost::container::map<int, int> m;
    for (int i = 0; i < keys.size(); ++i) {
        m[keys[i]] = i;
    }

    for (auto _ : state) {
        state.PauseTiming();
        std::uniform_int_distribution<> dis(0, keys.size() - 1);
        state.ResumeTiming();

        benchmark::DoNotOptimize(m[keys[dis(gen)]]);
    }
}

}

#define ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(function) \
BENCHMARK(function)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)

ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(AoL::Benchmark::BM_RotLibMapFind);
ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(AoL::Benchmark::BM_STDMapFind);
ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(AoL::Benchmark::BM_BoostMapFind);
