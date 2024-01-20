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

#include <Sol2D/Tiles/TileHeap.h>
#include <Sol2D/Tiles/ObjectHeap.h>
#include <Sol2D/Def.h>
#include <Sol2D/SDL.h>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <optional>
#include <functional>
#include <list>


namespace Sol2D::Tiles {

namespace __Private {

constexpr uint32_t INVALID_TILE_GID = UINT32_MAX;

// In the case when a 32-bit machine is used, all X's will be truncated in the hash table keys.
// This way, all X's will be placed in buckets. Hash table performance becomes O(X) instead of O(1).
// Do not use this storage for 32-bit code. Use the TileMapTileMatrixStorage instead.
class TileMapTileFlatStorage final
{
public:
    bool eraseTile(int32_t _x, int32_t _y)
    {
        int64_t idx = getTileIndex(_x, _y);
        return m_tiles.erase(idx) > 0;
    }

    void setTile(int32_t _x, int32_t _y, uint32_t _gid)
    {
        int64_t idx = getTileIndex(_x, _y);
        m_tiles[idx] = _gid;
    }

    uint32_t getTile(int32_t _x, int32_t _y) const
    {
        auto it = m_tiles.find(getTileIndex(_x, _y));
        return it == m_tiles.cend() ? INVALID_TILE_GID : it->second;
    }

private:
    int64_t getTileIndex(int32_t _x, int32_t _y) const
    {
        int32_t coords[2] { _x, _y };
        return *reinterpret_cast<int64_t *>(coords);
    }

private:
    std::unordered_map<size_t, uint32_t> m_tiles;
};

// This storage uses double indexing, which is an overhead.
// But this is the only way to access a matrix cell on 32-bit machines.
// For 64-bit code, the TileMapTileFlatStorage must be used.
class TileMapTileMatrixStorage final
{
public:
    ~TileMapTileMatrixStorage()
    {
        for(auto & record : m_tiles)
            delete record.second;
    }

    bool eraseTile(int32_t _x, int32_t _y)
    {
        auto * row = getRow(_x);
        return row ? row->erase(_y) > 0 : false;
    }

    void setTile(int32_t _x, int32_t _y, uint32_t _gid)
    {
        auto * row = getRow(_x);
        if(!row)
        {
            row = new std::unordered_map<uint32_t, uint32_t>();
            m_tiles[_x] = row;
        }
        row->insert_or_assign(_y, _gid);
    }

    uint32_t getTile(int32_t _x, int32_t _y) const
    {
        auto * row = getRow(_x);
        if(row)
        {
            auto it = row->find(_y);
            return it == row->cend() ? INVALID_TILE_GID : it->second;
        }
        return INVALID_TILE_GID;
    }

private:
    std::unordered_map<uint32_t, uint32_t> * getRow(int32_t _x) const
    {
        auto it = m_tiles.find(_x);
        return it == m_tiles.cend() ? nullptr : it->second;
    }

private:
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t> *> m_tiles;
};

// A hash table with 64-bit keys on 32-bit machines will have too many collisions, so the matrix will work faster.
using TileMapTileStorage = std::conditional<sizeof(size_t) >= 8, TileMapTileFlatStorage, TileMapTileMatrixStorage>::type;

} // namespace __Private


class TileMapLayerContainer;
class TileMapLayer;
class TileMapTileLayer;
class TileMapObjectLayer;
class TileMapImageLayer;
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
    TileMapTileLayer & createTileLayer(uint32_t _id,
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


enum class TileMapLayerType
{
    Tile,
    Object,
    Image,
    Group
};

class TileMapLayer
{
    DISABLE_COPY_AND_MOVE(TileMapLayer)

public:
    TileMapLayer(const uint32_t _id, const std::string & _name, TileMapLayerType _type) :
        m_id(_id),
        m_name(_name),
        m_type(_type),
        m_is_visible(true),
        m_opacity(1.0f),
        m_offset_x(0),
        m_offset_y(0),
        m_parallax_x(1.0f),
        m_parallax_y(1.0f)
    {
    }

    virtual ~TileMapLayer() { }
    uint32_t getId() const { return m_id; }
    const std::string & getName() const { return m_name; }
    TileMapLayerType getType() const { return m_type; }
    void setClass(const char * _class) { m_class = _class ? _class : std::string(); }
    const std::string & getClass() const { return m_class; }
    void setVisibility(bool _visible) { m_is_visible = _visible; }
    bool isVisible() const { return m_is_visible; }
    void setOpacity(float _opacity) { m_opacity = _opacity; }
    float getOpacity() const { return m_opacity; }
    void setOffsetX(int32_t _offset) { m_offset_x = _offset; }
    int32_t getOffsetX() const { return m_offset_x; }
    void setOffsetY(int32_t _offset) { m_offset_y = _offset; }
    int32_t getOffsetY() const { return m_offset_y; }
    void setParallaxX(float _parallax) { m_parallax_x = _parallax; }
    float getParallaxX() const { return m_parallax_x; }
    void setParallaxY(float _parallax) { m_parallax_y = _parallax; }
    float getParallaxY() const { return m_parallax_y; }
    void setTintColor(const SDL_Color & _color) { m_tint_color = _color; }
    void removeTintColor() { m_tint_color.reset(); }
    const std::optional<SDL_Color> & getTintColor() const { return m_tint_color; }

private:
    uint32_t m_id;
    const std::string m_name;
    TileMapLayerType m_type;
    std::string m_class;
    bool m_is_visible;
    float m_opacity;
    int32_t m_offset_x; // TODO: To render offset
    int32_t m_offset_y;
    float m_parallax_x; // TODO: To render parallax https://doc.mapeditor.org/en/stable/manual/layers/#parallax-factor
    float m_parallax_y;
    std::optional<SDL_Color> m_tint_color; // TODO: To render tint color. https://doc.mapeditor.org/en/stable/manual/layers/#tint-color
};


class TileMapImageLayer : public TileMapLayer
{
public:
    TileMapImageLayer(uint32_t _id, const std::string & _name) :
        TileMapLayer(_id, _name, TileMapLayerType::Image)
    {
    }

    void setImage(SDL_TexturePtr _image) { m_image = _image; }
    const SDL_TexturePtr getImage() const { return m_image; }

private:
    SDL_TexturePtr m_image;
};


class TileMapObjectLayer : public TileMapLayer
{
public:
    TileMapObjectLayer(const ObjectHeap & _heap, uint32_t _id, const std::string & _name) :
        TileMapLayer(_id, _name, TileMapLayerType::Object),
        mr_heap(_heap)
    {
    }

    void forEachObject(std::function<void(const TileMapObject &)> _cb) const
    {
        mr_heap.forEachObject(getId(), _cb);
    }

private:
    const ObjectHeap & mr_heap;
};


class TileMapTileLayer : public TileMapLayer
{
public:
    TileMapTileLayer(const TileHeap & _tile_heap,
                     uint32_t _id,
                     const std::string & _name,
                     uint32_t _tile_width,
                     uint32_t _tile_height,
                     int32_t _x,
                     int32_t _y,
                     uint32_t _width,
                     uint32_t _height);

    int32_t getX() const
    {
        return m_x;
    }

    int32_t getY() const
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

    bool eraseTile(int32_t _x, int32_t _y)
    {
        return m_tiles.eraseTile(_x, _y);
    }

    void setTile(int32_t _x, int32_t _y, uint32_t _gid)
    {
        m_tiles.setTile(_x, _y, _gid);
    }

    const Tile * getTileAtPoint(int32_t _x, int32_t _y) const
    {
        // TODO: offsets
        return m_tile_width && m_tile_height ? getTile(_x / m_tile_width, _y / m_tile_height) : nullptr;
    }

    const Tile * getTile(int32_t _x, int32_t _y) const
    {
        uint32_t gid = m_tiles.getTile(_x, _y);
        return gid == __Private::INVALID_TILE_GID ? nullptr : mr_tile_heap.getTile(gid);
    }

private:
    const TileHeap & mr_tile_heap;
    uint32_t m_tile_width;
    uint32_t m_tile_height;
    int32_t m_x;
    int32_t m_y;
    uint32_t m_width;
    uint32_t m_height;
    __Private::TileMapTileStorage m_tiles;
};

inline TileMapTileLayer::TileMapTileLayer(const TileHeap & _tile_heap,
                                          uint32_t _id,
                                          const std::string & _name,
                                          uint32_t _tile_width,
                                          uint32_t _tile_height,
                                          int32_t _x,
                                          int32_t _y,
                                          uint32_t _width,
                                          uint32_t _height) :
    TileMapLayer(_id, _name, TileMapLayerType::Tile),
    mr_tile_heap(_tile_heap),
    m_tile_width(_tile_width),
    m_tile_height(_tile_height),
    m_x(_x),
    m_y(_y),
    m_width(_width),
    m_height(_height)
{
}


class TileMapGroupLayer : public TileMapLayer, public TileMapLayerContainer
{
public:
    TileMapGroupLayer(const uint32_t _id,
                      const std::string & _name,
                      const TileHeap & _tile_heap,
                      const ObjectHeap & _object_heap) :
        TileMapLayer(_id, _name, TileMapLayerType::Group),
        TileMapLayerContainer(_tile_heap, _object_heap)
    {
    }
};

} // namespace Sol2D::Tiles
