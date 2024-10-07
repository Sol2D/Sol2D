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

#include <Sol2D/Dimension.h>

namespace Sol2D::Forms {

struct WidgetPadding
{
    S2_DEFAULT_COPY_AND_MOVE(WidgetPadding)

    WidgetPadding() : WidgetPadding(.0f)
    {
    }

    // cppcheck-suppress noExplicitConstructor
    WidgetPadding(float _pixels) :
        WidgetPadding(_pixels, _pixels, _pixels, _pixels)
    {
    }

    // cppcheck-suppress noExplicitConstructor
    WidgetPadding(const Dimension<float> & _padding) :
        WidgetPadding(_padding, _padding, _padding, _padding)
    {
    }

    WidgetPadding(
        const Dimension<float> & _top,
        const Dimension<float> & _right,
        const Dimension<float> & _bottom,
        const Dimension<float> & _left
        ) :
        top(_top),
        right(_right),
        bottom(_bottom),
        left(_left)
    {
    }

    WidgetPadding & operator = (float _pixels)
    {
        top = _pixels;
        right = _pixels;
        bottom = _pixels;
        left = _pixels;
        return *this;
    }

    Dimension<float> top;
    Dimension<float> right;
    Dimension<float> bottom;
    Dimension<float> left;
};

} // namespace Sol2D::Forms
