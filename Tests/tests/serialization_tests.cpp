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
	{
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

	}
	fmt::print("\n");
	{
		fmt::print("*****Container types*****\n");
		SerializeThis(
			AoL::Internal::KeyValuePairEx<int, int>{1, 2},
			[](auto kvp)
			{
				fmt::print("KeyValuePairEx<int, int>:\n");
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
					fmt::print("KeyOrderMapEx<int, int>:\n");
					for (auto kvp : map)
						fmt::print("key = {} | value = {}\n", kvp.first, kvp.second);
					fmt::print("\n");
				}
			);
		}
		{
			AoL::ArrayNamed2<int> arr2;
			arr2.x = 123;
			arr2.y = 5124;
			SerializeThis(
				arr2,
				[](const auto& arr2)
				{
					fmt::print("NamedArray2:\n");
					fmt::print("data_arr: [0] = {}\n", arr2.data_arr[0]);
					fmt::print("data_arr: [1] = {}\n", arr2.data_arr[1]);
					fmt::print("x = {}\n", arr2.x);
					fmt::print("y = {}\n", arr2.y);
				}
			);
			fmt::print("\n");
		}
		{
			AoL::ArrayNamed3<int> arr3;
			arr3.x = 123;
			arr3.y = 5124;
			arr3.z = 492184;
			SerializeThis(
				arr3,
				[](const auto& arr3)
				{
					fmt::print("NamedArray3:\n");
					fmt::print("data_arr: [0] = {}\n", arr3.data_arr[0]);
					fmt::print("data_arr: [1] = {}\n", arr3.data_arr[1]);
					fmt::print("data_arr: [2] = {}\n", arr3.data_arr[2]);
					fmt::print("x = {}\n", arr3.x);
					fmt::print("y = {}\n", arr3.y);
					fmt::print("z = {}\n", arr3.z);
				}
			);
			fmt::print("\n");
		}
		{
			AoL::ArrayNamed4<int> arr4;
			arr4.x = 123;
			arr4.y = 5124;
			arr4.z = 492184;
			arr4.w = 29482109;
			SerializeThis(
				arr4,
				[](const auto& arr4)
				{
					fmt::print("NamedArray4:\n");
					fmt::print("data_arr: [0] = {}\n", arr4.data_arr[0]);
					fmt::print("data_arr: [1] = {}\n", arr4.data_arr[1]);
					fmt::print("data_arr: [2] = {}\n", arr4.data_arr[2]);
					fmt::print("data_arr: [3] = {}\n", arr4.data_arr[3]);
					fmt::print("x = {}\n", arr4.x);
					fmt::print("y = {}\n", arr4.y);
					fmt::print("z = {}\n", arr4.z);
					fmt::print("w = {}\n", arr4.w);
				}
			);
			fmt::print("\n");
		}
		{
			AoL::CyclicBufferF<int, 8> cbf{};
			for (AoL::SizeT i = 0; i < cbf.size() + 4; ++i)
			{
				cbf.push_back(static_cast<int>(i));
			}
			SerializeThis(
				cbf,
				[](const auto& cbf)
				{
					fmt::print("CyclicBufferF:\n");
					for (AoL::SizeT i = 0; i < cbf.size(); ++i)
					{
						fmt::print("[{}] = {}\n", i, cbf[i]);
					}
				}
			);
			fmt::print("\n");
		}
		{
			AoL::CyclicBufferD<int> cbd(8);
			for (AoL::SizeT i = 0; i < cbd.size() + 4; ++i)
			{
				cbd.push_back(static_cast<int>(i));
			}
			SerializeThis(
				cbd,
				[](const auto& cbd)
				{
					fmt::print("CyclicBufferD:\n");
					for (AoL::SizeT i = 0; i < cbd.size(); ++i)
					{
						fmt::print("[{}] = {}\n", i, cbd[i]);
					}
				}
			);
			fmt::print("\n");
		}
		{
			AoL::PartitionVector<int> pv{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
			auto& sp = pv.create_partition(5, false);
			sp.erase(1, 2);
			sp.pop_back();
			pv.create_partition([](int v) { return v % 2 == 0; });
			SerializeThis(
				pv,
				[](const auto& pv)
				{
					fmt::print("PartitionVector:\n");
					for (int i = 0; i < pv.number_of_partitions(); ++i)
					{
						fmt::print("Partition {}\n", i);
						const auto& sub_partition = pv.get_partition(i);
						for (int j = 0; j < sub_partition.size(); ++j)
						{
							fmt::print("[{}] = {}\n", j, sub_partition[j]);
						}
					}
				}
			);
		}
	}
	fmt::print("\n");
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
					fmt::print("DataSample1 derived from DataBasic:\n");
					fmt::print("name: {}\n", data_sample.name);
					fmt::print("short_name: {}\n", data_sample.short_name);
					fmt::print("description: {}\n", data_sample.description);
					fmt::print("id: {}\n", static_cast<SizeT>(data_sample.id));
				}
			);
			fmt::print("\n");
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
					fmt::print("DataSample2 derived from DataMinimal:\n");
					fmt::print("name: {}\n", data_sample.name);
					fmt::print("id: {}\n", static_cast<SizeT>(data_sample.id));
				}
			);
			fmt::print("\n");
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