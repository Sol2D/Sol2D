// Sol2D Game Engine
// Copyright (C) 2023-2024 Sergey Smolyannikov aka brainstream
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

#include <optional>

namespace Sol2D {

enum class BodyType
{
    Static = 0,
    Dynamic = 1,
    Kinematic = 2
};

std::optional<BodyType> castToBodyType(std::integral auto _integer)
{
    switch(_integer) {
    case static_cast<decltype(_integer)>(BodyType::Static):
        return BodyType::Static;
    case static_cast<decltype(_integer)>(BodyType::Dynamic):
        return BodyType::Dynamic;
    case static_cast<decltype(_integer)>(BodyType::Kinematic):
        return BodyType::Kinematic;
    default:
        return std::optional<BodyType>();
    }
}

} // namespace Sol2D
