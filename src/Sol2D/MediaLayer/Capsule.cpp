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

#include <Sol2D/MediaLayer/Capsule.h>

using namespace Sol2D;

Capsule::Capsule(float _radius, const SDL_FPoint & _center1, const SDL_FPoint & _center2) :
    m_radius(_radius)
{
    SDL_FPoint aabb_min, aabb_max;
    if(_center1.x > _center2.x)
    {
        aabb_min.x = _center2.x;
        aabb_max.x = _center1.x;
    }
    else
    {
        aabb_min.x = _center1.x;
        aabb_max.x = _center2.x;
    }
    if(_center1.y > _center2.y)
    {
        aabb_min.y = _center2.y;
        aabb_max.y = _center1.y;
    }
    else
    {
        aabb_min.y = _center1.y;
        aabb_max.y = _center2.y;
    }
    aabb_min.x -= _radius;
    aabb_min.y -= _radius;
    aabb_max.x += _radius;
    aabb_max.y += _radius;

    if(_center1.x == _center2.x) // Already a vertical capsule
    {
        m_rect.x = aabb_min.x;
        m_rect.y = aabb_min.y;
        m_rect.w = _radius * 2;
        m_rect.h = aabb_max.y - aabb_min.y;
        return;
    }

    //
    // If the capsule is not vertical, it needs to be rotated.
    //

    const float ab = std::abs(_center1.y - _center2.y);
    const float ac = std::abs(_center1.x - _center2.x);
    const float bc = std::sqrt(ab * ab + ac * ac);
    const float sine = ab / bc;
    const float cosine = ac / bc;
    m_rotation.reset(cosine, -sine); // Rotate 90 degrees since we are expecting a vertical capsule.

    SDL_FPoint origin_center_of_entire_capsule
    {
        .x = aabb_min.x + (aabb_max.x - aabb_min.x) / 2,
        .y = aabb_min.y + (aabb_max.y - aabb_min.y) / 2
    };

    SDL_FPoint new_center1
    {
        .x = _center1.x - origin_center_of_entire_capsule.x,
        .y = _center1.y - origin_center_of_entire_capsule.y
    };
    SDL_FPoint new_center2
    {
        .x = _center2.x - origin_center_of_entire_capsule.x,
        .y = _center2.y - origin_center_of_entire_capsule.y
    };
    new_center1 = m_rotation.rotateVectorCW(new_center1);
    new_center2 = m_rotation.rotateVectorCW(new_center2);
    new_center1.x += origin_center_of_entire_capsule.x;
    new_center1.y += origin_center_of_entire_capsule.y;
    new_center2.x += origin_center_of_entire_capsule.x;
    new_center2.y += origin_center_of_entire_capsule.y;

    m_rect.x = std::min(new_center1.x, new_center2.x) - _radius;
    m_rect.y = std::min(new_center1.y, new_center2.y) - _radius;
    m_rect.w = _radius * 2;
    m_rect.h = std::abs(new_center2.y - new_center1.y) + _radius * 2;
}
