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

#include <box2d/box2d.h>

namespace Sol2D::World {

class Body;
class BodyShape;
class Joint;

inline Body * getUserData(b2BodyId _b2_body)
{
    return static_cast<Body *>(b2Body_GetUserData(_b2_body));
}

inline BodyShape * getUserData(b2ShapeId _b2_shape)
{
    return static_cast<BodyShape *>(b2Shape_GetUserData(_b2_shape));
}

inline Joint * getUserData(b2JointId _b2_joint)
{
    return static_cast<Joint *>(b2Joint_GetUserData(_b2_joint));
}

} // namespace Sol2D::World
