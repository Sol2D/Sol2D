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

#include <Sol2D/Tiles/TileMapLayer.h>

using namespace Sol2D::Tiles;

TileMapLayerContainer::~TileMapLayerContainer()
{
    for(auto * layer : m_layers)
        delete layer;
}

TileMapTileLayer & TileMapLayerContainer::createTileLayer(uint32_t _id,
                                                         const std::string & _name,
                                                         uint32_t _tile_width,
                                                         uint32_t _tile_height,
                                                         int32_t _x,
                                                         int32_t _y,
                                                         uint32_t _width,
                                                         uint32_t _height)
{
    TileMapTileLayer * layer = new TileMapTileLayer(mr_tile_heap,
                                                   _id,
                                                   _name,
                                                   _tile_width,
                                                   _tile_height,
                                                   _x,
                                                   _y,
                                                   _width,
                                                   _height);
    storeLayer(_name, layer);
    return *layer;
}

TileMapObjectLayer & TileMapLayerContainer::createObjectLayer(uint32_t _id, const std::string & _name)
{
    TileMapObjectLayer * layer = new TileMapObjectLayer(mr_object_heap, _id, _name);
    storeLayer(_name, layer);
    return *layer;
}

TileMapImageLayer & TileMapLayerContainer::createImageLayer(uint32_t _id, const std::string & _name)
{
    TileMapImageLayer * layer = new TileMapImageLayer(_id, _name);
    storeLayer(_name, layer);
    return *layer;
}

TileMapGroupLayer & TileMapLayerContainer::createGroupLayer(uint32_t _id, const std::string & _name)
{
    TileMapGroupLayer * layer = new TileMapGroupLayer(_id, _name, mr_tile_heap, mr_object_heap);
    storeLayer(_name, layer);
    return *layer;
}
