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

#include <SDL3/SDL_rect.h>
#include <box2d/math_functions.h>

namespace Sol2D {

struct Point
{
    float x;
    float y;

    operator const SDL_FPoint & () const noexcept
    {
        return *toSdlPtr();
    }

    operator const b2Vec2 & () const noexcept
    {
        return *toBox2DPtr();
    }

    const SDL_FPoint * toSdlPtr() const noexcept
    {
        return reinterpret_cast<const SDL_FPoint *>(this);
    }

    const b2Vec2 * toBox2DPtr() const noexcept
    {
        return reinterpret_cast<const b2Vec2 *>(this);
    }

    Point operator + (const Point & _point) const noexcept
    {
        return { x + _point.x, y + _point.y };
    }
};

struct Size
{
    float w;
    float h;
};

struct Rect
{
    float x;
    float y;
    float w;
    float h;

    Point getPoint() const noexcept
    {
        return { x,  y };
    }

    operator Point () const noexcept
    {
        return { x, y };
    }

    Size getSize() const noexcept
    {
        return { w,  h };
    }

    operator Size () const noexcept
    {
        return { w, h };
    }

    operator const SDL_FRect & () const noexcept
    {
        return *toSdlPtr();
    }

    const SDL_FRect * toSdlPtr() const noexcept
    {
        return reinterpret_cast<const SDL_FRect *>(this);
    }
};

inline constexpr Point makePoint(float _x, float _y) noexcept
{
    return { _x,  _y };
}

inline const Point & asPoint(const b2Vec2 & _vec) noexcept
{
    return *reinterpret_cast<const Point *>(&_vec);
}

inline const b2Vec2 & asBox2dVec2(const Point & _point) noexcept
{
    return *reinterpret_cast<const b2Vec2 *>(&_point);
}

inline constexpr Size makeSize(float _w, float _h) noexcept
{
    return { _w, _h };
}

inline constexpr Rect makeRect(float _x, float _y, float _w, float _h) noexcept
{
    return { _x, _y, _w, _h };
}

inline constexpr Rect makeRect(const Point & _point, float _w, float _h) noexcept
{
    return { _point.x, _point.y, _w, _h };
}

inline constexpr Rect makeRect(float _x, float _y, const Size & _size) noexcept
{
    return { _x, _y, _size.h, _size.w };
}

inline constexpr Rect makeRect(const Point & _point, const Size & _size) noexcept
{
    return { _point.x, _point.y, _size.w, _size.w };
}

} // namespace Sol2D
