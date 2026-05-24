/********************************************************************
* Serialization tests
* - Tests the proper serialization of AoL types
********************************************************************/

#ifndef BENCHMARK_CONFIG_ON

#include "config.h"

#if AOL_TESTS_FLAG(SERIALIZATION)

#include "all_tests.h"
#include "aol/aol.h"
#include "aol/serialization/serialize.h"

#include "cereal/archives/binary.hpp"

#include <sstream>

template<typename T, typename P>
static void SerializeThis(const T& t_value, P&& print_predicate);

namespace AoL
{

void SerializationTests() noexcept
{
	fmt::print("Serialization tests:\n");
	SerializeThis(
		1,
		[](int i)
		{ 
			fmt::print("int:\n");
			fmt::print("{}\n\n", i);
		}
	);
	SerializeThis(
		69.420f,
		[](float f)
		{
			fmt::print("float:\n");
			fmt::print("{:3f}\n\n", f);
		}
	);
}

}

template<typename T, typename P>
static void SerializeThis(const T& t_value, P&& print_predicate)
{
	std::stringstream data_stream;
	T serialized_in;
	{
		cereal::BinaryOutputArchive bin_out(data_stream);
		bin_out(t_value);
	}
	{
		cereal::BinaryInputArchive bin_in(data_stream);
		bin_in(serialized_in);
	}

	print_predicate(serialized_in);
}

#endif // AOL_TESTS_FLAG(SERIALIZATION)
#endif // BENCHMARK_CONFIG_ON