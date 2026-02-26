#ifdef BENCHMARK_CONFIG_ON

#include "all_benchmarks.h"

#include "aol/aol.h"

#include "absl/strings/str_cat.h"

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

void BM_STDCONCAT(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string str;
        str += "asdasdasd";
        str += std::to_string(123456);
        str += std::to_string(1.41231241245f);
        str += "askdljaslkdn214e1293uiondlasndkjasnd9u12-dk21jkd12";
        benchmark::DoNotOptimize(str);
    }
}

void BM_ABSLCONCAT(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string str = absl::StrCat("asdasdasd", 123456, 1.41231241245f, "askdljaslkdn214e1293uiondlasndkjasnd9u12-dk21jkd12");
        benchmark::DoNotOptimize(str);
    }
}

void BM_FMTCONCAT(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string str = fmt::format("{}{}{}{}","asdasdasd", 123456, 1.41231241245f, "askdljaslkdn214e1293uiondlasndkjasnd9u12-dk21jkd12");
        benchmark::DoNotOptimize(str);
    }
}

void BM_STBCONCAT(benchmark::State& state)
{
    for (auto _ : state)
    {
        char str[512];
        stbsp_sprintf(str, "%s%d%0.10f%s", "asdasdasd", 123456, 1.41231241245f, "askdljaslkdn214e1293uiondlasndkjasnd9u12-dk21jkd12");
        benchmark::DoNotOptimize(str);
    }
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
		for (size_t i = 0; i < N; ++i)
            v.push_back(static_cast<char>(i % 256));

		benchmark::DoNotOptimize(v.data());
    }
}

void BM_STDString(benchmark::State& state)
{
    const size_t N = state.range(0);

    for (auto _ : state)
    {
        std::string v;
        for (size_t i = 0; i < N; ++i)
            v.push_back(static_cast<char>(i % 256));

        benchmark::DoNotOptimize(v.data());
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

}

#define ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(function) \
BENCHMARK(function)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)

//ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(AoL::Benchmark::BM_STDVector);
//ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(AoL::Benchmark::BM_RotLibVector);
//ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(AoL::Benchmark::BM_STDString);
//ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(AoL::Benchmark::BM_RotLibString);
//ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(AoL::Benchmark::BM_STDMap);
//ROTLIB_BENCHMARK_CREATE_CONTAINER_BENCHMARK(AoL::Benchmark::BM_RotLibMap);

BENCHMARK(AoL::Benchmark::BM_STDCONCAT);
BENCHMARK(AoL::Benchmark::BM_ABSLCONCAT);
BENCHMARK(AoL::Benchmark::BM_FMTCONCAT);
BENCHMARK(AoL::Benchmark::BM_STBCONCAT);

#endif