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

#include <Sol2D/SpriteAnimation.h>
#include <SDL3/SDL_rect.h>
#include <optional>
#include <variant>

namespace Sol2D {

struct BodyShapeGraphicOptions
{
    BodyShapeGraphicOptions() :
        position{.0f, .0f},
        is_flipped_horizontally(false),
        is_flipped_vertically(false)
    {
    }

    SDL_FPoint position;
    std::optional<SDL_FPoint> size;
    bool is_flipped_horizontally;
    bool is_flipped_vertically;

    SDL_RendererFlip getFlip() const
    {
        int flip = SDL_FLIP_NONE;
        if(is_flipped_horizontally) flip |= SDL_FLIP_HORIZONTAL;
        if(is_flipped_vertically) flip |= SDL_FLIP_VERTICAL;
        return static_cast<SDL_RendererFlip>(flip);
    }
};

using BodyShapeGraphicVariant = std::variant<Sprite, SpriteAnimation>;

struct BodyShapeGraphic
{
    BodyShapeGraphicVariant graphic;
    BodyShapeGraphicOptions options;
};

} // namespace Sol2D
