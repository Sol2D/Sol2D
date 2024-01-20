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

#include <Sol2D/Tiles/TileMapLayer.h>
#include <Sol2D/Def.h>

namespace Sol2D::Tiles {

class TileMap final : public TileMapLayerContainer
{
    DISABLE_COPY_AND_MOVE(TileMap)

public:
    enum class Orientation
    {
        Orthogonal,
        Isometric,
        Staggered,
        Hexagonal
    };

    enum class RenderOrder
    {
        RightDown,
        RightUp,
        LeftDown,
        LeftUp
    };

    enum class StaggerIndex
    {
        Even,
        Odd
    };

    enum Axis
    {
        X,
        Y
    };

public:
    TileMap(const TileHeap & _tile_heap, const ObjectHeap & _object_heap) :
        TileMapLayerContainer(_tile_heap, _object_heap),
        m_orientation(Orientation::Orthogonal),
        m_render_order(RenderOrder::RightDown),
        m_x(0),
        m_y(0),
        m_width(0),
        m_height(0),
        m_tile_width(0),
        m_tile_height(0),
        m_hex_side_length(0),
        m_stagger_axis(Axis::Y),
        m_stagger_index(StaggerIndex::Odd),
        m_parallax_origin_x(0),
        m_parallax_origin_y(0),
        m_background_color{ 0, 0, 0, 0 }
    {
    }

    void setClass(const char * _class) { m_class = _class ? _class : std::string(); }
    const std::string & getClass() const { return m_class; }
    void setOrientation(Orientation _orientation) { m_orientation = _orientation; }
    Orientation getOrientation() const { return m_orientation; }
    void setRenderOrder(RenderOrder _order) { m_render_order = _order; }
    RenderOrder getRenderOrder() const { return m_render_order; }
    int32_t getX() const { return m_x; }
    int32_t getY() const { return m_y; }
    void setWidth(uint32_t _width) { m_width = _width; }
    uint32_t getWidth() const { return m_width; }
    void setHeight(uint32_t _height) { m_height = _height; }
    uint32_t getHeight() const { return m_height; }

    void expand(int _x, int _y, uint32_t _width, uint32_t _height) // TODO: not inline
    {
        int32_t delta_x = m_x - _x;
        if(delta_x > 0)
        {
            m_x -= delta_x;
            m_width += delta_x;
        }
        int32_t delta_y = m_y - _y;
        if(delta_y > 0)
        {
            m_y -= delta_y;
            m_height += delta_y;
        }
        if(m_width < _width)
            m_width = _width;
        if(m_height < _height)
            m_height = _height;
    }

    void setTileWidth(uint32_t _width) { m_tile_width = _width; }
    uint32_t getTileWidth() const { return m_tile_width; }
    void setTileHeight(uint32_t _height) { m_tile_height = _height; }
    uint32_t getTileHeight() const { return m_tile_height; }
    void setHexSideLength(uint32_t _length) { m_hex_side_length = _length; }
    uint32_t getHexSideLength() const { return m_hex_side_length; }
    void setStaggerAxis(Axis _axis) { m_stagger_axis = _axis; }
    Axis getStaggerAxis() const { return m_stagger_axis; }
    void setStaggerIndex(StaggerIndex _index) { m_stagger_index = _index; }
    StaggerIndex getStaggerIndex() const { return m_stagger_index; }
    void setParallaxOriginX(int32_t _value) { m_parallax_origin_x = _value; }
    int32_t getParallaxOriginX() const { return m_parallax_origin_x; }
    void setParallaxOriginY(int32_t _value) { m_parallax_origin_y = _value; }
    int32_t getParallaxOriginY() const { return m_parallax_origin_y; }
    void setBackgroundColor(const SDL_Color & _color) { m_background_color = _color; }
    const SDL_Color & getBackgroundColor() const { return m_background_color; }

private:
    std::string m_class;
    Orientation m_orientation;
    RenderOrder m_render_order;
    int32_t m_x;
    int32_t m_y;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_tile_width;
    uint32_t m_tile_height;
    uint32_t m_hex_side_length;
    Axis m_stagger_axis;
    StaggerIndex m_stagger_index;
    int32_t m_parallax_origin_x;
    int32_t m_parallax_origin_y;
    SDL_Color m_background_color;
};

} // namespace Sol2D::Tiles

