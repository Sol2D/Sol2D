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
#include <Sol2D/Tiles/TileHeap.h>
#include <list>

namespace Sol2D::Tiles {

struct TileMapTileLayerCell
{
    TileMapTileLayerCell(int32_t _x, int32_t _y) :
        x(_x),
        y(_y),
        tile(nullptr)
    {
    }

    const int32_t x;
    const int32_t y;
    const Tile * tile;
    std::list<TileMapTileLayerCell *> master_cells;
    std::list<TileMapTileLayerCell *> slave_cells;
};

class TileMapTileLayer : public TileMapLayer
{
public:
    TileMapTileLayer(
        const TileHeap & _tile_heap,
        uint32_t _id,
        const std::string & _name,
        uint32_t _tile_width,
        uint32_t _tile_height,
        int32_t _x,
        int32_t _y,
        uint32_t _width,
        uint32_t _height);
    ~TileMapTileLayer() override;
    int32_t getX() const{ return m_x; }
    int32_t getY() const { return m_y; }
    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }
    void setTile(int32_t _x, int32_t _y, uint32_t _gid);
    bool eraseTile(int32_t _x, int32_t _y);
    const Tile * getTileAtPoint(int32_t _x, int32_t _y) const;
    const Tile * getTile(int32_t _x, int32_t _y) const;
    const TileMapTileLayerCell * getCell(int32_t _x, int32_t _y) const { return getLayerCell(_x, _y); }

private:
    TileMapTileLayerCell * getLayerCell(uint32_t _x, uint32_t _y) const
    {
        return getMatrixCell(toMatrixX(_x), toMatrixY(_y));
    }

    TileMapTileLayerCell * getMatrixCell(uint32_t _x, uint32_t _y) const
    {
        return _x < m_width && _y < m_height ? &mp_cells[m_width * _y + _x] : nullptr;
    }

    uint32_t toMatrixX(int32_t _layer_x) const
    {
        return _layer_x - m_x;
    }

    uint32_t toMatrixY(int32_t _layer_y) const
    {
        return _layer_y - m_y;
    }

private:
    const TileHeap & mr_tile_heap;
    uint32_t m_tile_width;
    uint32_t m_tile_height;
    int32_t m_x;
    int32_t m_y;
    uint32_t m_width;
    uint32_t m_height;
    TileMapTileLayerCell * mp_cells;
};

} // namespace Tiles::Sol2D
