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
    uint16_t row_count;
    uint16_t col_count;
    uint16_t padding_top;
    uint16_t padding_right;
    uint16_t padding_bottom;
    uint16_t padding_left;
    std::set<SpriteIndex> ignores; // TODO: delete
    std::optional<SDL_Color> color_to_alpha;
};

class BodyPrototype
{
    DISABLE_COPY(BodyPrototype)

public:
    explicit BodyPrototype(SDL_Renderer & _renderer);
    ~BodyPrototype();
    SpriteIndex loadSprite(const std::filesystem::path & _file);
    SpriteIndex loadSpriteSheet(const std::filesystem::path & _path, const SpriteSheetSettings & _settings);
    const std::vector<Sprite> & getSprites() const;

private:
    SDL_Renderer & mr_renderer;
    std::vector<Sprite> m_sprites;
    std::vector<SDL_Texture *> m_textures;
};

inline const std::vector<Sprite> & BodyPrototype::getSprites() const
{
    return m_sprites;
}

} // namespace Sol2D
