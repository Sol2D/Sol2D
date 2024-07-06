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

#include <Sol2D/SpriteSheet.h>
#include <SDL3_image/SDL_image.h>

using namespace Sol2D;
using namespace Sol2D::SDL;

SpriteSheet::SpriteSheet(SDL_Renderer & _renderer) :
    mp_renderer(&_renderer),
    m_texture_ptr(nullptr)
{
}

bool SpriteSheet::loadFromFile(const std::filesystem::path & _path, const SpriteSheetOptions & _options)
{
    m_texture_ptr.reset();
    if(!_options.row_count || !_options.col_count || !_options.sprite_width || !_options.sprite_height)
        return false;
    SDL_Surface * surface = IMG_Load(_path.c_str());
    if(!surface)
        return false;
    if(_options.color_to_alpha.has_value())
    {
        Color color = _options.color_to_alpha.value();
        SDL_SetSurfaceColorKey(
            surface,
            SDL_TRUE,
            SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a)
            );
    }
    m_texture_ptr = wrapSdlTexturePtr(SDL_CreateTextureFromSurface(mp_renderer, surface));
    SDL_DestroySurface(surface);
    Rect rect = makeRect(
        .0f,
        .0f,
        static_cast<float>(_options.sprite_width),
        static_cast<float>(_options.sprite_height));
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
