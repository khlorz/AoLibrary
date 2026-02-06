#pragma once

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