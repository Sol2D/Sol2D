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

#include <box2d/box2d.h>
#include <vector>
#include <optional>

namespace Sol2D::World {

struct AStarOptions
{
    AStarOptions() :
        allow_diagonal_steps(false),
        avoid_sensors(false)
    {
    }

    bool allow_diagonal_steps;
    bool avoid_sensors;
};

std::optional<std::vector<b2Vec2>> aStarFindPath(
    b2WorldId _world_id, b2BodyId _body_id, const b2Vec2 & _destination, const AStarOptions & _options
);

} // namespace Sol2D::World
