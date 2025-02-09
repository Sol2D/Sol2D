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

#include <SDL3/SDL_rect.h>
#include <Sol2D/MediaLayer/Rotation.h>

namespace Sol2D {

class Capsule final
{
public:
    Capsule(float _radius, const SDL_FPoint & _center1, const SDL_FPoint & _center2);
    float getRadius() const { return m_radius; }
    const Rotation & getRotation() const { return m_rotation; }
    const SDL_FRect & getRect() const { return m_rect; }

private:
    float m_radius;
    Rotation m_rotation;
    SDL_FRect m_rect;
};

} // namespace Sol2D
