/*****************************************************
 * Data components serialization implementations
 *****************************************************/
#ifndef AOL_HEADER_INTERNAL_SERIALIZATION_DATA_COMPONENTS_H
#define AOL_HEADER_INTERNAL_SERIALIZATION_DATA_COMPONENTS_H


#include "aol/configs.h"
#include "aol/macros.h"
#include "aol/traits.h"
#include "aol/types.h"


namespace cereal
{

#if defined(AOL_HEADER_DATA_COMPONENTS_PROFILES_H)
/****************************************
* DataID
****************************************/

template<typename Archive, typename T>
void serialize(Archive& archive, AoL::Internal::DataID<T>& data_id)
{
	archive(data_id.value_);
}

/****************************************
* DataBasic
****************************************/

template<typename Archive, typename Derived>
void save(Archive& archive, const AoL::DataBasic<Derived>& data)
{
	archive(
		data.name,
		data.description,
		data.short_name,
		data.id
	);
}

template<typename Archive, typename Derived>
void load(Archive& archive, AoL::DataBasic<Derived>& data)
{
	using DataBasic = AoL::DataBasic<Derived>;

	archive(
		data.name,
		data.description,
		data.short_name,
		data.id
	);

	if (data.name.size() > DataBasic::Default::NameLength)
	{
		data.name.resize(DataBasic::Default::NameLength);
	}

	if (data.description.size() > DataBasic::Default::DescriptionLength)
	{
		data.description.resize(DataBasic::Default::DescriptionLength);
	}

	if (data.short_name.size() > DataBasic::Default::ShortNameLength)
	{
		data.short_name.resize(DataBasic::Default::ShortNameLength);
	}
}

/****************************************
* DataMinimal
****************************************/

template<typename Archive, typename Derived>
void save(Archive& archive, const AoL::DataMinimal<Derived>& data)
{
	archive(
		data.name,
		data.id
	);
}

template<typename Archive, typename Derived>
void load(Archive& archive, AoL::DataMinimal<Derived>& data)
{
	using DataBasic = AoL::DataMinimal<Derived>;

	archive(
		data.name,
		data.id
	);

	if (data.name.size() > DataBasic::Default::NameLength)
	{
		data.name.resize(DataBasic::Default::NameLength);
	}
}
#endif // AOL_HEADER_DATA_COMPONENTS_PROFILES_H

} // cereal namespace


#endif // !AOL_HEADER_INTERNAL_SERIALIZATION_DATA_COMPONENTS_H