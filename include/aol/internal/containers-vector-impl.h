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

	using iterator = typename vector_type::iterator;
	using const_iterator = typename vector_type::const_iterator;
	using reverse_iterator = typename vector_type::reverse_iterator;
	using const_reverse_iterator = typename vector_type::const_reverse_iterator;

	using sub_partition_type = AoL::Subrange<iterator>;

	vector_type container_obj;
	AoL::Vector<sub_partition_type> sub_partitions;

	VectorPartitionEx() noexcept :
		container_obj( ),
		sub_partitions{ Subrange{container_obj.begin(), container_obj.end()} }
	{}

	VectorPartitionEx(const VectorPartitionEx& other) noexcept = default;

	VectorPartitionEx(const VectorPartitionEx& other, const allocator_type& allocator) noexcept :
		container_obj( other.container_obj, allocator ),
		sub_partitions{ Subrange{container_obj.begin(), container_obj.end()} }
	{}

	VectorPartitionEx& operator = (const VectorPartitionEx& other) noexcept = default;

	VectorPartitionEx(VectorPartitionEx&& other) noexcept = default;

	VectorPartitionEx(VectorPartitionEx&& other, const allocator_type& allocator) noexcept :
		container_obj( std::move(other.container_obj), allocator ),
		sub_partitions{ Subrange{container_obj.begin(), container_obj.end()} }
	{}

	VectorPartitionEx& operator = (VectorPartitionEx&& other) noexcept = default;

	template<typename It>
	VectorPartitionEx(It start_it, It end_it, allocator_type allocator = allocator_type{}) noexcept :
		container_obj{ start_it, end_it, allocator },
		sub_partitions{ Subrange{container_obj.begin(), container_obj.end()} }
	{}

	VectorPartitionEx(size_type initial_size, allocator_type allocator = allocator_type{}) noexcept :
		container_obj( initial_size, allocator ),
		sub_partitions{ Subrange{container_obj.begin(), container_obj.end()} }
	{}

	VectorPartitionEx(size_type initial_size, AoL::Traits::ConstRefOrCopyType<value_type> value, allocator_type allocator = allocator_type{}) noexcept :
		container_obj( initial_size, value, allocator ),
		sub_partitions{ Subrange{container_obj.begin(), container_obj.end()} }
	{}

	VectorPartitionEx(allocator_type allocator) noexcept :
		container_obj( allocator ),
		sub_partitions{ Subrange{container_obj.begin(), container_obj.end()} }
	{}

	VectorPartitionEx(std::initializer_list<value_type> list, allocator_type allocator = allocator_type{}) noexcept :
		container_obj( list, allocator ),
		sub_partitions{ Subrange{container_obj.begin(), container_obj.end()} }
	{}

	AOL_NO_DISCARD constexpr const sub_partition_type& get_partition(size_type partition_idx) const noexcept
	{
		assert(partition_idx < this->number_of_partitions() && "Invalid partition number! Partition number is greater than the number of current partition present.");
		return sub_partitions[partition_idx];
	}

	AOL_NO_DISCARD constexpr sub_partition_type& get_partition(size_type partition_idx) noexcept
	{
		assert(partition_idx < this->number_of_partitions() && "Invalid partition number! Partition number is greater than the number of current partition present.");
		return sub_partitions[partition_idx];
	}

	AOL_NO_DISCARD constexpr size_type size_of_partition(size_type partition_idx) const noexcept
	{
		assert(partition_idx < this->number_of_partitions() && "Invalid partition index! Reminder: Partition numbering is 0-based indexing!");
		return sub_partitions[partition_idx].size();
	}

	AOL_NO_DISCARD constexpr size_type number_of_partitions() const noexcept
	{
		return sub_partitions.size();
	}

	constexpr void create_partition(size_type partition_size) noexcept
	{
		sub_partition_type& new_end_part = sub_partitions.emplace_back(sub_partitions.back().begin(), sub_partitions.back().begin() + partition_size);
		sub_partition_type& old_end_part = sub_partitions[sub_partitions.size() - 2];
		std::swap(old_end_part, new_end_part);
		new_end_part.start = old_end_part.finish;
	}

	constexpr void push_back(T&& value) noexcept
	{
		container_obj.push_back(std::forward<T>(value));
	}

	constexpr void push_back(Traits::ConstRefOrCopyType<T> value) noexcept
	{
		container_obj.push_back(value);
	}

	template<typename... Ts>
	constexpr decltype(auto) emplace_back(Ts&&... ts) noexcept
	{
		container_obj.emplace_back(std::forward<Ts>(ts)...);
	}

	AOL_NO_DISCARD constexpr size_type size() const noexcept
	{
		return container_obj.size();
	}

	AOL_NO_DISCARD constexpr bool empty() const noexcept
	{
		return container_obj.empty();
	}

	AOL_NO_DISCARD constexpr iterator begin() noexcept
	{
		return container_obj.begin();
	}

	AOL_NO_DISCARD constexpr const_iterator begin() const noexcept
	{
		return container_obj.cbegin();
	}

	AOL_NO_DISCARD constexpr const_iterator cbegin() const noexcept
	{
		return container_obj.cbegin();
	}

	AOL_NO_DISCARD constexpr iterator end() noexcept
	{
		return container_obj.end();
	}

	AOL_NO_DISCARD constexpr const_iterator end() const noexcept
	{
		return container_obj.cend();
	}

	AOL_NO_DISCARD constexpr const_iterator cend() const noexcept
	{
		return container_obj.cend();
	}

	AOL_NO_DISCARD constexpr reverse_iterator rbegin() noexcept
	{
		return container_obj.rbegin();
	}

	AOL_NO_DISCARD constexpr const_reverse_iterator rbegin() const noexcept
	{
		return container_obj.crbegin();
	}

	AOL_NO_DISCARD constexpr const_reverse_iterator crbegin() const noexcept
	{
		return container_obj.crbegin();
	}

	AOL_NO_DISCARD constexpr reverse_iterator rend() noexcept
	{
		return container_obj.rend();
	}

	AOL_NO_DISCARD constexpr const_reverse_iterator rend() const noexcept
	{
		return container_obj.crend();
	}

	AOL_NO_DISCARD constexpr const_reverse_iterator crend() const noexcept
	{
		return container_obj.crend();
	}
};

} // AoL::Internal namespace


// containers-vector-impl.h EOF