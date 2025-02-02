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
#include <box2d/box2d.h>

namespace Sol2D {

void detectContentRect(const SDL_Surface & _surface, SDL_Rect & _rect);

inline const b2Vec2 & toBox2D(const SDL_FPoint & _sdl_point)
{
    return *reinterpret_cast<const b2Vec2 *>(&_sdl_point);
}

inline const SDL_FPoint & toSDL(const b2Vec2 & _b2_vec)
{
    return *reinterpret_cast<const SDL_FPoint *>(&_b2_vec);
}

} // namespace Sol2D
