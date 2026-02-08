#pragma once

/***********************************************************
* AoLibrary Data Component Profiles
************************************************************
* - Header provides basic types for data-centric types
* - Variety of choices for different cases
* - Read type comments for more information on the type
***********************************************************/

#include "aol/internal/config.h"
#include "aol/internal/macros.h"

#include "aol/internal/types.h"
#include "aol/internal/strings.h"

#include "type_safe/strong_typedef.hpp"

#include <optional>
#include <limits>

namespace AoL
{

namespace Internal
{

/**
* The strong_typedef for ID to be used for DataBasic<T>
* 
* - Safely get underlying type via DataID<T>::UType
* 
* @tparam T The type tag
*/
template<typename T>
struct DataID : public
	type_safe::strong_typedef<DataID<T>, SizeT>,
	type_safe::strong_typedef_op::integer_arithmetic<DataID<T>>,
	type_safe::strong_typedef_op::equality_comparison<DataID<T>>,
	type_safe::strong_typedef_op::relational_comparison<DataID<T>>
{
private:
	using BaseST = type_safe::strong_typedef<DataID<T>, SizeT>;

public:
	using BaseST::strong_typedef;

	using UType = typename type_safe::underlying_type<BaseST>;
};

struct AOL_EMPTY_BASE_OPTIMIZATION DataDefaultName
{
	static constexpr const char* Name{ "N/A" };
	static constexpr SizeT NameLength{ 128 };
};

struct AOL_EMPTY_BASE_OPTIMIZATION DataDefaultDescription
{
	static constexpr const char* Description{ "N/A" };
	static constexpr SizeT DescriptionLength{ 1024 };
};

struct AOL_EMPTY_BASE_OPTIMIZATION DataDefaultShortName
{
	static constexpr const char* ShortName{ "N/A" };
	static constexpr SizeT ShortNameLength{ 32 };
};

template<typename T>
struct AOL_EMPTY_BASE_OPTIMIZATION DataDefaultID
{
	static constexpr DataID<T> ID { std::numeric_limits<DataID<T>::UType>::max() };
};

template<typename T>
struct AOL_EMPTY_BASE_OPTIMIZATION DataBasicDefaults : 
	DataDefaultName,
	DataDefaultShortName,
	DataDefaultDescription,
	DataDefaultID<T>
{};

template<typename T>
struct AOL_EMPTY_BASE_OPTIMIZATION DataMinimalDefaults :
	DataDefaultName,
	DataDefaultID<T>
{};

}

/**
 * Base structure for data-driven objects used throughout the system.
 * 
 * - This class is CRTP-based and must be inherited or type-tagged before use
 * 
 * - The derived or type-tagged class will have its own strong_typed ID to help with type safety
 * 
 * - Default values can be accessed through DataBasic<Derived>::Default
 * 
 * - Best used for Array of Structures (AoS)
 *
 * @tparam Derived The derived class or the type-tagged class
 */
template<typename Derived>
struct DataBasic : private Internal::DataBasicDefaults<Derived>
{
public:
	using IDT		= Internal::DataID<Derived>;
	using Default	= Internal::DataBasicDefaults<Derived>;

	String	name;
	String	short_name;
	String	description;
	IDT		id;

protected:
	DataBasic() noexcept :
		name{ Default::Name },
		short_name{ Default::ShortName },
		description{ Default::Description },
		id{ Default::ID }
	{
	}
};

/**
 * Minimal base structure for data-driven objects used throughout the system.
 * 
 * - Compared to DataBasic, this only has two member, [ name ] and [ id ]
 *
 * - This class is CRTP-based and must be inherited or type-tagged before use
 *
 * - The derived or type-tagged class will have its own strong_typed ID to help with type safety
 *
 * - Default values can be accessed through DataMinimal<Derived>::Default
 *
 * - Best used for Array of Structures (AoS)
 *
 * @tparam Derived The derived class or the type-tagged class
 */
template<typename Derived>
struct DataMinimal : private Internal::DataMinimalDefaults<Derived>
{
public:
	using IDT		= Internal::DataID<Derived>;
	using Default	= Internal::DataMinimalDefaults<Derived>;

	String	name;
	IDT		id;

protected:
	DataMinimal() noexcept :
		name{ Default::Name },
		id{ Default::ID }
	{
	}
};


}