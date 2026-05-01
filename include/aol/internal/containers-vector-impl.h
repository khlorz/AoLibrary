#pragma once

/*************************************************
* [Container] Vector implementations
*************************************************/

namespace AoL::Internal
{

template<
	typename V
>
struct SubPartition
{
	using main_partition_vector = V;

	using container_tag = ContainerTag;
	using value_type = typename main_partition_vector::value_type;
	using allocator_type = typename main_partition_vector::allocator_type;

	using size_type = SizeT;
	using difference_type = PtrDiff;

	using iterator = typename main_partition_vector::iterator;
	using const_iterator = typename main_partition_vector::const_iterator;
	using reverse_iterator = typename main_partition_vector::reverse_iterator;
	using const_reverse_iterator = typename main_partition_vector::const_reverse_iterator;

	main_partition_vector* main_partition;
	size_type start;
	size_type finish;

	SubPartition(V& main_partition_vector_, size_type start_, size_type finish_) :
		main_partition(std::addressof(main_partition_vector_)),
		start(start_),
		finish(finish_)
	{
	}

	AOL_NO_DISCARD constexpr size_type size() const noexcept
	{
		return finish - start;
	}

	AOL_NO_DISCARD constexpr iterator begin() noexcept
	{
		return main_partition->begin() + start;
	}

	AOL_NO_DISCARD constexpr const_iterator begin() const noexcept
	{
		return main_partition->cbegin() + start;
	}

	AOL_NO_DISCARD constexpr const_iterator cbegin() const noexcept
	{
		return main_partition->cbegin() + start;
	}

	AOL_NO_DISCARD constexpr iterator end() noexcept
	{
		return main_partition->begin() + finish;
	}

	AOL_NO_DISCARD constexpr const_iterator end() const noexcept
	{
		return main_partition->cbegin() + finish;
	}

	AOL_NO_DISCARD constexpr const_iterator cend() const noexcept
	{
		return main_partition->cbegin() + finish;
	}

	AOL_NO_DISCARD constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(this->end());
	}

	AOL_NO_DISCARD constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(this->cend());
	}

	AOL_NO_DISCARD constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(this->cend());
	}

	AOL_NO_DISCARD constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator(this->begin());
	}

	AOL_NO_DISCARD constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(this->cbegin());
	}

	AOL_NO_DISCARD constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(this->cbegin());
	}
};

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

	using container_tag = ContainerTag;
	using value_type = vector_type::value_type;
	using allocator_type = vector_type::allocator_type;

	using size_type = SizeT;
	using difference_type = PtrDiff;

	using iterator = typename vector_type::iterator;
	using const_iterator = typename vector_type::const_iterator;
	using reverse_iterator = typename vector_type::reverse_iterator;
	using const_reverse_iterator = typename vector_type::const_reverse_iterator;

	using sub_partition_type = SubPartition<vector_type>;

	vector_type container_obj;
	AoL::Vector<sub_partition_type> sub_partitions;

	VectorPartitionEx() noexcept :
		container_obj(),
		sub_partitions{ SubPartition{container_obj, 0, 0} }
	{
	}

	VectorPartitionEx(const VectorPartitionEx& other) noexcept = default;

	VectorPartitionEx(const VectorPartitionEx& other, const allocator_type& allocator) noexcept :
		container_obj(other.container_obj, allocator),
		sub_partitions(other.sub_partitions)
	{
	}

	VectorPartitionEx& operator = (const VectorPartitionEx& other) noexcept = default;

	VectorPartitionEx(VectorPartitionEx&& other) noexcept = default;

	VectorPartitionEx(VectorPartitionEx&& other, const allocator_type& allocator) noexcept :
		container_obj(std::move(other.container_obj), allocator),
		sub_partitions(std::move(other.sub_partitions))
	{
	}

	VectorPartitionEx& operator = (VectorPartitionEx&& other) noexcept = default;

	template<typename It>
	VectorPartitionEx(It start_it, It end_it, allocator_type allocator = allocator_type{}) noexcept :
		container_obj{ start_it, end_it, allocator },
		sub_partitions{ SubPartition{container_obj, 0, container_obj.size()} }
	{
	}

	VectorPartitionEx(size_type initial_size, allocator_type allocator = allocator_type{}) noexcept :
		container_obj(initial_size, allocator),
		sub_partitions{ SubPartition{container_obj, 0, initial_size} }
	{
	}

	VectorPartitionEx(size_type initial_size, AoL::Traits::ConstRefOrCopyType<value_type> value, allocator_type allocator = allocator_type{}) noexcept :
		container_obj(initial_size, value, allocator),
		sub_partitions{ SubPartition{container_obj, 0, initial_size} }
	{
	}

	VectorPartitionEx(allocator_type allocator) noexcept :
		container_obj(allocator),
		sub_partitions{ SubPartition{container_obj, 0, 0} }
	{
	}

	VectorPartitionEx(std::initializer_list<value_type> list, allocator_type allocator = allocator_type{}) noexcept :
		container_obj(list, allocator),
		sub_partitions{ SubPartition{container_obj, 0, container_obj.size()} }
	{
	}

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

	constexpr sub_partition_type& create_partition(size_type partition_size) noexcept
	{
		assert(sub_partitions.back().size() > 1 && "Invalid function call! The remaining partition only has a size of one!");
		assert(partition_size > 0 && "Invalid partition size! Cannot create a partition with zero size!");
		assert(partition_size < sub_partitions.back().size() && "Invalid partition size! Partition size cannot be more than or equal to the remaining partition");
		size_type split_point = sub_partitions.back().start + partition_size;
		sub_partitions.back().finish = split_point;
		sub_partitions.emplace_back(container_obj, split_point, container_obj.size());
		return sub_partitions[sub_partitions.size() - 2];
	}

	template<typename F> requires std::predicate<F&, value_type&>
	constexpr sub_partition_type& create_partition(F&& partition_predicate, bool is_stable = true) noexcept(noexcept(std::declval<F&>()(std::declval<value_type&>())))
	{
		sub_partition_type& back_partition = sub_partitions.back();
		auto default_partition_begin = 
			is_stable ? 
			std::stable_partition(back_partition.begin(), back_partition.end(), std::forward<F>(partition_predicate)) :
			std::partition(back_partition.begin(), back_partition.end(), std::forward<F>(partition_predicate));
		return create_partition(default_partition_begin - back_partition.begin());
	}

	constexpr void push_back(T&& value) noexcept
	{
		container_obj.push_back(std::forward<T>(value));
		sub_partitions.back().finish = container_obj.size();
	}

	constexpr void push_back(Traits::ConstRefOrCopyType<T> value) noexcept
	{
		container_obj.push_back(value);
		sub_partitions.back().finish = container_obj.size();
	}

	template<typename... Args>
	constexpr T& emplace_back(Args&&... args) noexcept
	{
		T& ret = container_obj.emplace_back(std::forward<Args>(args)...);
		sub_partitions.back().finish = container_obj.size();
		return ret;
	}

	constexpr void reserve(size_type new_capacity) noexcept
	{
		container_obj.reserve(new_capacity);
	}

	constexpr void resize(size_type new_size) noexcept
	{
		size_type old_size = container_obj.size();
		container_obj.resize(new_size);
		if (old_size <= new_size)
		{
			sub_partitions.back().finish = new_size;
		}
		else
		{
			size_t new_sp_size = sub_partitions.size();

			for (sub_partition_type& sp : std::views::reverse(sub_partitions))
			{
				if (sp.start >= new_size)
				{
					--new_sp_size;
					continue;
				}
				if (sp.finish >= new_size)
				{
					sp.finish = new_size;
					break;
				}
			}

			sub_partitions.erase(sub_partitions.begin() + new_sp_size, sub_partitions.end());
		}
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