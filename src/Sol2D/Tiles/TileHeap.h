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

#include <Sol2D/Tiles/Tile.h>
#include <Sol2D/Def.h>
#include <vector>
#include <unordered_map>

namespace Sol2D::Tiles {

class TileHeap final
{
    S2_DISABLE_COPY(TileHeap)

public:
    TileHeap();
    ~TileHeap();
    TileSet & createTileSet();
    Tile * createTile(uint32_t _gid, const TileSet & _set, std::shared_ptr<SDL_Texture> _source,
                     int32_t _src_x, int32_t _src_y, uint32_t _width, uint32_t _height);
    uint32_t getNextGid() const;
    const Tile * getTile(uint32_t _gid) const;
    Tile * getTile(uint32_t _gid);

private:
    std::vector<TileSet *> m_sets;
    std::unordered_map<uint32_t, Tile *> m_tiles;
    uint32_t m_next_gid;
};

inline TileHeap::TileHeap() :
    m_next_gid(0)
{
}

inline uint32_t TileHeap::getNextGid() const
{
    return m_next_gid;
}

inline const Tile * TileHeap::getTile(uint32_t _gid) const
{
    auto it = m_tiles.find(_gid);
    return it == m_tiles.cend() ? nullptr : it->second;
}

inline Tile * TileHeap::getTile(uint32_t _gid)
{
    auto it = m_tiles.find(_gid);
    return it == m_tiles.end() ? nullptr : it->second;
}

} // namespace Sol2D::Tiles
