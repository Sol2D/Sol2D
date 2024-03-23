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

#include <Sol2D/SpriteRenderOptions.h>
#include <Sol2D/SDL.h>
#include <Sol2D/Def.h>
#include <filesystem>
#include <optional>

namespace Sol2D {

struct SpriteOptions
{
    std::optional<SDL_FRect> rect;
    std::optional<SDL_Color> color_to_alpha;
};

class Sprite final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(Sprite)

    explicit Sprite(SDL_Renderer & _renderer);
    bool loadFromFile(const std::filesystem::path & _path, const SpriteOptions & _options = SpriteOptions());
    bool isValid() const;
    SDL_TexturePtr getTexture() const;
    const SDL_FRect & getRect() const;
    void render(const SDL_FPoint & _point, SpriteRenderOptions _options = SpriteRenderOptions());

private:
    SDL_Renderer * mp_renderer;
    SDL_TexturePtr m_texture_ptr;
    SDL_FRect m_source_rect;
};

inline Sprite::Sprite(SDL_Renderer & _renderer) :
    mp_renderer(&_renderer),
    m_source_rect({ .0f, .0f, .0f, .0f })
{
}

inline bool Sprite::isValid() const
{
    return m_texture_ptr != nullptr;
}

inline SDL_TexturePtr Sprite::getTexture() const
{
    return m_texture_ptr;
}

inline const SDL_FRect & Sprite::getRect() const
{
    return m_source_rect;
}

} // namespace Sol2D
