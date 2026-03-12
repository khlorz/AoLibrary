#pragma once

/*************************************************
* [Container] Vector implementations
*************************************************/

namespace AoL::Internal
{

/**
* @details Circular vector for circular purposes like overwriting the oldest and such
* 
* @tparam T element type
* @tparam A allocator type
*/
template<
	typename T,
	typename A
>
struct VectorCircularEx : ContainerBase<ContainerTypeTag::Vector, T, Vector<T,A>>
{
private:
	using Base = ContainerBase<ContainerTypeTag::Vector, T, Vector<T, A>>;

	using Base::container_obj;

public:
	using container_type = Vector<T,A>;

	using value_type = container_type::value_type;
	using allocator_type = container_type::allocator_type;

	using pointer = container_type::pointer;
	using const_pointer = container_type::const_pointer;
	using reference = container_type::reference;
	using const_reference = container_type::const_reference;
};

} // AoL::Internal namespace


// containers-vector-impl.h EOF