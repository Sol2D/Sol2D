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

#include <Sol2D/Utils/Math.h>
#include <Sol2D/SDL/SDL.h>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>

using namespace Sol2D;
using namespace Sol2D::Utils;

namespace {

void renderCircles(SDL_Renderer * _renderer, uint32_t _radius, const SDL_FPoint * _centers, size_t _count)
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
        points.reserve(points.size() + 8 * _count);
        for(size_t i = 0; i < _count; ++i)
        {
            points.push_back({ .x = _centers[i].x + x, .y = _centers[i].y + y });
            points.push_back({ .x = _centers[i].x + y, .y = _centers[i].y + x });
            points.push_back({ .x = _centers[i].x - y, .y = _centers[i].y + x });
            points.push_back({ .x = _centers[i].x - x, .y = _centers[i].y + y });
            points.push_back({ .x = _centers[i].x - x, .y = _centers[i].y - y });
            points.push_back({ .x = _centers[i].x - y, .y = _centers[i].y - x });
            points.push_back({ .x = _centers[i].x + y, .y = _centers[i].y - x });
            points.push_back({ .x = _centers[i].x + x, .y = _centers[i].y - y });
        }
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

} // namespace

void Sol2D::SDL::sdlRenderCircle(SDL_Renderer * _renderer, const SDL_FPoint & _center, float _radius)
{
    renderCircles(_renderer, _radius, &_center, 1);
}

void Sol2D::SDL::sdlRenderCapsule(
    SDL_Renderer * _renderer,
    const SDL_FPoint & _center1,
    const SDL_FPoint & _center2,
    float _radius)
{
    SDL_FPoint centers[2];
    if(_center1.y < _center2.y)
    {
        centers[0] = _center1;
        centers[1] = _center2;
    }
    else
    {
        centers[0] = _center2;
        centers[1] = _center1;
    }

    const double delta_x = _center2.x - _center1.x;
    const double delta_y = _center2.y - _center1.y;
    const double rect_height = std::sqrt(delta_x * delta_x + delta_y * delta_y);
    const double sine = delta_y / rect_height;
    const double cosine = delta_x / rect_height;
    VectorRotator rotator(Rotation(sine, cosine));
    Point delta = rotator.rotate(makePoint(.0f, _radius - 1));
    SDL_RenderLine(
        _renderer,
        centers[0].x + delta.x,
        centers[0].y + delta.y,
        centers[1].x + delta.x,
        centers[1].y + delta.y);
    delta = rotator.rotate(makePoint(.0f, 1 - _radius));
    SDL_RenderLine(
        _renderer,
        centers[0].x + delta.x,
        centers[0].y + delta.y,
        centers[1].x + delta.x,
        centers[1].y + delta.y);

    renderCircles(_renderer, _radius, centers, 2);
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
