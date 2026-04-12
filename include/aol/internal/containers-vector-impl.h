#pragma once

/*************************************************
* [Container] Vector implementations
*************************************************/

namespace AoL::Internal
{

/**
* Partitioned Vector
*/
template<
	typename T,
	typename A
>
struct VectorPartitionEx
{
	using vector_type = AoL::Vector<T, A>;

	using container_tag = AoL::Internal::ContainerTag;
	using value_type = vector_type::value_type;
	using allocator_type = vector_type::allocator_type;

	using size_type = SizeT;
	using difference_type = PtrDiff;

	using iterator = vector_type::iterator;
	using const_iterator = vector_type::const_iterator;
	using reverse_iterator = vector_type::reverse_iterator;
	using const_reverse_iterator = vector_type::const_reverse_iterator;

	vector_type container_obj;
	AoL::Vector<size_type> partition_indices;

	VectorPartitionEx() noexcept :
		container_obj( ),
		partition_indices{ size_type{0} }
	{}

	VectorPartitionEx(const VectorPartitionEx& other) noexcept = default;

	VectorPartitionEx(const VectorPartitionEx& other, const allocator_type& allocator) noexcept :
		container_obj( other, allocator ),
		partition_indices{ size_type{0} }
	{}

	VectorPartitionEx& operator = (const VectorPartitionEx& other) noexcept = default;

	VectorPartitionEx(VectorPartitionEx&& other) noexcept = default;

	VectorPartitionEx(VectorPartitionEx&& other, const allocator_type& allocator) noexcept :
		container_obj( std::move(other.container_obj), allocator ),
		partition_indices{ size_type{0} }
	{}

	VectorPartitionEx& operator = (VectorPartitionEx&& other) noexcept = default;

	template<typename It>
	VectorPartitionEx(It start_it, It end_it, allocator_type allocator = allocator_type{}) noexcept :
		container_obj{ start_it, end_it, allocator },
		partition_indices{ size_type{0} }
	{}

	VectorPartitionEx(size_type initial_size, allocator_type allocator = allocator_type{}) noexcept :
		container_obj( initial_size, allocator ),
		partition_indices{ size_type{0} }
	{}

	VectorPartitionEx(size_type initial_size, AoL::Traits::ConstRefOrCopyType<value_type> value, allocator_type allocator = allocator_type{}) noexcept :
		container_obj( initial_size, value, allocator ),
		partition_indices{ size_type{0} }
	{}

	VectorPartitionEx(allocator_type allocator) noexcept :
		container_obj( allocator ),
		partition_indices{ size_type{0} }
	{}

	VectorPartitionEx(std::initializer_list<value_type> list, allocator_type allocator = allocator_type{}) noexcept :
		container_obj( list, allocator ),
		partition_indices{ size_type{0} }
	{}

	AOL_NO_DISCARD constexpr Subrange<const_iterator> get_partition(size_type partition_idx) const noexcept
	{
		assert(partition_idx < partition_indices.size() && "Invalid partition number! Partition number is greater than the number of current partition present.");
		return Subrange{
			container_obj.begin() + partition_indices[partition_idx],
			partition_idx == partition_indices.size() - 1 ? container_obj.end() : (container_obj.begin() + partition_indices[partition_idx + 1])
		};
	}

	AOL_NO_DISCARD constexpr Subrange<iterator> get_partition(size_type partition_idx) noexcept
	{
		assert(partition_idx < partition_indices.size() && "Invalid partition number! Partition number is greater than the number of current partition present.");
		return Subrange{
			container_obj.begin() + partition_indices[partition_idx],
			partition_idx == partition_indices.size() - 1 ? container_obj.end() : (container_obj.begin() + partition_indices[partition_idx + 1])
		};
	}

	constexpr void create_partition(size_type partition_size) noexcept
	{
		assert(partition_size < container_obj.size() - partition_indices.back() && "Invalid partition size! Must be at least 1 and not equal to the remaining partition size");
		partition_indices.push_back(partition_indices.back() + partition_size);
	}

	AOL_NO_DISCARD constexpr size_type size_total() const noexcept
	{
		return container_obj.size();
	}

	AOL_NO_DISCARD constexpr size_type size_of_partition(size_type partition_idx) const noexcept
	{
		return partition_idx == partition_indices.size() ? container_obj.size() : partition_indices[partition_idx + 1];
	}

	AOL_NO_DISCARD constexpr size_type number_of_partitions() const noexcept
	{
		return partition_indices.size();
	}
};


} // AoL::Internal namespace


// containers-vector-impl.h EOF