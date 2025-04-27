// Sol2D Game Engine
// Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Lesser Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <variant>

namespace Sol2D::Utils {

template<typename T>
struct is_variant : std::false_type
{
};

template<typename... Args>
struct is_variant<std::variant<Args...>> : std::true_type
{
};

template<typename T>
inline constexpr bool is_variant_v = is_variant<T>::value;

} // namespace Sol2D::Utils
