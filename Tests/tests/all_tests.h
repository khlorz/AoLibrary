#pragma once

#ifndef BENCHMARK_CONFIG_ON

#include "config.h"

namespace AoL
{

#if AOL_TESTS_FLAG(BASIC_DATA)
AOL_TESTS_DECLARATION_ON(BasicData)
#else
AOL_TESTS_DECLARATION_OFF(BasicData)
#endif

#if AOL_TESTS_FLAG(GENERIC_MANAGER)
AOL_TESTS_DECLARATION_ON(GenericManager)
#else
AOL_TESTS_DECLARATION_OFF(GenericManager)
#endif

#if AOL_TESTS_FLAG(PARTITION)
AOL_TESTS_DECLARATION_ON(Partition)
#else
AOL_TESTS_DECLARATION_OFF(Partition)
#endif

void RunTests();

}

#endif
