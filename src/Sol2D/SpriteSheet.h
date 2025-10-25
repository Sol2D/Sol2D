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

#include <Sol2D/Sprite.h>
#include <vector>
#include <optional>
#include <filesystem>

namespace Sol2D {

struct SpriteSheetOptions
{
    uint32_t sprite_width;
    uint32_t sprite_height;
    uint32_t row_count;
    uint32_t col_count;
    uint32_t margin_top;
    uint32_t margin_left;
    uint32_t horizontal_spacing;
    uint32_t vertical_spacing;
    std::optional<SDL_FColor> color_to_alpha;
};

struct SpriteSheetFrame
{
    SDL_FRect texture_rect;
    FSize sprite_size;
    SDL_FPoint sprite_point;
    bool is_rotated;
};

class SpriteSheet final
{
public:
    S2_DISABLE_COPY(SpriteSheet)
    S2_DEFAULT_MOVE(SpriteSheet)

    explicit SpriteSheet(Renderer & _renderer);
    bool loadFromFile(const std::filesystem::path & _path, const SpriteSheetOptions & _options);
    bool loadFromAtlas(const std::filesystem::path & _path);
    bool isValid() const;
    size_t getSpriteCount() const;
    Sprite toSprite(size_t _idx) const;
    const std::vector<SpriteSheetFrame> & getFrames() const; // TODO: delete
    const Texture & getTexture() const; // TODO: delete

private:
    Renderer * m_renderer;
    Texture m_texture;
    std::vector<SpriteSheetFrame> m_frames;
};

inline bool SpriteSheet::isValid() const
{
    return m_texture && !m_frames.empty();
}

inline size_t SpriteSheet::getSpriteCount() const
{
    return m_frames.size();
}

inline Sprite SpriteSheet::toSprite(size_t _idx) const
{
    return _idx >= m_frames.size() ? Sprite(*m_renderer) : Sprite(*m_renderer, m_texture, m_frames[_idx].texture_rect); // TODO: Frame
}

inline const std::vector<SpriteSheetFrame> & SpriteSheet::getFrames() const
{
    return m_frames;
}

inline const Texture & SpriteSheet::getTexture() const
{
    return m_texture;
}

} // namespace Sol2D
