/********************************************************************
* Partition tests
* - Tests for Partition containers
********************************************************************/

#ifndef BENCHMARK_CONFIG_ON

#include "config.h"
#if AOL_TESTS_FLAG(PARTITION)

#include "all_tests.h"
#include "aol/aol.h"

#define AOL_PRINT_NEW_LINE fmt::print("\n");

template<typename P>
void PrintPartition(const AoL::SubPartition<P>& partition)
{
	for (auto& v : partition)
	{
		fmt::print("Value: {}\n", v);
	}
	fmt::print("\n");
}

template<typename It>
void PrintIteration(It begin, It end)
{
	for (auto it = begin; it != end; ++it)
	{
		fmt::print("Value: {}\n", *it);
	}
	fmt::print("\n");
}

void AoL::PartitionTests() noexcept
{
	fmt::print("\nPartition tests\n");
	{
		fmt::print("***********************************\n");
		fmt::print("          PartitionVector          \n");
		fmt::print("***********************************\n");
		{
			fmt::print("Ctors\n");
			Internal::DefaultAllocator<int> allocator_obj{};
			AoL::PartitionVector<int> vp_copy{ 1,2,3,4,5,6,7,8,9,10 };
			AoL::PartitionVector<int> vp_move{ 1,2,3,4,5,6,7,8,9,10 };

			{
				fmt::print("Initializer list: \"AoL::PartitionVector<int> vp1{{ 1,2,3,4,5,6,7,8,9,10 }}\"\n");
				AoL::PartitionVector<int> vp{ 1,2,3,4,5,6,7,8,9,10 };
				PrintPartition(vp.get_partition(0));
			}
			{
				fmt::print("Default initialization: \"AoL::PartitionVector<int> vp();\"\n");
				AoL::PartitionVector<int> vp{};
				PrintPartition(vp.get_partition(0));
			}
			{
				fmt::print("Copy constructor:\n");
				AoL::PartitionVector<int> vp(vp_copy);
				PrintPartition(vp.get_partition(0));
			}
			{
				fmt::print("Move constructor:\n");
				fmt::print("vp_move before:\n");
				PrintPartition(vp_move.get_partition(0));
				AoL::PartitionVector<int> vp(std::move(vp_move));
				fmt::print("vp_move after:\n");
				PrintPartition(vp_move.get_partition(0));
				fmt::print("vp after move:\n");
				PrintPartition(vp.get_partition(0));
			}
			{
				fmt::print("Copy operator =:\n");
				vp_move = vp_copy;
				fmt::print("vp_move new:\n");
				PrintPartition(vp_move.get_partition(0));
			}
			{
				fmt::print("Move operator =:\n");
				AoL::PartitionVector<int> vp = std::move(vp_move);
				fmt::print("vp after move:\n");
				PrintPartition(vp.get_partition(0));
			}
			{
				fmt::print("Iterator initilization:\n");
				AoL::PartitionVector<int> vp(vp_copy.begin(), vp_copy.end());
				PrintPartition(vp.get_partition(0));
			}
			{
				fmt::print("Move Iterator initilization:\n");
				AoL::PartitionVector<std::string> vpstr{ "asdsad", "asdasdasdasd", "qweoihasklndas", "qwhwleh201321", "asdkln21uin 4o2h14" };
				AoL::PartitionVector<std::string> vp(std::make_move_iterator(vpstr.begin()), std::make_move_iterator(vpstr.end()));
				PrintPartition(vp.get_partition(0));
			}
		}

		fmt::print("\n");
		{
			fmt::print("Subpartition creation\n");
			AoL::PartitionVector<int> vp{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
			PrintPartition(vp.create_partition(10, false));
			auto& part = vp.create_partition(
				[](int v)
				{
					return v % 2 == 0;
				}
			);
			PrintPartition(part);
		}

		{
			fmt::print("Subpartition creation with non-'full' default_partition\n");
			AoL::PartitionVector<int> vp{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
			auto& back_partition = vp.get_partition(0);

			back_partition.pop_back();
			back_partition.pop_back();
			back_partition.pop_back();
			back_partition.pop_back();
			back_partition.pop_back();

			PrintPartition(vp.create_partition(20, false));
		}

		{
			fmt::print("Subpartition resizing down\n");
			AoL::PartitionVector<int> vp{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
			vp.create_partition(10);
			auto& partition = vp.create_partition(8, false);

			partition.erase(2, 4);
			vp.resize(13);
			fmt::print("PartitionVector after erasing from 2 to 6, and resizing down to 13\n");
			PrintPartition(vp.get_partition(vp.number_of_partitions() - 1));
		}

		{
			fmt::print("push_back on empty size partition\n");
			AoL::PartitionVector<int> vp{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
			auto& partition = vp.create_partition(10);

			for (int i = 0; i < (int)partition.max_size(); ++i)
			{
				partition.push_back(i);
			}

			PrintPartition(partition);
		}
		fmt::print("\n");
	}
	{
		fmt::print("***********************************\n");
		fmt::print("          PartitionArray           \n");
		fmt::print("***********************************\n");
		{
			fmt::print("Ctors\n");
			AoL::PartitionArray<int, 8> ap_copy{ 8,7,6,5,4,3,2,1 };
			{
				fmt::print("Initializer list ctor:\n");
				AoL::PartitionArray<int, 8> ap{ 1,2,3,4,5,6,7,8 };
				PrintIteration(ap.begin(), ap.end());
			}
			{
				fmt::print("Default ctor\n");
				AoL::PartitionArray<int, 8> ap{};
				PrintIteration(ap.begin(), ap.end());
			}
			{
				fmt::print("Copy ctor\n");
				AoL::PartitionArray<int, 8> ap_new{ ap_copy };
				PrintIteration(ap_new.begin(), ap_new.end());
			}
			{
				fmt::print("Copy operator\n");
				AoL::PartitionArray<int, 8> ap_new;
				ap_new = ap_copy;
				PrintIteration(ap_new.begin(), ap_new.end());
			}
			{
				fmt::print("Move ctor\n");
				AoL::PartitionArray<AoL::String, 8> ap_move{ "1s", "2s", "3s", "4s", "5s", "6s", "7s", "8s" };
				AoL::PartitionArray<AoL::String, 8> ap_new{ std::move(ap_move) };
				PrintIteration(ap_new.begin(), ap_new.end());
			}
			{
				fmt::print("Move operator\n");
				AoL::PartitionArray<AoL::String, 8> ap_move{ "1s", "2s", "3s", "4s", "5s", "6s", "7s", "8s" };
				AoL::PartitionArray<AoL::String, 8> ap_new;
				ap_new = std::move(ap_move);
				PrintIteration(ap_new.begin(), ap_new.end());
			}
			{
				fmt::print("Iterator ctor\n");
				{
					AoL::PartitionArray<AoL::String, 8> ap_move{ "1s", "2s", "3s", "4s", "5s", "6s", "7s", "8s" };
					AoL::PartitionArray<AoL::String, 8> ap_new{ std::make_move_iterator(ap_move.begin()), std::make_move_iterator(ap_move.end()) };
					PrintIteration(ap_new.begin(), ap_new.end());
				}
				{
					AoL::PartitionArray<int, 8> ap_new{ ap_copy.begin(), ap_copy.end() };
					PrintIteration(ap_new.begin(), ap_new.end());
				}
			}
			{
				fmt::print("Fill ctor\n");
				AoL::PartitionArray<int, 8> ap(10);
				PrintIteration(ap.begin(), ap.end());
			}
		}
	}
}

#endif // AOL_TEST_FLAG(PARTITION)
#endif // BENCHMARK_CONFIG_ON