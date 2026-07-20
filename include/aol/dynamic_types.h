/***************************************************************************************
* AoLibrary dynamic_types
****************************************************************************************
* - Library's runtime-types
* - Use this for types where you need any runtime-related things you can't do on
*	compile time
***************************************************************************************/
#ifndef AOL_HEADER_DYNAMIC_TYPES_H
#define AOL_HEADER_DYNAMIC_TYPES_H


#include "configs.h"
#include "macros.h"
#include "traits.h"
#include "types.h"

#include <optional>	// std::optional
#include <variant>	// std::variant
#include <any>		// std::any


namespace AoL
{

template<typename T>
using Optional = std::optional<T>;

template<typename... Ts>
using Variant = std::variant<Ts...>;

using AnyType = std::any;

}

#endif // AOL_HEADER_DYNAMIC_TYPES_H