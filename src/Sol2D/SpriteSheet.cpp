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

#include <Sol2D/SpriteSheet.h>

using namespace Sol2D;

SpriteSheet::SpriteSheet(Renderer & _renderer) :
    m_renderer(&_renderer)
{
}

bool SpriteSheet::loadFromFile(const std::filesystem::path & _path, const SpriteSheetOptions & _options)
{
    if(!_options.row_count || !_options.col_count || !_options.sprite_width || !_options.sprite_height)
        return false;
    SDL_Surface * surface = IMG_Load(_path.c_str());
    if(!surface)
        return false;
    if(_options.color_to_alpha.has_value())
    {
        SDL_Color color = toR8G8B8A8_UINT(_options.color_to_alpha.value());
        const SDL_PixelFormatDetails * pixel_format = SDL_GetPixelFormatDetails(surface->format);
        SDL_SetSurfaceColorKey(surface, true, SDL_MapRGBA(pixel_format, nullptr, color.r, color.g, color.b, color.a));
    }
    m_texture = m_renderer->createTexture(*surface, "Sprite Sheet");
    SDL_DestroySurface(surface);
    SDL_FRect rect = {
        .x = .0f,
        .y = .0f,
        .w = static_cast<float>(_options.sprite_width),
        .h = static_cast<float>(_options.sprite_height)
    };
    for(uint16_t row = 0; row < _options.row_count; ++row)
    {
        rect.y = _options.margin_top + row * _options.sprite_height + row * _options.vertical_spacing;
        for(uint16_t col = 0; col < _options.col_count; ++col)
        {
            rect.x = _options.margin_left + col * _options.sprite_width + col * _options.horizontal_spacing;
            m_rects.push_back(rect);
        }
    }
    return true;
}
