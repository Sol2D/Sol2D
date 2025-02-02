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

#include <SDL3/SDL.h>

namespace Sol2D {

inline SDL_Color toR8G8B8A8_UINT(const SDL_FColor & _color)
{
    return
    {
        .r = static_cast<uint8_t>(_color.r * 255.0f),
        .g = static_cast<uint8_t>(_color.g * 255.0f),
        .b = static_cast<uint8_t>(_color.b * 255.0f),
        .a = static_cast<uint8_t>(_color.a * 255.0f)
    };
}

inline SDL_FColor toR32G32B32A32_SFLOAT(const SDL_Color & _color)
{
    return
    {
        .r = _color.r / 255.0f,
        .g = _color.g / 255.0f,
        .b = _color.b / 255.0f,
        .a = _color.a / 255.0f
    };
}

} // namespace Sol2D
