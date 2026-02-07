/***************************************************
* AoLibrary tests/benchmarks
***************************************************/

#ifdef BENCHMARK_CONFIG_ON
#include "benchmarks/all_benchmarks.h"

BENCHMARK_MAIN();

#else
#include "tests/all_tests.h"

int main()
{
	AoL::RunTests();

	return 0;
}

#endif