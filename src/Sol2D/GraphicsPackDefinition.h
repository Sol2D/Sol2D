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

#pragma once

#include <Sol2D/SpriteSheet.h>
#include <cstdint>
#include <chrono>
#include <vector>
#include <variant>

namespace Sol2D {

struct GraphicsPackSpriteSheetSpriteDefinition
{
    S2_DEFAULT_COPY_AND_MOVE(GraphicsPackSpriteSheetSpriteDefinition)

    GraphicsPackSpriteSheetSpriteDefinition(std::shared_ptr<SpriteSheet> _sprite_sheet, size_t _sprite_index) :
        sprite_sheet(_sprite_sheet),
        sprite_index(_sprite_index)
    {
    }

    std::shared_ptr<SpriteSheet> sprite_sheet;
    size_t sprite_index;
};

struct GraphicsPackSpriteDefinition
{
    S2_DEFAULT_COPY_AND_MOVE(GraphicsPackSpriteDefinition)

    GraphicsPackSpriteDefinition() :
        GraphicsPackSpriteDefinition(nullptr)
    {
    }

    explicit GraphicsPackSpriteDefinition(std::shared_ptr<Sprite> _sprite) :
        sprite(_sprite),
        position {.0f, .0f},
        scale_factor {1.0f, 1.0f},
        is_visible(true)
    {
    }

    GraphicsPackSpriteDefinition(std::shared_ptr<SpriteSheet> _sprite_sheet, size_t _sprite_index) :
        sprite(GraphicsPackSpriteSheetSpriteDefinition(_sprite_sheet, _sprite_index)),
        position {.0f, .0f},
        scale_factor {1.0f, 1.0f},
        is_visible(true)
    {
    }

    std::variant<std::shared_ptr<Sprite>, GraphicsPackSpriteSheetSpriteDefinition> sprite;
    SDL_FPoint position;
    SDL_FPoint scale_factor;
    uint16_t z_index = 0;
    bool is_visible;
};

struct GraphicsPackFrameDefinition
{
    std::chrono::milliseconds duration = std::chrono::milliseconds::zero();
    std::vector<GraphicsPackSpriteDefinition> sprites;
    bool is_visible = true;
};

struct GraphicsPackDefinition
{
    SDL_FPoint position = { .0f, .0f };
    std::vector<GraphicsPackFrameDefinition> frames;
    int32_t animation_iterations = -1;
    bool is_flipped_horizontally = false;
    bool is_flipped_vertically = false;
};

} // namespace Sol2D
