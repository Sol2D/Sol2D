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

struct BodyShapePhysicsDefinition
{
    BodyShapePhysicsDefinition() :
        is_sensor(false),
        is_pre_solve_enabled(false)
    {
    }

    std::optional<float> density;
    std::optional<float> restitution;
    std::optional<float> friction;
    bool is_sensor;
    bool is_pre_solve_enabled;
};

} // namespace Sol2D::World
