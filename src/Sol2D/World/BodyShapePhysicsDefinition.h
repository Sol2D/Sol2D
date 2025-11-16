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

#include <Sol2D/MediaLayer/MediaLayer.h>
#include <optional>

namespace Sol2D::World {

struct BodyShapePhysicsDefinition
{
    std::optional<float> density;
    std::optional<b2SurfaceMaterial> material;
    bool is_sensor = false;
    bool are_pre_solve_events_enabled = false;
    bool are_sensor_events_enabled = false;
    bool are_contact_events_enabled = false;
    bool are_hit_events_enabled = false;
};

} // namespace Sol2D::World
