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

#include <Sol2D/BodyType.h>

namespace Sol2D {

struct BodyOptions
{
    BodyOptions() :
        type(BodyType::Static),
        linear_damping(.0f),
        angular_damping(.0f),
        fixed_rotation(false)
    {
    }

    BodyType type;
    float linear_damping;
    float angular_damping;
    bool fixed_rotation;
};

} // namespace Sol2D
