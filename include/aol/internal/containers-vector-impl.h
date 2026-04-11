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

	using value_type = vector_type::value_type;
	using allocator_type = vector_type::allocator_type;

	using size_type = SizeT;
	using difference_type = PtrDiff;

	using iterator = vector_type::iterator;
	using const_iterator = vector_type::const_iterator;
	using reverse_iterator = vector_type::reverse_iterator;
	using const_reverse_iterator = vector_type::const_reverse_iterator;

	vector_type container_obj;

	VectorPartitionEx() noexcept :
		container_obj{ }
	{}

	VectorPartitionEx(const VectorPartitionEx& other) noexcept = default;

	VectorPartitionEx(const VectorPartitionEx& other, const allocator_type& allocator) noexcept :
		container_obj( other, allocator )
	{}

	VectorPartitionEx& operator = (const VectorPartitionEx& other) noexcept = default;

	VectorPartitionEx(VectorPartitionEx&& other) noexcept = default;

	VectorPartitionEx(VectorPartitionEx&& other, const allocator_type& allocator) noexcept :
		container_obj( other, allocator )
	{}

	VectorPartitionEx& operator = (VectorPartitionEx&& other) noexcept = default;

	template<typename It>
	VectorPartitionEx(It start_it, It end_it, allocator_type allocator = allocator_type{}) noexcept :
		container_obj{ start_it, end_it, allocator }
	{}

	VectorPartitionEx(size_type initial_size, allocator_type allocator = allocator_type{}) noexcept :
		container_obj( initial_size, allocator )
	{}

	VectorPartitionEx(size_type initial_size, AoL::Traits::ConstRefOrCopyType<value_type> value, allocator_type allocator = allocator_type{}) noexcept :
		container_obj( initial_size, value, allocator )
	{}

	VectorPartitionEx(allocator_type allocator) noexcept :
		container_obj( allocator )
	{}

	VectorPartitionEx(std::initializer_list<value_type> list, allocator_type allocator = allocator_type{}) noexcept :
		container_obj( list, allocator )
	{}
};


} // AoL::Internal namespace


// containers-vector-impl.h EOF