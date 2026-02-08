#pragma once

/***************************************************************************************
* AoLibrary serialization
****************************************************************************************
* - Serialization of the library types require the cereal library
* - To serialize the type, just include this header and that's it
***************************************************************************************/

#include "cereal/cereal.hpp"										/* main serialization include */
#include "cereal/types/base_class.hpp"								/* for inheritance types*/
#include "cereal/types/string.hpp"									/* std::string and the likes */
#include "cereal/types/vector.hpp"									/* std::vector and the likes */
#include "cereal/types/array.hpp"									/* std::array and the likes */
#include "cereal/types/set.hpp"										/* std::set and the likes */
#include "cereal/types/unordered_set.hpp"							/* std::unordered_set and the likes */
#include "cereal/types/concepts/pair_associative_container.hpp"		/* paired-containers like map and the likes */

#include "aol/internal/traits.h"


/*****************************************************
* Containers serialization
*****************************************************/

#include "aol/internal/containers.h"

namespace cereal
{

/****************************************
* Internal::ContainerBase
* - The base class for most containers
****************************************/

template<typename Archive, AoL::ContainerTypeTag CTT, typename T, typename C>
void serialize(Archive& archive, AoL::Internal::ContainerBase<CTT, T, C>& container)
{
	archive(container);
}

/****************************************
* Derived containers
* - Derived from Internal::ContainerBase
****************************************/

template<
	typename Archive,
	AoL::Traits::IsRotContainer C,
	typename B = C::Base
>
void serialize(Archive& archive, C& c)
{
	archive(base_class<B>(&c));
}

}


/****************************************
* Data-driven types serialization
****************************************/

#include "aol/data_components/profiles.h"

namespace cereal
{

/****************************************
* DataBasic
****************************************/


template<typename Archive, typename T>
void serialize(Archive& archive, AoL::Internal::DataID<T>& data_id)
{
	archive(data_id.value_);
}

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

	if (data.name.size() > DataBasic::Default::Name)
	{
		data.name.resize(DataBasic::Default::NameLength);
	}

	if (data.description.size() > DataBasic::Default::Description)
	{
		data.description.resize(DataBasic::Default::DescriptionLength);
	}

	if (data.short_name.size() > DataBasic::Default::ShortName)
	{
		data.short_name.resize(DataBasic::Default::ShortNameLength);
	}
}

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

	if (data.name.size() > DataBasic::Default::Name)
	{
		data.name.resize(DataBasic::Default::NameLength);
	}
}

} // namespace AoL