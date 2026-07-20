/*****************************************************
 * Container's serialization implementations
 *****************************************************/
#ifndef AOL_HEADER_INTERNAL_SERIALIZATION_CONTAINERS_H
#define AOL_HEADER_INTERNAL_SERIALIZATION_CONTAINERS_H


#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/traits.h"
#include "aol/types.h"


namespace cereal
{

#if defined(AOL_HEADER_VECTOR_H)
#include "cereal/types/vector.hpp"
#endif // AOL_HEADER_VECTOR_H


#if defined(AOL_HEADER_ARRAY_H)
#include "cereal/types/array.hpp"

/****************************************
* ArrayNamed# containers
****************************************/

#define AOL_SERIALIZATION_HELPER_NAMED_ARRAY(size) \
template<															\
	typename Archive,												\
	typename T														\
>																	\
void save(Archive& archive, const AoL::ArrayNamed##size##<T>& arr)	\
{																	\
	archive(														\
		cereal::binary_data(arr.data_arr, sizeof(T) * size)			\
	);																\
}																	\
																	\
template<															\
	typename Archive,												\
	typename T														\
>																	\
void load(Archive& archive, AoL::ArrayNamed##size##<T>& arr)		\
{																	\
	archive(														\
		cereal::binary_data(arr.data_arr, sizeof(T) * size)			\
	);																\
}																	

AOL_SERIALIZATION_HELPER_NAMED_ARRAY(2)
AOL_SERIALIZATION_HELPER_NAMED_ARRAY(3)
AOL_SERIALIZATION_HELPER_NAMED_ARRAY(4)
#endif // AOL_HEADER_ARRAY_H


#if defined(AOL_HEADER_KEY_ORDERED_MAP_H)
#include "cereal/types/vector.hpp"

/****************************************
* KeyValuePairEx
****************************************/

template<
	typename Archive,
	typename K,
	typename V
>
void save(Archive& archive, const AoL::Internal::KeyValuePairEx<K, V>& kvp)
{
	archive(
		kvp.first,
		kvp.second
	);
}

template<
	typename Archive,
	typename K,
	typename V
>
void load(Archive& archive, AoL::Internal::KeyValuePairEx<K, V>& kvp)
{
	archive(
		kvp.first,
		kvp.second
	);
}

/****************************************
* KeyOrderMapEx
****************************************/

template<
	typename Archive,
	typename K,
	typename V,
	typename P,
	typename C,
	typename A
>
void save(Archive& archive, const AoL::Internal::KeyOrderMapEx<K, V, P, C, A>& c)
{
	archive(c.container_obj);
}

template<
	typename Archive,
	typename K,
	typename V,
	typename P,
	typename C,
	typename A
>
void load(Archive& archive, AoL::Internal::KeyOrderMapEx<K, V, P, C, A>& c)
{
	archive(c.container_obj);
}
#endif // AOL_HEADER_KEY_ORDERED_MAP_H


#if defined(AOL_HEADER_KEY_ORDERED_SET_H)
#include "cereal/types/set.hpp"
#endif // AOL_HEADER_KEY_ORDERED_SET_H


#if defined(AOL_HEADER_INSERT_ORDERED_MAP_H)
#include "cereal/types/map.hpp"
#endif // AOL_HEADER_INSERT_ORDERED_MAP_H


#if defined(AOL_HEADER_INSERT_ORDERED_SET_H)
#include "cereal/types/set.hpp"
#endif // AOL_HEADER_INSERT_ORDERED_SET_H


#if defined(AOL_HEADER_HASH_MAP_H)
#include "cereal/types/unordered_map.hpp"
#endif // AOL_HEADER_HASH_MAP_H


#if defined(AOL_HEADER_HASH_SET_H)
#include "cereal/types/unordered_set.hpp"
#endif // AOL_HEADER_HASH_SET_H


#if defined(AOL_HEADER_CYCLIC_BUFFER_H)
#include "cereal/types/vector.hpp"
#include "cereal/types/array.hpp"

/****************************************
* CyclicBuffer containers
****************************************/

template<
	typename Archive,
	typename D,
	typename T,
	typename A,
	AoL::SizeT S
>
void save(Archive& archive, const AoL::Internal::CyclicBufferBase<D, T, A, S>& cbb)
{
	archive(
		cbb.container_obj,
		cbb.head,
		cbb.item_count,
		cbb.mask
	);
}

template<
	typename Archive,
	typename D,
	typename T,
	typename A,
	AoL::SizeT S
>
void load(Archive& archive, AoL::Internal::CyclicBufferBase<D, T, A, S>& cbb)
{
	archive(
		cbb.container_obj,
		cbb.head,
		cbb.item_count,
		cbb.mask
	);
}

template<
	typename Archive,
	typename T,
	typename A
>
void save(Archive& archive, const AoL::CyclicBufferD<T, A>& cbd)
{
	archive(
		cereal::base_class<AoL::Internal::CyclicBufferBase<AoL::CyclicBufferD<T, A>, T, A, 0>>(&cbd)
	);
}

template<
	typename Archive,
	typename T,
	typename A
>
void load(Archive& archive, AoL::CyclicBufferD<T, A>& cbd)
{
	archive(
		cereal::base_class<AoL::Internal::CyclicBufferBase<AoL::CyclicBufferD<T, A>, T, A, 0>>(&cbd)
	);
}

template<
	typename Archive,
	typename T,
	AoL::SizeT S
>
void save(Archive& archive, const AoL::CyclicBufferF<T, S>& cbb)
{
	archive(
		cereal::base_class<AoL::Internal::CyclicBufferBase<AoL::CyclicBufferF<T, S>, T, void, S>>(&cbb)
	);
}

template<
	typename Archive,
	typename T,
	AoL::SizeT S
>
void load(Archive& archive, AoL::CyclicBufferF<T, S>& cbb)
{
	archive(
		cereal::base_class<AoL::Internal::CyclicBufferBase<AoL::CyclicBufferF<T, S>, T, void, S>>(&cbb)
	);
}
#endif // AOL_HEADER_CYCLIC_BUFFER_H


#if defined(AOL_HEADER_PARTITIONS_H)
#include "cereal/types/vector.hpp"
#include "cereal/types/array.hpp"

/****************************************
* Partitions containers
****************************************/

template<
	typename Archive,
	typename T,
	typename A
>
void save(Archive& archive, const AoL::PartitionVector<T, A>& pv)
{
	archive(
		pv.container_obj
	);
	archive(pv.number_of_partitions());
	for (AoL::SizeT i = 0; i < pv.number_of_partitions(); ++i)
	{
		auto& sub_partition = pv.get_partition(i);
		archive(
			sub_partition.max_size(),
			sub_partition.size()
		);
	}
}

template<
	typename Archive,
	typename T,
	typename A
>
void load(Archive& archive, AoL::PartitionVector<T, A>& pv)
{
	typename AoL::PartitionVector<T,A>::container_type new_vector;
	AoL::SizeT sub_partitions_size;
	AoL::SizeT max_size;
	AoL::SizeT current_size;

	archive(
		new_vector,
		sub_partitions_size
	);
	
	pv.assign(std::move(new_vector));
	for (AoL::SizeT i = 0; i < sub_partitions_size - 1; ++i)
	{
		archive(
			max_size,
			current_size
		);
		auto& sub_partition = pv.create_partition(max_size, false);
		auto empty_size = max_size - current_size;
		if (empty_size > 0)
		{
			sub_partition.erase(current_size, empty_size);
		}
	}
}

template<
	typename Archive,
	typename T,
	AoL::SizeT S
>
void save(Archive& archive, const AoL::PartitionArray<T, S>& pa)
{
	archive(
		pa.container_obj
	);
	archive(pa.number_of_partitions());
	for (AoL::SizeT i = 0; i < pa.number_of_partitions(); ++i)
	{
		auto& sub_partition = pa.get_partition(i);
		archive(
			sub_partition.max_size(),
			sub_partition.size()
		);
	}
}

template<
	typename Archive,
	typename T,
	AoL::SizeT S
>
void load(Archive& archive, AoL::PartitionArray<T, S>& pa)
{
	typename AoL::PartitionArray<T, S>::container_type new_array;
	AoL::SizeT sub_partitions_size;
	AoL::SizeT max_size;
	AoL::SizeT current_size;

	archive(
		new_array,
		sub_partitions_size
	);

	pa.assign(std::move(new_array));
	for (AoL::SizeT i = 0; i < sub_partitions_size - 1; ++i)
	{
		archive(
			max_size,
			current_size
		);
		auto& sub_partition = pa.create_partition(max_size, false);
		auto empty_size = max_size - current_size;
		if (empty_size > 0)
		{
			sub_partition.erase(current_size, empty_size);
		}
	}
}
#endif // AOL_HEADER_PARTITIONS_H

}


#endif // !AOL_HEADER_INTERNAL_SERIALIZATION_CONTAINERS_H