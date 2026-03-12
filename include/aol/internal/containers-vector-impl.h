#pragma once

/*************************************************
* Vector implementations
*************************************************/

namespace AoL::Internal
{

template<
	typename T,
	typename A
>
struct VectorCircular : ContainerBase<AoL::ContainerTypeTag::Vector, T, AoL::Vector<T,A>>
{

};

}


// containers-vector-impl.h EOF