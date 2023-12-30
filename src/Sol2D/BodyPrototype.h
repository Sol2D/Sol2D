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

#include <Sol2D/Body.h>
#include <Sol2D/Sprite.h>
#include <Sol2D/Def.h>
#include <filesystem>
#include <vector>
#include <set>
#include <optional>

namespace Sol2D {

struct SpriteSheetSettings
{
    uint32_t sprite_width;
    uint32_t sprite_height;
    uint32_t row_count;
    uint32_t col_count;
    uint32_t margin_top;
    uint32_t margin_left;
    uint32_t horizintal_spacing;
    uint32_t vertical_spacing;
    std::set<SpriteIndex> ignores; // TODO: delete
    std::optional<SDL_Color> color_to_alpha;
};

class BodyPrototype
{
    DISABLE_COPY(BodyPrototype)

public:
    explicit BodyPrototype(SDL_Renderer & _renderer);
    ~BodyPrototype();
    bool loadSpriteSheet(const std::filesystem::path & _path, const SpriteSheetSettings & _settings);
    const std::vector<Sprite> & getSprites() const;
    uint32_t getSpriteWidth() const;
    uint32_t getSpriteHeight() const;

private:
    SDL_Renderer & mr_renderer;
    std::vector<Sprite> m_sprites;
    std::vector<SDL_Texture *> m_textures;
    uint32_t m_sprite_width;
    uint32_t m_sprite_height;
};

inline const std::vector<Sprite> & BodyPrototype::getSprites() const
{
    return m_sprites;
}

inline uint32_t BodyPrototype::getSpriteWidth() const
{
    return m_sprite_width;
}

inline uint32_t BodyPrototype::getSpriteHeight() const
{
    return m_sprite_height;
}

} // namespace Sol2D
