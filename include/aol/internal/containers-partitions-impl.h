#pragma once

/*************************************************
* [Container] Partition implementations
*************************************************/

namespace AoL::Internal
{

template<
	typename V
>
struct SubPartitionEx
{
public:
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

private:
	template<typename>
	friend struct PartitionContiguousBase;

	template<typename, typename>
	friend struct PartitionVectorEx;

	main_partition_vector* main_partition;
	size_type begin_offset;
	size_type end_offset;
	size_type current_size;

	SubPartitionEx(V& main_partition_vector_, size_type begin_off, size_type end_off, Optional<size_type> starting_size = std::nullopt) :
		main_partition(std::addressof(main_partition_vector_)),
		begin_offset(begin_off),
		end_offset(end_off),
		current_size(starting_size ? *starting_size : end_off - begin_off)
	{
	}

public:
	constexpr void clear() noexcept
	{
		current_size = 0;
	}

	constexpr void erase(size_type idx) noexcept
	{
		this->erase(idx, 1);
	}

	constexpr void erase(size_type starting_point, size_type count) noexcept
	{
		size_type end_point = starting_point + count;
		assert(count > 0 && "Cannot erase with a count of zero!");
		assert(starting_point < current_size && "Invalid starting point!");
		assert(end_point <= current_size && "Invalid count!");
		std::rotate(
			this->begin() + starting_point,
			this->begin() + end_point,
			this->end()
		);
		current_size -= count;
	}

	constexpr void pop_front() noexcept
	{
		assert(current_size > 0 && "Cannot pop an element in an empty partition!");
		std::rotate(
			this->begin(),
			this->begin() + 1,
			this->end()
		);
		current_size--;
	}

	constexpr void pop_back() noexcept
	{
		assert(current_size > 0 && "Cannot pop an element in an empty partition!");
		current_size--;
	}

	constexpr bool push_back(Traits::ConstRefOrCopyType<value_type> value) noexcept
	{
		// We no-op if the partition is already full
		// It'll be up to the user what to do if that happens
		if (this->full())
		{
			return false;
		}

		(*main_partition)[begin_offset + current_size++] = value;
		return true;
	}

	constexpr bool push_back(value_type&& value) noexcept
	{
		// We no-op if the partition is already full
		// It'll be up to the user what to do if that happens
		if (this->full())
		{
			return false;
		}

		(*main_partition)[begin_offset + current_size++] = std::move(value);
		return true;
	}

	template<typename... Args>
	constexpr value_type* emplace_back(Args&&... args) noexcept
	{
		// We no-op if the partition is already full
		// It'll be up to the user what to do if that happens
		if (this->full())
		{
			return nullptr;
		}

		(*main_partition)[begin_offset + current_size++] = value_type(std::forward<Args>(args)...);
		return std::addressof(this->back());
	}

	AOL_NO_DISCARD constexpr value_type& operator[] (size_type idx) noexcept
	{
		assert(!this->empty() && "Invalid operation! Accessing an empty partition!");
		assert(idx < current_size && "Invalid index! Accessing beyond allowable size!");
		return (*main_partition)[begin_offset + idx];
	}

	AOL_NO_DISCARD constexpr const value_type& operator[] (size_type idx) const noexcept
	{
		assert(!this->empty() && "Invalid operation! Accessing an empty partition!");
		assert(idx < current_size && "Invalid index! Accessing beyond allowable size!");
		return (*main_partition)[begin_offset + idx];
	}

	AOL_NO_DISCARD constexpr value_type& front() noexcept
	{
		return (*this)[0];
	}

	AOL_NO_DISCARD constexpr const value_type& front() const noexcept
	{
		return (*this)[0];
	}

	AOL_NO_DISCARD constexpr value_type& back() noexcept
	{
		return (*this)[current_size - 1];
	}

	AOL_NO_DISCARD constexpr const value_type& back() const noexcept
	{
		return (*this)[current_size - 1];
	}

	AOL_NO_DISCARD constexpr size_type size() const noexcept
	{
		return current_size;
	}

	AOL_NO_DISCARD constexpr bool empty() const noexcept
	{
		return current_size == 0;
	}

	AOL_NO_DISCARD constexpr bool full() const noexcept
	{
		return this->size() == this->max_size();
	}

	AOL_NO_DISCARD constexpr size_type max_size() const noexcept
	{
		return end_offset - begin_offset;
	}

	AOL_NO_DISCARD constexpr iterator begin() noexcept
	{
		return main_partition->begin() + begin_offset;
	}

	AOL_NO_DISCARD constexpr const_iterator begin() const noexcept
	{
		return main_partition->cbegin() + begin_offset;
	}

	AOL_NO_DISCARD constexpr const_iterator cbegin() const noexcept
	{
		return main_partition->cbegin() + begin_offset;
	}

	AOL_NO_DISCARD constexpr iterator end() noexcept
	{
		return this->begin() + current_size;
	}

	AOL_NO_DISCARD constexpr const_iterator end() const noexcept
	{
		return this->cbegin() + current_size;
	}

	AOL_NO_DISCARD constexpr const_iterator cend() const noexcept
	{
		return this->cbegin() + current_size;
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

private:
	enum class size_update_mode
	{
		unchanged,
		empty,
		update
	};

	void update_start_offset(size_type new_begin_off, size_update_mode update_type) noexcept
	{
		begin_offset = new_begin_off;
		switch (update_type)
		{
		case size_update_mode::empty:
		current_size = 0;
		break;

		case size_update_mode::update:
		current_size = end_offset - begin_offset;
		break;

		default:
		break;
		}
	}

	void update_end_offset(size_type new_end_off, size_update_mode update_type) noexcept
	{
		end_offset = new_end_off;
		switch (update_type)
		{
		case size_update_mode::empty:
		current_size = 0;
		break;

		case size_update_mode::update:
		current_size = end_offset - begin_offset;
		break;

		default:
		break;
		}
	}

	void shift_left_all_offset(size_type shift_count) noexcept
	{
		begin_offset -= shift_count;
		end_offset -= shift_count;
	}

	void shift_right_all_offset(size_type shift_count) noexcept
	{
		begin_offset += shift_count;
		end_offset += shift_count;
	}
};

template<
	typename D
>
struct PartitionContiguousBase
{
	using size_type = SizeT;
	using difference_type = PtrDiff;

	AOL_NO_DISCARD constexpr decltype(auto) get_partition(size_type partition_idx) const noexcept
	{
		assert(partition_idx < this->number_of_partitions() && "Invalid partition number! Partition number is greater than the number of current partition present.");
		return static_cast<const D*>(this)->sub_partitions[partition_idx];
	}

	AOL_NO_DISCARD constexpr decltype(auto) get_partition(size_type partition_idx) noexcept
	{
		assert(partition_idx < this->number_of_partitions() && "Invalid partition number! Partition number is greater than the number of current partition present.");
		return static_cast<D*>(this)->sub_partitions[partition_idx];
	}

	AOL_NO_DISCARD constexpr size_type size_of_partition(size_type partition_idx) const noexcept
	{
		assert(partition_idx < this->number_of_partitions() && "Invalid partition index! Reminder: Partition numbering is 0-based indexing!");
		return static_cast<const D*>(this)->sub_partitions[partition_idx].size();
	}

	AOL_NO_DISCARD constexpr size_type number_of_partitions() const noexcept
	{
		return static_cast<const D*>(this)->sub_partitions.size();
	}

	constexpr decltype(auto) create_partition(size_type partition_size, bool start_empty = true) noexcept
	{
		// The old back partition will become the newly created partition
		// The newly emplace_back-ed sub_partition will become the default partition
		// We update the new partition current_size if start_empty is false
		auto& sub_partitions = static_cast<D*>(this)->sub_partitions;
		auto& container_obj = static_cast<D*>(this)->container_obj;
		using sub_partition_type = std::decay_t<decltype(sub_partitions)>::value_type;

		auto& old_back_parti = sub_partitions.back();

		assert(old_back_parti.size() > 1 && "Invalid function call! The remaining partition only has a size of one!");
		assert(partition_size > 0 && "Invalid partition size! Cannot create a partition with zero size!");
		assert(partition_size < old_back_parti.max_size() && "Invalid partition size! Partition size cannot be more than or equal to the remaining partition");

		size_type split_point = old_back_parti.begin_offset + partition_size;
		size_type old_parti_size = old_back_parti.size();
		bool has_smaller_old_size = old_parti_size <= partition_size;
		if (start_empty)
		{
			old_back_parti.update_end_offset(split_point, sub_partition_type::size_update_mode::empty);
		}
		else
		{
			old_back_parti.update_end_offset(split_point, has_smaller_old_size ? sub_partition_type::size_update_mode::unchanged : sub_partition_type::size_update_mode::update);
		}
		sub_partitions.emplace_back(sub_partition_type(container_obj, split_point, container_obj.size(), has_smaller_old_size ? 0 : old_parti_size - partition_size));
		return sub_partitions[sub_partitions.size() - 2];
	}

	template<typename F, typename T = D::value_type> requires std::predicate<F&, T&>
	constexpr decltype(auto) create_partition(F&& partition_predicate, bool is_stable = true) noexcept(std::is_nothrow_invocable_v<F&, T&>)
	{
		auto& sub_partitions = static_cast<D*>(this)->sub_partitions;
		auto& back_partition = sub_partitions.back();
		auto default_partition_begin =
			is_stable ?
			std::stable_partition(back_partition.begin(), back_partition.end(), std::forward<F>(partition_predicate)) :
			std::partition(back_partition.begin(), back_partition.end(), std::forward<F>(partition_predicate));
		return create_partition(default_partition_begin - back_partition.begin(), false);
	}

	constexpr void erase_partition(size_type idx) noexcept
	{
		assert(idx < sub_partitions.size() && "Invalid index value! Cannot remove beyond the sub partition size!");
		assert(idx != sub_partitions.size() - 1 && "Invalid index value! Cannot remove the default partition!");
		size_type shift_count = sub_partitions[idx].size();
		for (size_type i = idx + 1; i < sub_partitions.size(); ++i)
		{
			sub_partitions[i].shift_left_all_offset(shift_count);
		}
		sub_partitions.erase(sub_partitions.begin() + idx);
	}

	AOL_NO_DISCARD constexpr decltype(auto) front() noexcept
	{
		return static_cast<D*>(this)->container_obj.front();
	}

	AOL_NO_DISCARD constexpr decltype(auto) front() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.front();
	}

	AOL_NO_DISCARD constexpr decltype(auto) back() noexcept
	{
		return static_cast<D*>(this)->container_obj.back();
	}

	AOL_NO_DISCARD constexpr decltype(auto) back() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.back();
	}

	AOL_NO_DISCARD constexpr decltype(auto) operator[] (size_type idx) noexcept
	{
		return (*static_cast<D*>(this)->container_obj)[idx];
	}

	AOL_NO_DISCARD constexpr decltype(auto) operator[] (size_type idx) const noexcept
	{
		return (*static_cast<const D*>(this)->container_obj)[idx];
	}

	AOL_NO_DISCARD constexpr size_type size() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.size();
	}

	AOL_NO_DISCARD constexpr bool empty() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.empty();
	}

	AOL_NO_DISCARD constexpr auto begin() noexcept
	{
		return static_cast<D*>(this)->container_obj.begin();
	}

	AOL_NO_DISCARD constexpr auto begin() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.cbegin();
	}

	AOL_NO_DISCARD constexpr auto cbegin() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.cbegin();
	}

	AOL_NO_DISCARD constexpr auto end() noexcept
	{
		return static_cast<D*>(this)->container_obj.end();
	}

	AOL_NO_DISCARD constexpr auto end() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.cend();
	}

	AOL_NO_DISCARD constexpr auto cend() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.cend();
	}

	AOL_NO_DISCARD constexpr auto rbegin() noexcept
	{
		return static_cast<D*>(this)->container_obj.rbegin();
	}

	AOL_NO_DISCARD constexpr auto rbegin() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.crbegin();
	}

	AOL_NO_DISCARD constexpr auto crbegin() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.crbegin();
	}

	AOL_NO_DISCARD constexpr auto rend() noexcept
	{
		return static_cast<D*>(this)->container_obj.rend();
	}

	AOL_NO_DISCARD constexpr auto rend() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.crend();
	}

	AOL_NO_DISCARD constexpr auto crend() const noexcept
	{
		return static_cast<const D*>(this)->container_obj.crend();
	}
};

/**
* Partitioned Vector
*/
template<
	typename T,
	typename A
>
struct PartitionVectorEx : PartitionContiguousBase<PartitionVectorEx<T,A>>
{
	using container_type = AoL::Vector<T, A>;

	using container_tag = ContainerTag;
	using value_type = container_type::value_type;
	using allocator_type = container_type::allocator_type;
	using iterator_tag = std::contiguous_iterator_tag;

	using size_type = SizeT;
	using difference_type = PtrDiff;

	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;

	using sub_partition_type = SubPartitionEx<container_type>;

	container_type container_obj;
	AoL::Vector<sub_partition_type> sub_partitions;

	PartitionVectorEx() noexcept :
		container_obj(),
		sub_partitions{ sub_partition_type{container_obj, 0, 0} }
	{
	}

	PartitionVectorEx(const PartitionVectorEx& other) noexcept = default;

	PartitionVectorEx(const PartitionVectorEx& other, const allocator_type& allocator) noexcept :
		container_obj(other.container_obj, allocator),
		sub_partitions(other.sub_partitions)
	{
	}

	PartitionVectorEx& operator = (const PartitionVectorEx& other) noexcept = default;

	PartitionVectorEx(PartitionVectorEx&& other) noexcept = default;

	PartitionVectorEx(PartitionVectorEx&& other, const allocator_type& allocator) noexcept :
		container_obj(std::move(other.container_obj), allocator),
		sub_partitions(std::move(other.sub_partitions))
	{
	}

	PartitionVectorEx& operator = (PartitionVectorEx&& other) noexcept = default;

	template<typename It>
	PartitionVectorEx(It start_it, It end_it, allocator_type allocator = allocator_type{}) noexcept :
		container_obj{ start_it, end_it, allocator },
		sub_partitions{ sub_partition_type{container_obj, 0, container_obj.size()} }
	{
	}

	PartitionVectorEx(size_type initial_size, allocator_type allocator = allocator_type{}) noexcept :
		container_obj(initial_size, allocator),
		sub_partitions{ sub_partition_type{container_obj, 0, initial_size} }
	{
	}

	PartitionVectorEx(size_type initial_size, AoL::Traits::ConstRefOrCopyType<value_type> value, allocator_type allocator = allocator_type{}) noexcept :
		container_obj(initial_size, value, allocator),
		sub_partitions{ sub_partition_type{container_obj, 0, initial_size} }
	{
	}

	PartitionVectorEx(allocator_type allocator) noexcept :
		container_obj(allocator),
		sub_partitions{ sub_partition_type{container_obj, 0, 0} }
	{
	}

	PartitionVectorEx(std::initializer_list<value_type> list, allocator_type allocator = allocator_type{}) noexcept :
		container_obj(list, allocator),
		sub_partitions{ sub_partition_type{container_obj, 0, container_obj.size()} }
	{
	}

	constexpr void push_back(value_type&& value) noexcept
	{
		sub_partition_type& back_parti = sub_partitions.back();
		if (back_parti.full())
		{
			container_obj.push_back(std::move(value));
			back_parti.update_end_offset(container_obj.size(), sub_partition_type::size_update_mode::update);
		}
		else
		{
			back_parti.push_back(std::move(value));
		}
	}

	constexpr void push_back(Traits::ConstRefOrCopyType<value_type> value) noexcept
	{
		sub_partition_type& back_parti = sub_partitions.back();
		if (back_parti.full())
		{
			container_obj.push_back(value);
			back_parti.update_end_offset(container_obj.size(), sub_partition_type::size_update_mode::update);
		}
		else
		{
			back_parti.push_back(value);
		}
	}

	template<typename... Args>
	constexpr value_type& emplace_back(Args&&... args) noexcept
	{
		sub_partition_type& back_parti = sub_partitions.back();
		if (back_parti.full())
		{
			value_type& ret = container_obj.emplace_back(std::forward<Args>(args)...);
			sub_partitions.back().update_end_offset(container_obj.size(), sub_partition_type::size_update_mode::update);
			return ret;
		}
		else
		{
			return *back_parti.emplace_back(std::forward<Args>(args)...);
		}
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
			sub_partitions.back().update_end_offset(new_size, sub_partition_type::size_update_mode::unchanged);
		}
		else
		{
			size_t new_sp_size = sub_partitions.size();

			for (sub_partition_type& sp : std::views::reverse(sub_partitions))
			{
				if (sp.begin_offset >= new_size)
				{
					--new_sp_size;
					continue;
				}
				if (sp.end_offset >= new_size)
				{
					sp.update_end_offset(
						new_size,
						sp.current_size < (new_size - sp.begin_offset) ? sub_partition_type::size_update_mode::unchanged : sub_partition_type::size_update_mode::update
					);
					break;
				}
			}

			sub_partitions.erase(sub_partitions.begin() + new_sp_size, sub_partitions.end());
		}
	}

	constexpr void clear_partitions() noexcept
	{
		for (sub_partition_type& partition : sub_partitions)
		{
			partition.clear();
		}
	}

	constexpr void clear_all() noexcept
	{
		container_obj.clear();
		sub_partitions.clear();
	}
};

} // AoL::Internal namespace

// containers-partitions-impl.h EOF