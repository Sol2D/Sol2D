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

#include <Sol2D/MediaLayer/Rotation.h>
#include <array>

namespace Sol2D {

using Vector4 = std::array<float, 4>;
using Matrix4x4 = std::array<Vector4, 4>;

namespace Transform {

inline Matrix4x4 createRotation(const Rotation & _rotation)
{
    return {
        Vector4 {_rotation.cosine, _rotation.sine,   .0f,  .0f },
        Vector4 {-_rotation.sine,  _rotation.cosine, .0f,  .0f },
        Vector4 {.0f,              .0f,              1.0f, .0f },
        Vector4 {.0f,              .0f,              .0f,  1.0f}
    };
}

inline Matrix4x4 createTranslation(float _x, float _y)
{
    return {
        Vector4 {1.0f, .0f,  .0f,  .0f },
        Vector4 {.0f,  1.0f, .0f,  .0f },
        Vector4 {.0f,  .0f,  1.0f, .0f },
        Vector4 {_x,   _y,   .0f,  1.0f}
    };
}

inline Matrix4x4 createScale(float _h, float _v)
{
    return {
        Vector4 {_h,  .0f, .0f,  .0f },
        Vector4 {.0f, _v,  .0f,  .0f },
        Vector4 {.0f, .0f, 1.0f, .0f },
        Vector4 {.0f, .0f, .0f,  1.0f}
    };
}

inline Matrix4x4 createScale(float _s)
{
    return {
        Vector4 {_s,  .0f, .0f,  .0f },
        Vector4 {.0f, _s,  .0f,  .0f },
        Vector4 {.0f, .0f, 1.0f, .0f },
        Vector4 {.0f, .0f, .0f,  1.0f}
    };
}

inline Matrix4x4 createOrtho(float _ratio)
{
    return {
        Vector4 {1.0f / _ratio, .0f,  .0f,  .0f },
        Vector4 {.0f,           1.0f, .0f,  .0f },
        Vector4 {.0f,           .0f,  1.0f, .0f },
        Vector4 {.0f,           .0f,  .0f,  1.0f}
    };
}

} // namespace Transform
} // namespace Sol2D
