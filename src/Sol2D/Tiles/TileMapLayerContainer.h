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

#include <Sol2D/Tiles/TileMapTileLayer.h>
#include <Sol2D/Tiles/TileMapObjectLayer.h>
#include <Sol2D/Tiles/TileMapImageLayer.h>
#include <Sol2D/Tiles/TileHeap.h>
#include <Sol2D/Tiles/ObjectHeap.h>
#include <list>

namespace Sol2D::Tiles {

class TileMapGroupLayer;

class TileMapLayerContainer
{
public:
    TileMapLayerContainer(const TileHeap & _tile_heap, const ObjectHeap & _object_heap) :
        mr_tile_heap(_tile_heap),
        mr_object_heap(_object_heap)
    {
    }

    virtual ~TileMapLayerContainer();
    TileMapTileLayer & createTileLayer(
        uint32_t _id,
        const std::string & _name,
        uint32_t _tile_width,
        uint32_t _tile_height,
        int32_t _x,
        int32_t _y,
        uint32_t _width,
        uint32_t _height);
    TileMapObjectLayer & createObjectLayer(uint32_t _id, const std::string & _name);
    TileMapImageLayer & createImageLayer(uint32_t _id, const std::string & _name);
    TileMapGroupLayer & createGroupLayer(uint32_t _id, const std::string & _name);

    void forEachLayer(std::function<void(const TileMapLayer &)> _cb) const
    {
        for(const auto * layer : m_layers)
            _cb(*layer);
    }

    const TileMapLayer * getLayer(const std::string _name) const
    {
        auto it = m_layers_by_names.find(_name);
        return it == m_layers_by_names.cend() ? nullptr : it->second;
    }

    TileMapLayer * getLayer(const std::string _name)
    {
        auto it = m_layers_by_names.find(_name);
        return it == m_layers_by_names.end() ? nullptr : it->second;
    }

private:
    void storeLayer(const std::string & _name, TileMapLayer * _layer)
    {
        m_layers.push_back(_layer);
        m_layers_by_names[_name] = _layer;
    }

private:
    const TileHeap & mr_tile_heap;
    const ObjectHeap & mr_object_heap;
    std::list<TileMapLayer *> m_layers;
    std::unordered_map<std::string, TileMapLayer *> m_layers_by_names;
};

class TileMapGroupLayer : public TileMapLayer, public TileMapLayerContainer
{
public:
    TileMapGroupLayer(
        const uint32_t _id,
        const std::string & _name,
        const TileHeap & _tile_heap,
        const ObjectHeap & _object_heap
        ) :
        TileMapLayer(_id, _name, TileMapLayerType::Group),
        TileMapLayerContainer(_tile_heap, _object_heap)
    {
    }
};

} // namespace Sol2D::Tiles
