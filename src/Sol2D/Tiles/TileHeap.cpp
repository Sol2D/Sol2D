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

#include <Sol2D/Tiles/TileHeap.h>

using namespace Sol2D::Tiles;

TileHeap::~TileHeap()
{
    for(auto & kvp : m_tiles)
        delete kvp.second;
    for(auto & set : m_sets)
        delete set;
}

TileSet & TileHeap::createTileSet()
{
    TileSet * set = new TileSet();
    m_sets.push_back(set);
    return *set;
}

Tile * TileHeap::createTile(uint32_t _gid, const TileSet & _set, std::shared_ptr<SDL_Texture> _source,
                 int32_t _src_x, int32_t _src_y, uint32_t _width, uint32_t _height)
{
    if(m_tiles.contains(_gid))
        return nullptr;
    Tile * tile = new Tile(_set, _source, _src_x, _src_y, _width, _height);
    m_tiles[_gid] = tile;
    if(_gid >= m_next_gid)
        m_next_gid = _gid + 1;
    return tile;
}
