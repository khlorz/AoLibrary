/********************************************************************
* Serialization tests
* - Tests the proper serialization of AoL types
********************************************************************/

#ifndef BENCHMARK_CONFIG_ON

#include "config.h"

#if AOL_TESTS_FLAG(SERIALIZATION)

#include "all_tests.h"
#include "aol/aol.h"
#include "aol/data_components/profiles.h"
#include "aol/data_components/managers.h"
#include "aol/serialization/serialize.h"

#include "cereal/archives/binary.hpp"

#include <sstream>

template<typename T, typename P>
static void SerializeThis(const T& t_value, P&& print_predicate);

namespace AoL
{

void SerializationTests() noexcept
{
	fmt::print("**********Serialization tests**********\n");
	fmt::print("*****Fundamental types*****\n");
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
	SerializeThis(
		std::string("asdasdkjqlkwje"),
		[](auto str)
		{
			fmt::print("string:\n");
			fmt::print("{}\n\n", str);
		}
	);
	fmt::print("*****Container types*****\n");
	SerializeThis(
		AoL::Internal::KeyValuePairEx<int, int>{1, 2},
		[](auto kvp)
		{
			fmt::print("KeyValuePairEx<int, int>\n");
			fmt::print("key = {} | value = {}\n\n", kvp.first, kvp.second);
		}
	);
	{
		AoL::FlatKeyOrderMap<int, int> map{};
		map.build_start();
		map.build_add(214712047, 1279);
		map.build_add(1, 2);
		map.build_add(100, 50);
		map.build_add(50123, 824);
		map.build_add(40, 199);
		map.build_end();
		SerializeThis(
			map,
			[](auto& map)
			{
				fmt::print("KeyOrderMapEx<int, int>\n");
				for (auto kvp : map)
					fmt::print("key = {} | value = {}\n", kvp.first, kvp.second);
				fmt::print("\n");
			}
		);
	}
	{
		fmt::print("*****Data Components*****\n");
		{
			struct DataSample1 : public AoL::DataBasic<DataSample1>
			{
				using AoL::DataBasic<DataSample1>::DataBasic;
			};
			DataSample1 sample;
			sample.name = "SampleName";
			sample.short_name = "Sample";
			sample.description = "A sample description for a sample object";
			sample.id = static_cast<DataSample1::IDT>(1);

			SerializeThis(
				sample,
				[](const DataSample1& data_sample)
				{
					fmt::print("DataSample1 derived from DataBasic\n");
					fmt::print("name: {}\n", data_sample.name);
					fmt::print("short_name: {}\n", data_sample.short_name);
					fmt::print("description: {}\n", data_sample.description);
					fmt::print("id: {}\n", static_cast<SizeT>(data_sample.id));
				}
			);
		}
		{
			struct DataSample2 : public AoL::DataMinimal<DataSample2>
			{
				using AoL::DataMinimal<DataSample2>::DataMinimal;
			};

			DataSample2 sample;
			sample.name = "SampleName";
			sample.id = static_cast<DataSample2::IDT>(2);

			SerializeThis(
				sample,
				[](const DataSample2& data_sample)
				{
					fmt::print("DataSample2 derived from DataMinimal\n");
					fmt::print("name: {}\n", data_sample.name);
					fmt::print("id: {}\n", static_cast<SizeT>(data_sample.id));
				}
			);
		}
	}

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