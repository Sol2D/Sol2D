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

#include <Sol2D/BodyPrototype.h>
#include <SDL3_image/SDL_image.h>

using namespace Sol2D;

namespace fs = std::filesystem;

BodyPrototype::BodyPrototype(SDL_Renderer & _renderer) :
    mr_renderer(_renderer),
    m_sprite_width{0},
    m_sprite_height{0}
{
}

BodyPrototype::~BodyPrototype()
{
    for(SDL_Texture * texture : m_textures)
        SDL_DestroyTexture(texture);
}

bool BodyPrototype::loadSpriteSheet(const fs::path & _path, const SpriteSheetSettings & _settings)
{
    m_sprites.clear();
    if(!_settings.row_count || !_settings.col_count || !_settings.sprite_width || !_settings.sprite_height)
        return false;
    SDL_Surface * surface = IMG_Load(_path.c_str());
    if(!surface)
        return false;
    if(_settings.color_to_alpha.has_value())
    {
        SDL_Color color = _settings.color_to_alpha.value();
        SDL_SetSurfaceColorKey(
            surface,
            SDL_TRUE,
            SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a)
        );
    }
    SDL_Texture * texture = SDL_CreateTextureFromSurface(&mr_renderer, surface);
    m_textures.push_back(texture);
    SDL_DestroySurface(surface);
    size_t index = 0;
    SDL_Rect rect = {
        .x = 0,
        .y = 0,
        .w = static_cast<int>(_settings.sprite_width),
        .h = static_cast<int>(_settings.sprite_height)
    };
    m_sprite_width = _settings.sprite_width;
    m_sprite_height = _settings.sprite_height;
    for(uint16_t row = 0; row < _settings.row_count; ++row)
    {
        rect.y = _settings.margin_top + row * _settings.sprite_height + row * _settings.vertical_spacing;
        for(uint16_t col = 0; col < _settings.col_count; ++col)
        {
            if(_settings.ignores.contains(index++))
                continue;
            rect.x = _settings.margin_left + col * _settings.sprite_width + col * _settings.horizintal_spacing;
            m_sprites.push_back(Sprite {
                .texture = texture,
                .rect = rect
            });
        }
    }
    return true;
}
