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

#include <Sol2D/Box2dDebugDraw.h>
#include <Sol2D/Utils/Observable.h>
#include <Sol2D/BodyDefinition.h>
#include <Sol2D/BodyOptions.h>
#include <Sol2D/Contact.h>
#include <Sol2D/Canvas.h>
#include <Sol2D/Workspace.h>
#include <Sol2D/Tiles/TileMap.h>
#include <box2d/types.h>
#include <boost/container/slist.hpp>
#include <filesystem>
#include <unordered_set>
#include <functional>

namespace Sol2D {

struct SceneOptions
{
    SceneOptions() :
        meters_per_pixel(default_meters_per_pixel),
        gravity{.0f, .0f}
    {
    }

    static constexpr float default_meters_per_pixel = 0.01f;

    float meters_per_pixel;
    Point gravity;
};

class Scene final : public Canvas, public Utils::Observable<ContactObserver>
{
private:
    class BodyShapeCreator;
    friend class Scene::BodyShapeCreator;

public:
    Scene(const SceneOptions & _options, const Workspace & _workspace, SDL_Renderer & _renderer);
    ~Scene() override;
    void setGravity(const Point & _vector);
    uint64_t createBody(const Point & _position, const BodyDefinition & _definition);
    void createBodiesFromMapObjects(const std::string & _class, const BodyOptions & _body_options);
    bool destroyBody(uint64_t _body_id);
    bool setFollowedBody(uint64_t _body_id);
    void resetFollowedBody();
    bool setBodyLayer(uint64_t _body_id, const std::string & _layer);
    bool setBodyShapeCurrentGraphic(
        uint64_t _body_id,
        const std::string & _shape_key,
        const std::string & _graphic_key);
    bool flipBodyShapeGraphic(
        uint64_t _body_id,
        const std::string & _shape_key,
        const std::string & _graphic_key,
        bool _flip_horizontally,
        bool _flip_vertically);
    bool loadTileMap(const std::filesystem::path & _file_path);
    const Tiles::TileMapObject * getTileMapObjectById(uint32_t _id) const;
    const Tiles::TileMapObject * getTileMapObjectByName(std::string _name) const;
    void render(const RenderState & _state) override;
    void applyForce(uint64_t _body_id, const Point & _force);
    void applyImpulse(uint64_t _body_id, const Point & _impulse);
    Point getLinearVelocity(uint64_t _body_id) const;
    void setBodyPosition(uint64_t _body_id, const Point & _position);
    std::optional<Point> getBodyPosition(uint64_t _body_id) const;
    std::optional<std::vector<Point> > findPath(
        uint64_t _body_id,
        const Point & _destination,
        bool _allow_diagonal_steps,
        bool _avoid_sensors) const;

private:
    float physicalToGraphical(float _value);
    float graphicalToPhysical(float _value);
    void deinitializeTileMap();
    void destroyBody(b2BodyId _body_id);
    static b2BodyType mapBodyType(BodyType _type);
    void executeDefers();
    static bool box2dPreSolveContact(
        b2ShapeId _shape_id_a,
        b2ShapeId _shape_id_b,
        b2Manifold * _manifold,
        void * _context);
    void handleBox2dContactEvents();
    static bool tryGetContactSide(b2ShapeId _shape_id, ContactSide & _contact_side);
    void syncWorldWithFollowedBody();
    void drawLayersAndBodies(
        const Tiles::TileMapLayerContainer & _container,
        std::unordered_set<uint64_t> & _bodies_to_render,
        std::chrono::milliseconds _time_passed);
    b2BodyId findBody(uint64_t _body_id) const;
    void drawBody(b2BodyId _body_id, std::chrono::milliseconds _time_passed);
    void drawObjectLayer(const Tiles::TileMapObjectLayer & _layer);
    void drawPolyXObject(const Tiles::TileMapPolyX & _poly, bool _close);
    void drawCircle(const Tiles::TileMapCircle & _circle);
    void drawTileLayer(const Tiles::TileMapTileLayer & _layer);
    void drawImageLayer(const Tiles::TileMapImageLayer & _layer);
    Point toAbsoluteCoords(float _world_x, float _world_y) const;

private:
    const Workspace & mr_workspace;
    SDL_Renderer & mr_renderer;
    Point m_world_offset;
    b2WorldId m_b2_world_id;
    float m_meters_per_pixel;
    std::unordered_map<uint64_t, b2BodyId> m_bodies;
    b2BodyId m_followed_body_id;
    std::unique_ptr<Tiles::TileHeap> m_tile_heap_ptr;
    std::unique_ptr<Tiles::ObjectHeap> m_object_heap_ptr;
    std::unique_ptr<Tiles::TileMap> m_tile_map_ptr;
    boost::container::slist<std::function<void()>> m_defers;
    Box2dDebugDraw * mp_box2d_debug_draw;
};

inline float Scene::physicalToGraphical(float _value)
{
    return _value / m_meters_per_pixel;
}

inline float Scene::graphicalToPhysical(float _value)
{
    return _value * m_meters_per_pixel;
}

inline Point Scene::toAbsoluteCoords(float _world_x, float _world_y) const
{
    return { .x = _world_x - m_world_offset.x, .y = _world_y - m_world_offset.y };
}

inline const Tiles::TileMapObject * Scene::getTileMapObjectById(uint32_t _id) const
{
    return m_object_heap_ptr->findObject<Tiles::TileMapObject>(_id);
}

inline const Tiles::TileMapObject * Scene::getTileMapObjectByName(std::string _name) const
{
    return m_object_heap_ptr->findObject<Tiles::TileMapObject>(_name);
}

} // namespace Sol2D
