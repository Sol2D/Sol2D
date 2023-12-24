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

#include <Sol2D/Def.h>
#include <cstdint>
#include <string>

namespace Sol2D::Tiles {

class TileSet
{
public:
    enum class ObjectAlignment
    {
        unspecified,
        top_left,
        top,
        top_right,
        left,
        center,
        right,
        bottom_left,
        bottom,
        bottom_right
    };

    enum class TileRenderSize
    {
        tile,
        grid
    };

    enum class FillMode
    {
        stretch,
        preserve_aspect_fit
    };

public:
    DEFAULT_COPY(TileSet)

    TileSet() :
        m_tile_width(0),
        m_tile_height(0),
        m_object_aligment(ObjectAlignment::unspecified),
        m_tile_render_size(TileRenderSize::tile),
        m_fill_mode(FillMode::stretch)
    {
    }

    void setName(const char * _name) { m_name = _name ? _name : std::string(); }
    const std::string & getName() const { return m_name; }
    void setClass(const char * _class) { m_class = _class ? _class : std::string(); }
    const std::string & getClass() const { return m_class; }
    void setTileWidth(uint32_t _width) { m_tile_width = _width; }
    uint32_t getTileWidth() const { return m_tile_width; }
    void setTileHeight(uint32_t _height) { m_tile_height = _height; }
    uint32_t getTileHeight() const { return m_tile_height; }
    void setObjectAlignment(ObjectAlignment _aligment) { m_object_aligment = _aligment; }
    ObjectAlignment getObjectAlignment() const { return m_object_aligment; }
    void setTileRenderSize(TileRenderSize _size) { m_tile_render_size = _size; }
    TileRenderSize getTileRenderSize() const { return m_tile_render_size; }
    void setFillMode(FillMode _mode) { m_fill_mode = _mode; }
    FillMode getFillMode() const { return m_fill_mode; }

private:
    std::string m_name;
    std::string m_class;
    uint32_t m_tile_width;
    uint32_t m_tile_height;
    ObjectAlignment m_object_aligment;
    TileRenderSize m_tile_render_size;
    FillMode m_fill_mode;
};

} // namespace Sol2D::Tiles
