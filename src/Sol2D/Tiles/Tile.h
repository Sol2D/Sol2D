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

#include <Sol2D/Tiles/TileSet.h>
#include <Sol2D/MediaLayer/MediaLayer.h>

namespace Sol2D::Tiles {

class Tile final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(Tile)

    Tile(
        const TileSet & _set, const Texture & _source, int32_t _src_x, int32_t _src_y, uint32_t _width, uint32_t _height
    ) :
        m_set(&_set),
        m_x(_src_x),
        m_y(_src_y),
        m_width(_width),
        m_height(_height),
        m_source_ptr(_source)
    {
    }

    ~Tile() = default;

    const TileSet getTileSet() const
    {
        return *m_set;
    }

    int32_t getSourceX() const
    {
        return m_x;
    }

    int32_t getSourceY() const
    {
        return m_y;
    }

    uint32_t getWidth() const
    {
        return m_width;
    }

    uint32_t getHeight() const
    {
        return m_height;
    }

    const Texture & getSource() const
    {
        return m_source_ptr;
    }

private:
    const TileSet * m_set;
    int32_t m_x, m_y;
    uint32_t m_width, m_height;
    Texture m_source_ptr;
};

} // namespace Sol2D::Tiles
