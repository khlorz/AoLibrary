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

}