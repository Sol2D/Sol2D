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

#include <Sol2D/Workspace.h>
#include <Sol2D/BodyPrototype.h>
#include <Sol2D/Tiles/TileMap.h>
#include <Sol2D/SDL.h>
#include <filesystem>

namespace Sol2D {

class Level final
{
    DISABLE_COPY_AND_MOVE(Level)

public:
    Level(SDL_Renderer & _renderer, const Workspace & _workspace);
    ~Level();
    void deinitialize();
    bool loadFromTmx(const std::filesystem::path & _tmx_file);
    void render(const SDL_FRect & _viewport);
    void move(int _delta_x, int _delta_y);
    Body & createBody(const BodyPrototype & _proto);
    void setFollowedBody(const Body * _body);
    const Tiles::TileMapObject * getObjectById(uint32_t _id) const;
    const Tiles::TileMapObject * getObjectByName(const std::string & _name) const;
    Box2D & getBox2D() const;
    void createBox2dStaticBodies(const std::string _class, Box2D::StaticObjectKind _kind);

private:
    void initializeMap();
    void drawLayersAndBodies(const Tiles::TileMapLayerContainer & _container,
                             const SDL_FRect & _viewport,
                             std::vector<bool> & _rendered_bodies);
    void drawObjectLayer(const Tiles::TileMapObjectLayer & _layer);
    void drawPolyXObject(const Tiles::TileMapPolyX & _poly, bool _close);
    void drawCircle(const Tiles::TileMapCircle & _circle);
    void drawTileLayer(const SDL_FRect & _viewport , const Tiles::TileMapTileLayer & _layer);
    void drawImageLayer(const Tiles::TileMapImageLayer & _layer);
    void drawBody(const Body & _body) const;
    void drawBox2D();
    void syncWorldWithFollowedBody(const SDL_FRect & _viewport);
    SDL_FPoint toAbsoluteCoords(float _world_x, float _world_y) const;

private:
    std::unique_ptr<Tiles::TileHeap> m_tile_heap_ptr;
    std::unique_ptr<Tiles::ObjectHeap> m_object_heap_ptr;
    std::unique_ptr<Tiles::TileMap> m_tile_map_ptr;
    SDL_Renderer * mp_renderer;
    const Workspace & mr_workspace;
    SDL_FPoint m_world_offset;
    std::vector<Body *> m_bodies;
    const Body * mp_followed_body;
    Box2D * mp_box2d;
};

inline void Level::setFollowedBody(const Body * _body)
{
    mp_followed_body = _body;
}

inline const Tiles::TileMapObject * Level::getObjectById(uint32_t _id) const
{
    return m_object_heap_ptr->findBasicObject(_id);
}

inline const Tiles::TileMapObject * Level::getObjectByName(const std::string & _name) const
{
    return m_object_heap_ptr->findBasicObject(_name);
}

inline Box2D & Level::getBox2D() const
{
    return *mp_box2d;
}

} // namespace Sol2D
