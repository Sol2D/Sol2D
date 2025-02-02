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

#include <Sol2D/MediaLayer/Rotation.h>
#include <Sol2D/MediaLayer/Rect.h>
#include <Sol2D/MediaLayer/Texture.h>
#include <SDL3/SDL_pixels.h>
#include <optional>

namespace Sol2D {

struct RectRenderingDataBase
{
    RectRenderingDataBase(
        const SDL_FRect & _rect,
        const std::optional<Rotation> & _rotation) :
        rect(_rect),
        rotation(_rotation)
    {
    }

    SDL_FRect rect;
    std::optional<Rotation> rotation;
};

struct SolidRectRenderingData : RectRenderingDataBase
{
    SolidRectRenderingData(
        const SDL_FRect & _rect,
        const SDL_FColor & _color,
        const std::optional<Rotation> & _rotation = std::nullopt
    ) :
        RectRenderingDataBase(_rect, _rotation),
        color(_color)
    {
    }

    SDL_FColor color;
};

struct RectRenderingData : RectRenderingDataBase
{
    RectRenderingData(
        const SDL_FRect & _rect,
        const SDL_FColor & _border_color,
        float _border_width = 1.0f,
        const SDL_FColor & _color = { .0f, .0f, .0f, .0f },
        const std::optional<Rotation> & _rotation = std::nullopt
    ) :
        RectRenderingDataBase(_rect, _rotation),
        border_color(_border_color),
        border_width(_border_width),
        color(_color)
    {
    }

    SDL_FColor border_color;
    float border_width;
    SDL_FColor color;
};

struct TextureRenderingData : RectRenderingDataBase
{
    TextureRenderingData(
        const SDL_FRect & _rect,
        const Texture & _texture,
        const std::optional<SDL_FRect> & _texture_rect,
        const std::optional<Rotation> _rotation = std::nullopt,
        SDL_FlipMode _flip_mode = SDL_FLIP_NONE
    ) :
        RectRenderingDataBase(_rect, _rotation),
        texture(_texture),
        texture_rect(_texture_rect),
        flip_mode(_flip_mode)
    {
    }

    Texture texture;
    std::optional<SDL_FRect> texture_rect;
    SDL_FlipMode flip_mode;
};

} // namespace Sol2D
