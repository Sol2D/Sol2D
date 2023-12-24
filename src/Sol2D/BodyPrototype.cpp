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
#include <algorithm>

using namespace Sol2D;

namespace fs = std::filesystem;

BodyPrototype::BodyPrototype(SDL_Renderer & _renderer) :
    mr_renderer(_renderer)
{
}

BodyPrototype::~BodyPrototype()
{
    for(SDL_Texture * texture : m_textures)
        SDL_DestroyTexture(texture);
}

SpriteIndex BodyPrototype::loadSprite(const fs::path & _file)
{
    SDL_Surface * surface = IMG_Load(_file.c_str());
    if(!surface)
        return INVALID_SPRITE_INDEX;
    SDL_Texture * texture = SDL_CreateTextureFromSurface(&mr_renderer, surface);
    int width = surface->w;
    int height = surface->h;
    SDL_DestroySurface(surface);
    if(!texture)
        return INVALID_SPRITE_INDEX;
    m_textures.push_back(texture);
    m_sprites.push_back(Sprite {
        .texture = texture,
        .rect = { 0, 0, width, height }
    });
    return m_sprites.size() - 1;
}

SpriteIndex BodyPrototype::loadSpriteSheet(const fs::path & _path, const SpriteSheetSettings & _settings)
{
    if(_settings.col_count == 0 || _settings.row_count == 0)
        return INVALID_SPRITE_INDEX;
    SDL_Surface * surface = IMG_Load(_path.c_str());
    if(!surface)
        return INVALID_SPRITE_INDEX;
    if(_settings.color_to_alpha.has_value())
    {
        SDL_Color color = _settings.color_to_alpha.value();
        SDL_SetSurfaceColorKey(
            surface,
            SDL_TRUE,
            SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a)
        );
    }
    int image_width = surface->w;
    int image_height = surface->h;
    SDL_Texture * texture = SDL_CreateTextureFromSurface(&mr_renderer, surface);
    m_textures.push_back(texture);
    SDL_DestroySurface(surface);
    int sprite_width = (image_width - _settings.padding_left - _settings.padding_right) / _settings.col_count;
    int sprite_height = (image_height - _settings.padding_top - _settings.padding_bottom) / _settings.row_count;
    if(sprite_width == 0 || sprite_height == 0)
        return INVALID_SPRITE_INDEX;
    SpriteIndex result = m_sprites.size();
    size_t index = 0;
    for(uint16_t row = 0; row < _settings.row_count; ++row)
    {
        for(uint16_t col = 0; col < _settings.col_count; ++col)
        {
            if(_settings.ignores.contains(index++))
                continue;
            m_sprites.push_back(Sprite {
                .texture = texture,
                .rect = {
                    _settings.padding_left + col * sprite_width,
                    _settings.padding_top + row * sprite_height,
                    sprite_width,
                    sprite_height
                }
            });
        }
    }
    return result;
}
