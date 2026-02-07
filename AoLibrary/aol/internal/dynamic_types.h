#pragma once

/***************************************************************************************
* AoLibrary dynamic_types
****************************************************************************************
* - Library's runtime-types
* - Use this for types where you need any runtime-related things you can't do on
*	compile time
***************************************************************************************/

#include <optional>
#include <variant>
#include <any>

namespace AoL
{

template<typename T>
using Optional = std::optional<T>;

template<typename... Ts>
using Variant = std::variant<Ts...>;

using AnyType = std::any;

}