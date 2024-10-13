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

#include <Sol2D/Rect.h>
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
        is_visible(true),
        position{.0f, .0f},
        scale_factor{1.0f, 1.0f}
    {
    }

    GraphicsPackSpriteDefinition(std::shared_ptr<SpriteSheet> _sprite_sheet, size_t _sprite_index) :
        sprite(GraphicsPackSpriteSheetSpriteDefinition(_sprite_sheet, _sprite_index)),
        is_visible(true),
        position{.0f, .0f},
        scale_factor{1.0f, 1.0f}
    {
    }

    std::variant<std::shared_ptr<Sprite>, GraphicsPackSpriteSheetSpriteDefinition> sprite;
    bool is_visible;
    Point position;
    Point scale_factor;
};

struct GraphicsPackFrameDefinition
{
    S2_DEFAULT_COPY_AND_MOVE(GraphicsPackFrameDefinition)

    GraphicsPackFrameDefinition() :
        duration(std::chrono::milliseconds::zero()),
        is_visible(true)
    {
    }

    std::chrono::milliseconds duration;
    std::vector<GraphicsPackSpriteDefinition> sprites;
    bool is_visible;
};

struct GraphicsPackDefinition
{
    S2_DEFAULT_COPY_AND_MOVE(GraphicsPackDefinition)

    GraphicsPackDefinition() :
        position{.0f, .0f},
        is_flipped_horizontally(false),
        is_flipped_vertically(false),
        animation_iterations(-1)
    {
    }

    Point position;
    std::vector<GraphicsPackFrameDefinition> frames;
    std::optional<Point> flip_center;
    bool is_flipped_horizontally;
    bool is_flipped_vertically;
    int32_t animation_iterations;
};

} // namespace Sol2D
