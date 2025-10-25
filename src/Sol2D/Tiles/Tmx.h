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

#include <Sol2D/Tiles/TileMap.h>
#include <Sol2D/Workspace.h>
#include <Sol2D/Def.h>
#include <filesystem>

namespace Sol2D::Tiles {

struct Tmx
{
    S2_DISABLE_COPY(Tmx)

    Tmx(std::unique_ptr<TileHeap> && _tile_heap,
        std::unique_ptr<ObjectHeap> _object_heap,
        std::unique_ptr<TileMap> && _tile_map) :
        tile_heap(std::move(_tile_heap)),
        object_heap(std::move(_object_heap)),
        tile_map(std::move(_tile_map))
    {
    }

    Tmx(Tmx && _tmx) :
        tile_heap(std::move(_tmx.tile_heap)),
        object_heap(std::move(_tmx.object_heap)),
        tile_map(std::move(_tmx.tile_map))
    {
    }

    Tmx & operator= (Tmx && _tmx)
    {
        if(this != &_tmx)
        {
            tile_heap = std::move(_tmx.tile_heap);
            object_heap = std::move(_tmx.object_heap);
            tile_map = std::move(_tmx.tile_map);
        }
        return *this;
    }

    std::unique_ptr<TileHeap> tile_heap;
    std::unique_ptr<ObjectHeap> object_heap;
    std::unique_ptr<TileMap> tile_map;
};

Tmx loadTmx(Renderer & _renderer, const Workspace & _workspace, const std::filesystem::path & _path);

} // namespace Sol2D::Tiles
