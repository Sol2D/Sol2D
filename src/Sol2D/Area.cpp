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

#include <Sol2D/Area.h>

using namespace Sol2D;

SDL_FRect Area::calculateRect(const FSize & _output_size) const
{
    SDL_FRect rect;
    rect.x = left.has_value() ? left.value().getPixels(_output_size.w) : .0f;
    rect.y = top.has_value() ? top.value().getPixels(_output_size.h) : .0f;

    if(width.has_value())
    {
        rect.w = width.value().getPixels(_output_size.w);
    }
    else if(right.has_value())
    {
        float right_margin = right.value().getPixels(_output_size.w);
        rect.w = _output_size.w - (rect.x + right_margin);
    }
    else
    {
        rect.w = _output_size.w - rect.x;
    }

    if(height.has_value())
    {
        rect.h = height.value().getPixels(_output_size.h);
    }
    else if(bottom.has_value())
    {
        float bottom_margin = bottom.value().getPixels(_output_size.h);
        rect.h = _output_size.h - (rect.y + bottom_margin);
    }
    else
    {
        rect.h = _output_size.h - rect.y;
    }

    return rect;
}
