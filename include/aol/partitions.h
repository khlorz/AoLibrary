/***************************************************************************************
* AoLibrary Partition
****************************************************************************************
* - The library's partition
***************************************************************************************/
#ifndef AOL_HEADER_PARTITION_H
#define AOL_HEADER_PARTITION_H

#include "configs.h"
#include "types.h"
#include "traits.h"
#include "allocators.h"

#include "internal/containers/partitions.h"


namespace AoL
{

/*
* @details SubPartition of a container.
*
* Constructible only from a partition. Can be copied or moved but not constructed.
*/
template<typename P>
using SubPartition = Internal::SubPartitionEx<P>;

/*
* @details Partition using AoL::Vector
*
* As a vector, the storage is contiguous.
*
* @tparam T value type
* @tparam A allocator type (default: Internal::DefaultAllocator<T>)
*/
template<
	typename T,
	typename A = DefaultAllocator<T>
>
using PartitionVector = Internal::PartitionVectorEx<T, A>;

/*
* @details Partition using AoL::AoL
*
* As an array, the storage is contiguous.
*
* @tparam T value type
* @tparam S array size
*/
template<
	typename T,
	AoL::SizeT S
>
using PartitionArray = Internal::PartitionArrayEx<T, S>;

}


#endif // AOL_HEADER_PARTITION_H