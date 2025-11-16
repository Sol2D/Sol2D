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
#include <Sol2D/World/BodyType.h>
#include <optional>

namespace Sol2D::World {

struct BodyPhysicsDefinition
{
    BodyType type = BodyType::Static;
    SDL_FPoint position;
    std::optional<float> linear_damping;
    std::optional<float> angular_damping;
    std::optional<b2Rot> rotation;
    std::optional<SDL_FPoint> linear_velocity;
    std::optional<float> angular_velocity;
    std::optional<float> gravity_scale;
    std::optional<float> sleep_threshold;
    bool is_rotation_allowed = true;
    bool is_motion_x_allowed = true;
    bool is_motion_y_allowed = true;
    bool is_sleep_enabled = true;
    bool is_awake = true;
    bool is_bullet = false;
    bool is_enabled = true;
    bool is_fast_rotation_allowed = false;
};

} // namespace Sol2D::World
