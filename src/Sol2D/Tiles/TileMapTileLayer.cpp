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

#include <Sol2D/Tiles/TileMapTileLayer.h>

using namespace Sol2D::Tiles;

TileMapTileLayer::TileMapTileLayer(
    const TileMapLayer * _parent,
    const TileHeap & _tile_heap,
    uint32_t _id,
    const std::string & _name,
    uint32_t _tile_width,
    uint32_t _tile_height,
    int32_t _x,
    int32_t _y,
    uint32_t _width,
    uint32_t _height
) :
    TileMapLayer(_parent, _id, _name, TileMapLayerType::Tile),
    mr_tile_heap(_tile_heap),
    m_tile_width(_tile_width),
    m_tile_height(_tile_height),
    m_x(_x),
    m_y(_y),
    m_width(_width),
    m_height(_height)
{
    mp_cells = static_cast<TileMapTileLayerCell *>(malloc(m_width * m_height * sizeof(TileMapTileLayerCell)));
    for(size_t x = 0; x < m_width; ++x)
    {
        for(size_t y = 0; y < m_height; ++y)
        {
            new (getMatrixCell(x, y)) TileMapTileLayerCell(m_x + x, m_y + y);
        }
    }
}

TileMapTileLayer::~TileMapTileLayer()
{
    free(mp_cells);
}

void TileMapTileLayer::setTile(int32_t _x, int32_t _y, uint32_t _gid)
{
    const uint32_t matrix_x = toMatrixX(_x);
    const uint32_t matrix_y = toMatrixY(_y);
    const Tile * tile = mr_tile_heap.getTile(_gid);
    TileMapTileLayerCell * cell = getMatrixCell(matrix_x, matrix_y);
    if(!tile || !cell)
        return;
    eraseTile(_x, _y);
    cell->tile = tile;
    const uint32_t this_tile_width = tile->getWidth();
    const uint32_t this_tile_height = tile->getHeight();
    uint32_t spread_right = 0;
    uint32_t spread_top = 0;
    if(this_tile_width > m_tile_width)
    {
        uint32_t extra_width = this_tile_width - m_tile_width;
        spread_right = extra_width / m_tile_width;
        if(extra_width % m_tile_width)
            ++spread_right;
    }
    if(this_tile_height > m_tile_height)
    {
        uint32_t extra_height = this_tile_height - m_tile_height;
        spread_top = extra_height / m_tile_height;
        if(extra_height % m_tile_height)
            ++spread_top;
    }
    if(spread_right || spread_top)
    {
        for(uint32_t dx = 1; dx <= spread_right; ++dx)
        {
            uint32_t x = matrix_x + dx;
            if(x >= m_width) continue;
            for(uint32_t dy = 1; dy <= spread_top; ++dy)
            {
                int32_t y = matrix_y - dy;
                if(y < 0) continue;
                TileMapTileLayerCell * slave_cell = getMatrixCell(x, y);
                if(slave_cell)
                {
                    slave_cell->master_cells.push_back(cell);
                    cell->slave_cells.push_back(slave_cell);
                }
            }
        }
    }
}

bool TileMapTileLayer::eraseTile(int32_t _x, int32_t _y)
{
    TileMapTileLayerCell * cell = getLayerCell(_x, _y);
    if(!cell)
        return false;
    cell->tile = nullptr;
    for(auto * slave : cell->slave_cells)
        slave->master_cells.remove(cell);
    cell->slave_cells.clear();
    return true;
}

const Tile * TileMapTileLayer::getTileAtPoint(int32_t _x, int32_t _y) const
{
    // TODO: offsets
    return m_tile_width && m_tile_height ? getTile(_x / m_tile_width, _y / m_tile_height) : nullptr;
}

const Tile * TileMapTileLayer::getTile(int32_t _x, int32_t _y) const
{
    TileMapTileLayerCell * cell = getLayerCell(_x, _y);
    return cell ? cell->tile : nullptr;
}
