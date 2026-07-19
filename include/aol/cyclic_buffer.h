/***************************************************************************************
* AoLibrary Cyclic Buffer
****************************************************************************************
* - The library's cyclic buffer
***************************************************************************************/
#ifndef AOL_HEADER_CYCLIC_BUFFER_H
#define AOL_HEADER_CYCLIC_BUFFER_H

#include "configs.h"
#include "types.h"
#include "traits.h"
#include "allocators.h"

#include "internal/containers-cyclic-buffer-impl.h"


namespace AoL
{

/**
* @details Fixed size cyclic buffer
*
* - Cyclic buffer is a FIFO container that overwrite the oldest element when
*   hitting the max number of elements it can contain
*
* - This uses an array so this will immediately construct empty elements
*
* - Only accepts power of two value for size (e.g. 2, 4, 8, 16, 32, etc)
*
* @tparam T element type
* @tparam S max item count/size
*/
template<
	typename T,
	SizeT S
>
using CyclicBufferF = Internal::CyclicBufferFixed<T, S>;

/**
* @details Dynamic size cyclic buffer
*
* - Cyclic buffer is a FIFO container that overwrite the oldest element when
*   hitting the max number of elements it can contain
*
* - This uses a vector and will only allocate when needed until it reaches the max
*   number of elements it can contain
*
* - Only accepts power of two value for size (e.g. 2, 4, 8, 16, 32, etc)
*
* @tparam T element type
* @tparam A allocator type
*/
template<
	typename T,
	typename A = DefaultAllocator<T>
>
using CyclicBufferD = Internal::CyclicBufferDynamic<T, A>;

}


#endif