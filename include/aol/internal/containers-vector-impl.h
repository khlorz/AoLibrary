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

};

} // AoL::Internal namespace


// containers-vector-impl.h EOF