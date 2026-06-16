/*****************************************************
* Container's serialization implementations
*****************************************************/

#ifndef NDEBUG
#include "aol/internal/containers.h" // included only for debugging purposes
#endif

namespace cereal
{

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

}