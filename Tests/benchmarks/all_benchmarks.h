#pragma once

#ifdef BENCHMARK_CONFIG_ON

#include "google_benchmark/benchmark.h"

namespace AoL::Benchmark
{

void BM_RotLibVector(benchmark::State& state);
void BM_STDVector(benchmark::State& state);

void BM_RotLibString(benchmark::State& state);
void BM_STDString(benchmark::State& state);

void BM_RotLibMap(benchmark::State& state);
void BM_STDMap(benchmark::State& state);

void BM_RotLibMapFind(benchmark::State& state);
void BM_STDMapFind(benchmark::State& state);

void BM_STDCONCAT(benchmark::State& state);
void BM_ABSLCONCAT(benchmark::State& state);
void BM_FMTCONCAT(benchmark::State& state);
void BM_STBCONCAT(benchmark::State& state);

void BM_FMTFORMAT(benchmark::State& state);
void BM_ABSLFORMAT(benchmark::State& state);

}

#endif
