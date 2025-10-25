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

#include <Sol2D/MediaLayer/MediaLayer.h>
#include <filesystem>
#include <optional>

namespace Sol2D {

struct SpritePaddings
{
    SpritePaddings(float _value = .0f) :
        top(_value),
        right(_value),
        bottom(_value),
        left(_value)
    {
    }

    SpritePaddings(float _top_bottom, float _right_left) :
        top(_top_bottom),
        right(_right_left),
        bottom(_top_bottom),
        left(_right_left)
    {
    }

    SpritePaddings(float _top, float _right, float _bottom, float _left) :
        top(_top),
        right(_right),
        bottom(_bottom),
        left(_left)
    {
    }

    float top;
    float right;
    float bottom;
    float left;
};

struct SpriteOptions
{
    SpriteOptions() :
        autodetect_rect(false)
    {
    }

    std::optional<SpritePaddings> paddings;
    std::optional<SDL_FRect> rect;
    std::optional<SDL_FColor> color_to_alpha;
    bool autodetect_rect;
};

class Sprite final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(Sprite)

    explicit Sprite(Renderer & _renderer);
    Sprite(
        Renderer & _renderer,
        const Texture & _texture,
        const SDL_FRect & _rect,
        const SpritePaddings & _paddings = .0f);
    bool loadFromFile(const std::filesystem::path & _path, const SpriteOptions & _options = SpriteOptions());
    bool isValid() const;
    void scaleTo(const FSize & _size);
    void scale(float _scale_factor);
    void scale(float _scale_factor_x, float _scale_factor_y);
    void render(const SDL_FPoint & _point, const Rotation & _rotation, SDL_FlipMode _flip_mode);

private:
    Renderer * m_renderer;
    Texture m_texture;
    SDL_FRect m_source_rect;
    SpritePaddings m_paddings;
    SDL_FRect m_desination_rect;
};

inline Sprite::Sprite(Renderer & _renderer) :
    m_renderer(&_renderer),
    m_source_rect(.0f, .0f, .0f, .0f),
    m_desination_rect(.0f, .0f, .0f, .0f)
{
}

inline Sprite::Sprite(
    Renderer & _renderer,
    const Texture & _texture,
    const SDL_FRect & _rect,
    const SpritePaddings & _paddings
) :
    m_renderer(&_renderer),
    m_texture(_texture),
    m_source_rect(_rect),
    m_paddings(_paddings),
    m_desination_rect(.0f, .0f, _rect.w, _rect.h)
{
}

inline bool Sprite::isValid() const
{
    return m_texture != nullptr;
}

} // namespace Sol2D
