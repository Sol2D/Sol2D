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

#include <cstring>
#include <vector>
#include <algorithm>
#include <Sol2D/SDL/SDL.h>

void Sol2D::SDL::sdlRenderCircle(SDL_Renderer * _renderer, const SDL_FPoint & _center, uint32_t _radius) // TODO: float radius
{
    const int radius = static_cast<int>(_radius);
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    std::vector<SDL_FPoint> points;
    while(x >= y)
    {
        points.reserve(points.size() + 8);
        points.push_back({ .x = _center.x + x, .y = _center.y + y });
        points.push_back({ .x = _center.x + y, .y = _center.y + x });
        points.push_back({ .x = _center.x - y, .y = _center.y + x });
        points.push_back({ .x = _center.x - x, .y = _center.y + y });
        points.push_back({ .x = _center.x - x, .y = _center.y - y });
        points.push_back({ .x = _center.x - y, .y = _center.y - x });
        points.push_back({ .x = _center.x + y, .y = _center.y - x });
        points.push_back({ .x = _center.x + x, .y = _center.y - y });
        if(err <= 0)
        {
            ++y;
            err += dy;
            dy += 2;
        }
        if(err > 0)
        {
            --x;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
    SDL_RenderPoints(_renderer, points.data(), points.size());
}

bool operator == (const SDL_Color & _color1, const SDL_Color & _color2)
{
    return _color1.r == _color2.r &&
           _color1.g == _color2.g &&
           _color1.b == _color2.b &&
           _color1.a == _color2.a;
}


void Sol2D::SDL::sdlDetectContentRect(const SDL_Surface & _surface, SDL_Rect & _rect)
{
    std::vector<bool> rows(_surface.h);
    std::vector<bool> columns(_surface.w);

    const uint8_t * surface_pixels = static_cast<const uint8_t *>(_surface.pixels);
    const SDL_PixelFormatDetails * pixel_format = SDL_GetPixelFormatDetails( _surface.format);
    const Uint32 reference_pixel = SDL_MapRGBA(pixel_format, nullptr, 0, 0, 0, 0);

    for(int y = 0; y < _surface.h; ++y)
    {
        for(int x = 0; x < _surface.w; ++x)
        {
            const Uint32 * pixel = reinterpret_cast<const Uint32 *>(
                &surface_pixels[(x + y * _surface.w) * pixel_format->bits_per_pixel]
            );
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
