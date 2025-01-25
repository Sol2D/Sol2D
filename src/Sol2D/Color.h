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
#include <cstdint>

namespace Sol2D {

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    operator const SDL_Color & () const noexcept { return *reinterpret_cast<const SDL_Color *>(this); }
};

struct ColorF
{
    ColorF() :
        r(.0f),
        g(.0f),
        b(.0f),
        a(.0f)
    {
    }

    ColorF(float _r, float _g, float _b, float _a = 1.0f) :
        r(_r),
        g(_g),
        b(_b),
        a(_a)
    {
    }

    ColorF(const Color & _color) :
        r(_color.r / 255.0f),
        g(_color.g / 255.0f),
        b(_color.b / 255.0f),
        a(_color.a / 255.0f)
    {
    }

    ColorF(const ColorF &) = default;

    ColorF & operator = (const Color & _color)
    {
        r = _color.r / 255.0f;
        g = _color.g / 255.0f;
        b = _color.b / 255.0f;
        a = _color.a / 255.0f;
        return *this;
    }

    ColorF & operator = (const ColorF &) = default;

    float r;
    float g;
    float b;
    float a;
};

} // namespace Sol2D
