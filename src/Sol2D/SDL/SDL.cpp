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

#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
#include <Sol2D/SDL/SDL.h>

void Sol2D::SDL::sdlRenderCircle(SDL_Renderer * _renderer, const SDL_FPoint & _center, uint32_t _radius) // TODO: float radius
{
    constexpr double pi = std::numbers::pi;
    constexpr uint16_t min_points_count_per_quarter = 2;
    constexpr uint16_t k = 6;

    uint16_t points_count_per_quarter = _radius / k;
    if(points_count_per_quarter < min_points_count_per_quarter)
        points_count_per_quarter = min_points_count_per_quarter;
    std::vector<SDL_FPoint> points(points_count_per_quarter * 4 + 1);
    SDL_FPoint * q1 = points.data();
    SDL_FPoint * q2 = q1 + points_count_per_quarter;
    SDL_FPoint * q3 = q2 + points_count_per_quarter;
    SDL_FPoint * q4 = q3 + points_count_per_quarter;

    q1[0].x = _center.x +_radius;
    q1[0].y = _center.y;
    q2[0].x= _center.x;
    q2[0].y= _center.y + _radius;
    q3[0].x= _center.x - _radius;
    q3[0].y= _center.y;
    q4[0].x= _center.x;
    q4[0].y= _center.y - _radius;
    points[points.size() - 1] = points[0];

    const double base_angle = pi / (2 * points_count_per_quarter);

    for(uint16_t i = 1; i < points_count_per_quarter; ++i)
    {
        const double angle = base_angle * i;
        const double x = cos(angle) * _radius;
        const double y = sin(angle) * _radius;

        size_t mirror_index = points_count_per_quarter - i;

        int pos_x = _center.x + x;
        int neg_x = _center.x - x;
        int pos_y = _center.y + y;
        int neg_y = _center.y - y;

        q1[i].x = pos_x;
        q1[i].y = pos_y;
        q2[mirror_index].x = neg_x;
        q2[mirror_index].y = pos_y;
        q3[i].x = neg_x;
        q3[i].y = neg_y;
        q4[mirror_index].x = pos_x;
        q4[mirror_index].y = neg_y;
    }

    SDL_RenderLines(_renderer, q1, points.size());
}

bool operator == (const SDL_Color & _color1, const SDL_Color & _color2)
{
    return _color1.r == _color2.r &&
           _color1.g == _color2.g &&
           _color1.b == _color2.b &&
           _color1.a == _color2.a;
}


void Sol2D::SDL::sdlDetectContentRect(SDL_Surface & _surface, SDL_Rect & _rect)
{
    std::vector<bool> rows(_surface.h);
    std::vector<bool> columns(_surface.w);

    const uint8_t * surface_pixels = static_cast<const uint8_t *>(_surface.pixels);
    const Uint32 reference_pixel = SDL_MapRGBA(_surface.format, 0, 0, 0, 0);

    for(int y = 0; y < _surface.h; ++y)
    {
        for(int x = 0; x < _surface.w; ++x)
        {
            const Uint32 * pixel = reinterpret_cast<const Uint32 *>(
                &surface_pixels[(x + y * _surface.w) * _surface.format->bytes_per_pixel]
                );
            bool not_empty = std::memcmp(&reference_pixel, pixel, _surface.format->bytes_per_pixel) != 0;
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
