#include "all_tests.h"

#include "aol/aol.h"

void AoL::PartitionTests() noexcept
{
	{
		fmt::print("\nPartition tests\n");
		fmt::print("Ctors\n");
		AoL::VectorPartition<int> vp1{ 1,2,3,4,5,6,7,8,9,10 };
		AoL::VectorPartition<int> vp2{ 10 };
	}
	
	{
		fmt::print("Subpartition creation\n");
		AoL::VectorPartition<int> vp{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
		vp.create_partition(10);
		vp.create_partition(
			[](int v) noexcept
			{
				return v % 2 == 0;
			}
		);
	}

	{
		fmt::print("Subpartition creation with non-'full' default_partition\n");
		AoL::VectorPartition<int> vp{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
		auto& back_partition = vp.get_partition(0);

		back_partition.pop_back();
		back_partition.pop_back();
		back_partition.pop_back();
		back_partition.pop_back();
		back_partition.pop_back();

		vp.create_partition(20, false);
	}

	{
		fmt::print("Subpartition resizing down\n");
		AoL::VectorPartition<int> vp{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
		vp.create_partition(10);
		auto& partition = vp.create_partition(8, false);

		partition.erase(2, 4);
		vp.resize(13);
	}

	{
		fmt::print("push_back on empty size partition\n");
		AoL::VectorPartition<int> vp{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
		auto& partition = vp.create_partition(10);

		for (int i = 0; i < (int)partition.max_size(); ++i)
		{
			partition.push_back(i);
		}
	}
}