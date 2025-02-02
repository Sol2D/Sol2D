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

#include <Sol2D/MediaLayer/Utils.h>
#include <cstring>

using namespace Sol2D;

void Sol2D::detectContentRect(const SDL_Surface & _surface, SDL_Rect & _rect)
{
    std::vector<bool> rows(_surface.h);
    std::vector<bool> columns(_surface.w);

    const uint8_t * surface_pixels = static_cast<const uint8_t *>(_surface.pixels);
    const SDL_PixelFormatDetails * pixel_format = SDL_GetPixelFormatDetails( _surface.format);
    const uint32_t reference_pixel = SDL_MapRGBA(pixel_format, nullptr, 0, 0, 0, 0);

    for(int y = 0; y < _surface.h; ++y)
    {
        for(int x = 0; x < _surface.w; ++x)
        {
            const uint32_t * pixel = reinterpret_cast<const Uint32 *>(
                &surface_pixels[(x + y * _surface.w) * pixel_format->bits_per_pixel]);
            bool not_empty = std::memcmp(&reference_pixel, pixel, pixel_format->bytes_per_pixel) != 0;
            rows[y] = rows[y] || not_empty;
            columns[x] = columns[x] || not_empty;
        }
    }

    _rect.x = 0;
    _rect.y = 0;
    _rect.w = _surface.w;
    _rect.h = _surface.h;

    {
        auto it = std::find(columns.begin(), columns.end(), true);
        if(it != columns.end())
        {
            _rect.x = it - columns.begin();
            if(_rect.x == _rect.w - 1) // The image is empty
            {
                _rect.x = 0;
                return;
            }
            _rect.w -= _rect.x;
        }
    }

    {
        auto it = std::find(columns.rbegin(), columns.rend(), true);
        if(it != columns.rend())
            _rect.w -= it - columns.rbegin();
    }

    {
        auto it = std::find(rows.begin(), rows.end(), true);
        if(it != rows.end())
        {
            _rect.y = it - rows.begin();
            _rect.h -= _rect.y;
        }
    }

    {
        auto it = std::find(rows.rbegin(), rows.rend(), true);
        if(it != rows.rend())
            _rect.h -= it - rows.rbegin();
    }
}
