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

#include <optional>

namespace Sol2D::World {

enum class BodyShapeType
{
    Polygon,
    Circle,
    Capsule
};

std::optional<BodyShapeType> castToBodyShapeType(std::integral auto _integer)
{
    switch(_integer) {
    case static_cast<decltype(_integer)>(BodyShapeType::Polygon):
        return BodyShapeType::Polygon;
    case static_cast<decltype(_integer)>(BodyShapeType::Circle):
        return BodyShapeType::Circle;
    default:
        return std::nullopt;
    }
}

} // namespace Sol2D::World
