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

#include <Sol2D/World/Body.h>
#include <Sol2D/World/BodyDefinition.h>
#include <Sol2D/World/Joint.h>
#include <Sol2D/World/JointDefinition.h>
#include <Sol2D/World/BodyOptions.h>
#include <Sol2D/World/Contact.h>
#include <Sol2D/World/ActionQueue.h>
#include <Sol2D/World/Box2dDebugDraw.h>
#include <Sol2D/Tiles/TileMap.h>
#include <Sol2D/Utils/Observable.h>
#include <Sol2D/Utils/PreHashedMap.h>
#include <Sol2D/Canvas.h>
#include <Sol2D/Workspace.h>
#include <filesystem>
#include <unordered_set>

namespace Sol2D::World {

struct SceneOptions
{
    SceneOptions() :
        meters_per_pixel(default_meters_per_pixel),
        gravity {.0f, .0f}
    {
    }

    static constexpr float default_meters_per_pixel = 0.01f;

    float meters_per_pixel;
    SDL_FPoint gravity;
};

class StepObserver
{
public:
    virtual ~StepObserver()
    {
    }
    virtual void onStepComplete(const StepState & _state) = 0;
};

class Scene final : public Canvas, public Utils::Observable<ContactObserver>, public Utils::Observable<StepObserver>
{
private:
    class BodyShapeCreator;
    friend class Scene::BodyShapeCreator;

public:
    using Utils::Observable<ContactObserver>::addObserver;
    using Utils::Observable<ContactObserver>::removeObserver;
    using Utils::Observable<StepObserver>::addObserver;
    using Utils::Observable<StepObserver>::removeObserver;

public:
    Scene(const SceneOptions & _options, const Workspace & _workspace, Renderer & _renderer);
    ~Scene() override;
    void setGravity(const SDL_FPoint & _vector);
    uint64_t createBody(const SDL_FPoint & _position, const BodyDefinition & _definition);
    void createBodiesFromMapObjects(const std::string & _class, const BodyOptions & _body_options);
    Body * getBody(uint64_t _body_id);
    bool destroyBody(uint64_t _body_id);
    bool setFollowedBody(uint64_t _body_id);
    void resetFollowedBody();
    bool setBodyLayer(uint64_t _body_id, const std::string & _layer);
    GraphicsPack * getBodyShapeGraphicsPack(
        uint64_t _body_id,
        const Utils::PreHashedKey<std::string> & _shape_key,
        const Utils::PreHashedKey<std::string> & _graphics_key
    );
    bool setBodyShapeCurrentGraphics(
        uint64_t _body_id,
        const Utils::PreHashedKey<std::string> & _shape_key,
        const Utils::PreHashedKey<std::string> & _graphic_key
    );
    GraphicsPack * getBodyShapeCurrentGraphics(uint64_t _body_id, const Utils::PreHashedKey<std::string> & _shape_key);
    bool flipBodyShapeGraphics(
        uint64_t _body_id,
        const Utils::PreHashedKey<std::string> & _shape_key,
        const Utils::PreHashedKey<std::string> & _graphic_key,
        bool _flip_horizontally,
        bool _flip_vertically
    );
    uint64_t createJoint(const DistanceJointDefenition & _definition);
    uint64_t createJoint(const MotorJointDefinition & _definition);
    uint64_t createJoint(const MouseJointDefinition & _definition);
    uint64_t createJoint(const PrismaticJointDefinition & _definition);
    uint64_t createJoint(const WeldJointDefinition & _definition);
    uint64_t createJoint(const WheelJointDefinition & _definition);
    std::optional<DistanceJoint> getDistanceJoint(uint64_t _id) const;
    std::optional<MotorJoint> getMotorJoint(uint64_t _id) const;
    std::optional<MouseJoint> getMouseJoint(uint64_t _id) const;
    std::optional<PrismaticJoint> getPrismaticJoint(uint64_t _id) const;
    std::optional<WeldJoint> getWeldJoint(uint64_t _id) const;
    std::optional<WheelJoint> getWheelJoint(uint64_t _id) const;
    bool destroyJoint(uint64_t _joint_id);
    bool loadTileMap(const std::filesystem::path & _file_path);
    const Tiles::TileMapObject * getTileMapObjectById(uint32_t _id) const;
    const Tiles::TileMapObject * getTileMapObjectByName(const std::string & _name) const;
    boost::container::slist<const Tiles::TileMapObject *> getTileMapObjectsByClass(const std::string & _class) const;
    void step(const StepState & _state) override;
    bool doesBodyExist(uint64_t _body_id) const;
    bool doesBodyShapeExist(uint64_t _body_id, const Utils::PreHashedKey<std::string> & _shape_key) const;
    std::optional<std::vector<SDL_FPoint>> findPath(
        uint64_t _body_id, const SDL_FPoint & _destination, bool _allow_diagonal_steps, bool _avoid_sensors
    ) const;

private:
    float physicalToGraphical(float _value);
    float graphicalToPhysical(float _value);
    void deinitializeTileMap();
    static b2BodyType mapBodyType(BodyType _type);
    static bool box2dPreSolveContact(
        b2ShapeId _shape_id_a, b2ShapeId _shape_id_b, b2Manifold * _manifold, void * _context
    );
    void handleBox2dContactEvents();
    static bool tryGetContactSide(b2ShapeId _shape_id, ContactSide & _contact_side);
    void syncWorldWithFollowedBody();
    void drawLayersAndBodies(
        const Tiles::TileMapLayerContainer & _container,
        std::unordered_set<uint64_t> & _bodies_to_render,
        std::chrono::milliseconds _delta_time
    );
    b2BodyId findBox2dBody(uint64_t _body_id) const;
    b2JointId findJoint(uint64_t _joint_id) const;
    void drawBody(b2BodyId _body_id, std::chrono::milliseconds _delta_time);
    void drawObjectLayer(const Tiles::TileMapObjectLayer & _layer);
    void drawPolyXObject(const Tiles::TileMapPolyX & _poly, bool _close);
    void drawCircle(const Tiles::TileMapCircle & _circle);
    void drawTileLayer(const Tiles::TileMapTileLayer & _layer);
    SDL_FRect calculateViewport(const Tiles::TileMapTileLayer & _layer) const;
    void drawImageLayer(const Tiles::TileMapImageLayer & _layer);
    SDL_FPoint toAbsoluteCoords(float _world_x, float _world_y) const;

private:
    const Workspace & m_workspace;
    Renderer & m_renderer;
    SDL_FPoint m_world_offset;
    b2WorldId m_b2_world_id;
    float m_meters_per_pixel;
    std::unordered_map<uint64_t, b2BodyId> m_bodies;
    std::unordered_map<uint64_t, b2JointId> m_joints;
    b2BodyId m_followed_body_id;
    std::unique_ptr<Tiles::TileHeap> m_tile_heap_ptr;
    std::unique_ptr<Tiles::ObjectHeap> m_object_heap_ptr;
    std::unique_ptr<Tiles::TileMap> m_tile_map_ptr;
    ActionAccumulator m_defers;
    Box2dDebugDraw * m_box2d_debug_draw;
};

inline float Scene::physicalToGraphical(float _value)
{
    return _value / m_meters_per_pixel;
}

inline float Scene::graphicalToPhysical(float _value)
{
    return _value * m_meters_per_pixel;
}

inline SDL_FPoint Scene::toAbsoluteCoords(float _world_x, float _world_y) const
{
    return {.x = _world_x - m_world_offset.x, .y = _world_y - m_world_offset.y};
}

inline const Tiles::TileMapObject * Scene::getTileMapObjectById(uint32_t _id) const
{
    return m_object_heap_ptr->findBasicObject(_id);
}

inline const Tiles::TileMapObject * Scene::getTileMapObjectByName(const std::string & _name) const
{
    return m_object_heap_ptr->findBasicObject(_name);
}

inline boost::container::slist<const Tiles::TileMapObject *> Scene::getTileMapObjectsByClass(const std::string & _class
) const
{
    return m_object_heap_ptr->findBasicObjects(_class);
}

} // namespace Sol2D::World
