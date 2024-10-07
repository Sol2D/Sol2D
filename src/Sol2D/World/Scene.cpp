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

#include <Sol2D/World/Scene.h>
#include <Sol2D/World/AStar.h>
#include <Sol2D/Tiles/Tmx.h>
#include <Sol2D/Utils/Observable.h>
#include <Sol2D/SDL/SDL.h>
#include <box2d/box2d.h>
#include <unordered_set>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::SDL;
using namespace Sol2D::Tiles;
using namespace Sol2D::Utils;

namespace {

void initBodyPhysics(b2BodyDef & _b2_def, const BodyPhysicsDefinition & _physics)
{
    if(_physics.linear_damping.has_value())
        _b2_def.linearDamping = _physics.linear_damping.value();
    if(_physics.angular_damping.has_value())
        _b2_def.angularDamping = _physics.angular_damping.value();
    _b2_def.fixedRotation = _physics.fixed_rotation;
}

void initShapePhysics(b2ShapeDef & _b2_shape_def, const BodyShapePhysicsDefinition & _physics)
{
    _b2_shape_def.isSensor = _physics.is_sensor;
    _b2_shape_def.enablePreSolveEvents = _physics.is_pre_solve_enabled;
    if(_physics.density.has_value())
        _b2_shape_def.density = _physics.density.value();
    if(_physics.restitution.has_value())
        _b2_shape_def.restitution = _physics.restitution.value();
    if(_physics.friction.has_value())
        _b2_shape_def.friction = _physics.friction.value();
}

class BodyShape final
{
    S2_DISABLE_COPY_AND_MOVE(BodyShape)

public:
    BodyShape(const std::string & _key, std::optional<uint32_t> _tile_map_object_id);
    ~BodyShape();
    const std::string & getKey() const;
    const std::optional<uint32_t> getTileMapObjectId() const;
    void addGraphics(
        SDL_Renderer & _renderer,
        const PreHashedKey<std::string> & _key,
        const GraphicsPackDefinition & _definition);
    bool setCurrentGraphics(const PreHashedKey<std::string> & _key);
    GraphicsPack * getCurrentGraphics();
    GraphicsPack * getGraphics(const PreHashedKey<std::string> & _key);
    bool flipGraphics(const PreHashedKey<std::string> & _key, bool _flip_horizontally, bool _flip_vertically);

private:
    const std::string m_key;
    const std::optional<uint32_t> m_tile_map_object_id;
    PreHashedMap<std::string, GraphicsPack *> m_graphics;
    GraphicsPack * mp_current_graphic;
};

inline BodyShape::BodyShape(const std::string & _key, std::optional<uint32_t> _tile_map_object_id) :
    m_key(_key),
    m_tile_map_object_id(_tile_map_object_id),
    mp_current_graphic(nullptr)
{
}

inline BodyShape::~BodyShape()
{
    for(const auto & pair : m_graphics)
        delete pair.second;
}

inline const std::string & BodyShape::getKey() const
{
    return m_key;
}

inline const std::optional<uint32_t> BodyShape::getTileMapObjectId() const
{
    return m_tile_map_object_id;
}

inline void BodyShape::addGraphics(
    SDL_Renderer & _renderer,
    const PreHashedKey<std::string> & _key,
    const GraphicsPackDefinition & _definition)
{
    auto it = m_graphics.find(_key);
    if(it != m_graphics.end())
        delete it->second;
    m_graphics[_key] = new GraphicsPack(_renderer, _definition);
}

inline bool BodyShape::setCurrentGraphics(const PreHashedKey<std::string> & _key)
{
    GraphicsPack * graphics = getGraphics(_key);
    if(graphics)
    {
        mp_current_graphic = graphics;
        return true;
    }
    return false;
}

inline GraphicsPack * BodyShape::getGraphics(const PreHashedKey<std::string> & _key)
{
    auto it = m_graphics.find(_key);
    return it == m_graphics.end() ? nullptr : it->second;
}

inline GraphicsPack * BodyShape::getCurrentGraphics()
{
    return mp_current_graphic;
}

inline bool BodyShape::flipGraphics(
    const PreHashedKey<std::string> & _key,
    bool _flip_horizontally,
    bool _flip_vertically)
{
    auto it = m_graphics.find(_key);
    if(it == m_graphics.end())
        return false;
    it->second->setFilippedHorizontally(_flip_horizontally);
    it->second->setFilippedVertically(_flip_vertically);
    return true;
}

class Body final
{
    S2_DISABLE_COPY_AND_MOVE(Body)

public:
    Body();
    ~Body();
    uint64_t getId() const;
    void setLayer(const std::string & _layer);
    const std::optional<std::string> & getLayer() const;
    BodyShape & createShape(
        const std::string & _key,
        std::optional<uint32_t> _tile_map_object_id = std::nullopt);
    BodyShape * findShape(const PreHashedKey<std::string> & _key);

private:
    static uint64_t s_next_id;
    uint64_t m_id;
    std::optional<std::string> m_layer;
    PreHashedMap<std::string, BodyShape *> m_shapes;
};

uint64_t Body::s_next_id = 1;

inline Body::Body() :
    m_id(s_next_id++)
{
}

inline Body::~Body()
{
    for(const auto & shape : m_shapes)
        delete shape.second;
}

inline uint64_t Body::getId() const
{
    return m_id;
}

inline void Body::setLayer(const std::string & _layer)
{
    m_layer = _layer;
}

inline const std::optional<std::string> & Body::getLayer() const
{
    return m_layer;
}

inline BodyShape & Body::createShape(
    const std::string & _key,
    std::optional<uint32_t> _tile_map_object_id /*= std::nullopt*/)
{
    BodyShape * shape = new BodyShape(_key, _tile_map_object_id);
    m_shapes.insert(std::make_pair(_key, shape));
    return *shape;
}

inline BodyShape * Body::findShape(const PreHashedKey<std::string> & _key)
{
    auto it = m_shapes.find(_key);
    return it == m_shapes.end() ? nullptr : it->second;
}

inline Body * getUserData(b2BodyId _body_id)
{
    return static_cast<Body *>(b2Body_GetUserData(_body_id));
}

inline BodyShape * getUserData(b2ShapeId _shape_id)
{
    return static_cast<BodyShape *>(b2Shape_GetUserData(_shape_id));
}

} // namespace

class Scene::BodyShapeCreator
{
public:
    explicit BodyShapeCreator(Scene & _scene, Body & _body, const b2BodyId & _b2_body_id, const std::string & _key);
    void operator ()(const BodyPolygonDefinition & _polygon);
    void operator ()(const BodyRectDefinition & _rect);
    void operator ()(const BodyCircleDefinition & _circle);

private:
    template<BodyShapeType shape_type>
    b2ShapeDef createBox2dShapeDef(const BodyBasicShapeDefinition<shape_type> & _def) const;
    template<BodyShapeType shape_type>
    BodyShape & createShape(const BodyBasicShapeDefinition<shape_type> & _def) const;

private:
    Scene & mr_scene;
    Body & mr_body;
    const b2BodyId & mr_b2_body_id;
    const std::string & mr_key;
};

Scene::BodyShapeCreator::BodyShapeCreator(
    Scene & _scene,
    Body & _body,
    const b2BodyId & _b2_body_id,
    const std::string & _key
) :
    mr_scene(_scene),
    mr_body(_body),
    mr_b2_body_id(_b2_body_id),
    mr_key(_key)
{
}

void Scene::BodyShapeCreator::operator ()(const BodyPolygonDefinition & _polygon)
{
    if(_polygon.points.size() < 3 || _polygon.points.size() > b2_maxPolygonVertices)
        return; // TODO: log
    std::vector<b2Vec2> shape_points(_polygon.points.size());
    for(size_t i = 0; i < _polygon.points.size(); ++i)
    {
        shape_points[i].x = mr_scene.graphicalToPhysical(_polygon.points[i].x);
        shape_points[i].y = mr_scene.graphicalToPhysical(_polygon.points[i].y);
    }
    b2Hull b2_hull = b2ComputeHull(shape_points.data(), shape_points.size());
    b2Polygon b2_polygon = b2MakePolygon(&b2_hull, .0f);
    b2ShapeDef b2_shape_def = createBox2dShapeDef(_polygon);
    b2ShapeId b2_shape_id = b2CreatePolygonShape(mr_b2_body_id, &b2_shape_def, &b2_polygon);
    BodyShape & body_shape = createShape(_polygon);
    b2Shape_SetUserData(b2_shape_id, &body_shape);
}

template<BodyShapeType shape_type>
inline b2ShapeDef Scene::BodyShapeCreator::createBox2dShapeDef(const BodyBasicShapeDefinition<shape_type> & _def) const
{
    b2ShapeDef b2_shape_def = b2DefaultShapeDef();
    initShapePhysics(b2_shape_def, _def.physics);
    return b2_shape_def;
}

template<BodyShapeType shape_type>
BodyShape & Scene::BodyShapeCreator::createShape(const BodyBasicShapeDefinition<shape_type> & _def) const
{
    BodyShape & body_shape = mr_body.createShape(mr_key);
    for(const auto & graphics_kv : _def.graphics)
    {
        body_shape.addGraphics(
            mr_scene.mr_renderer,
            makePreHashedKey(graphics_kv.first),
            graphics_kv.second);
    }
    return body_shape;
}

void Scene::BodyShapeCreator::operator ()(const BodyRectDefinition & _rect)
{
    const float half_w = mr_scene.graphicalToPhysical(_rect.w) / 2.0f;
    const float half_h = mr_scene.graphicalToPhysical(_rect.h) / 2.0f;
    b2Polygon b2_polygon = b2MakeOffsetBox(
        half_w,
        half_h,
        {
            .x = (_rect.x ? mr_scene.graphicalToPhysical(_rect.x) : .0f) + half_w,
            .y = (_rect.y ?  mr_scene.graphicalToPhysical(_rect.y) : .0f) + half_h
        },
        b2Rot_identity);
    b2ShapeDef b2_shape_def = createBox2dShapeDef(_rect);
    b2ShapeId b2_shape_id = b2CreatePolygonShape(mr_b2_body_id, &b2_shape_def, &b2_polygon);
    BodyShape & body_shape = createShape(_rect);
    b2Shape_SetUserData(b2_shape_id, &body_shape);
}

void Scene::BodyShapeCreator::operator ()(const BodyCircleDefinition & _circle)
{
    b2Circle b2_circle
    {
        .center =
        {
            .x = mr_scene.graphicalToPhysical(_circle.center.x),
            .y = mr_scene.graphicalToPhysical(_circle.center.y)
        },
        .radius = mr_scene.graphicalToPhysical(_circle.radius)
    };
    if(b2_circle.radius <= .0f)
        return; // TODO: log
    b2ShapeDef b2_shape_def = createBox2dShapeDef(_circle);
    b2ShapeId b2_shape_id = b2CreateCircleShape(mr_b2_body_id, &b2_shape_def, &b2_circle);
    BodyShape & body_shape = createShape(_circle);
    b2Shape_SetUserData(b2_shape_id, &body_shape);
}

Scene::Scene(const SceneOptions & _options, const Workspace & _workspace, SDL_Renderer & _renderer) :
    mr_workspace(_workspace),
    mr_renderer(_renderer),
    m_world_offset{.0f, .0f},
    m_meters_per_pixel(_options.meters_per_pixel),
    m_followed_body_id(b2_nullBodyId),
    mp_box2d_debug_draw(nullptr)
{
    if(m_meters_per_pixel <= .0f)
        m_meters_per_pixel = SceneOptions::default_meters_per_pixel;
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = _options.gravity;
    m_b2_world_id = b2CreateWorld(&world_def);
    b2World_SetPreSolveCallback(m_b2_world_id, &Scene::box2dPreSolveContact, this);
    if(_workspace.isDebugRenderingEnabled())
    {
        mp_box2d_debug_draw = new Box2dDebugDraw(
            mr_renderer,
            m_b2_world_id,
            [this](float __x, float __y)
            {
                return toAbsoluteCoords(physicalToGraphical(__x), physicalToGraphical(__y));
            },
            [this](float __len)
            {
                return physicalToGraphical(__len);
            }
        );
    }
}

Scene::~Scene()
{
    deinitializeTileMap();
    b2DestroyWorld(m_b2_world_id);
    delete mp_box2d_debug_draw;
}

void Scene::deinitializeTileMap()
{
    for(auto & pair : m_bodies)
        destroyBody(pair.second);
    m_bodies.clear();
    m_tile_heap_ptr.reset();
    m_object_heap_ptr.reset();
    m_tile_map_ptr.reset();
    m_followed_body_id = b2_nullBodyId;
}

void Scene::setGravity(const Point & _vector)
{
    m_defers.push_front([this, _vector]() {
        b2World_SetGravity(m_b2_world_id, *_vector.toBox2DPtr()); // TODO: scale factor?
    });
}

uint64_t Scene::createBody(const Point & _position, const BodyDefinition & _definition)
{
    Body * body = new Body;
    b2BodyDef b2_body_def = b2DefaultBodyDef();
    b2_body_def.type = mapBodyType(_definition.type);
    b2_body_def.position = { .x = _position.x, .y = _position.y };
    initBodyPhysics(b2_body_def, _definition.physics);
    b2BodyId b2_body_id = b2CreateBody(m_b2_world_id, &b2_body_def);
    b2Body_SetUserData(b2_body_id, body);
    m_bodies.insert(std::make_pair(body->getId(), b2_body_id));
    for(const auto & shape_kv : _definition.shapes)
    {
        BodyShapeCreator visitor(*this, *body, b2_body_id, shape_kv.first);
        std::visit(visitor, shape_kv.second);
    }
    return body->getId();
}

void Scene::createBodiesFromMapObjects(const std::string & _class, const BodyOptions & _body_options)
{
    b2BodyType body_type = mapBodyType(_body_options.type);

    m_object_heap_ptr->forEachObject([&](const TileMapObject & __map_object) {
        if(__map_object.getClass() != _class) return;
        Body * body = new Body;
        b2BodyDef b2_body_def = b2DefaultBodyDef();
        b2_body_def.type = body_type;
        initBodyPhysics(b2_body_def, _body_options.body_physics);
        b2_body_def.position =
        {
            .x = graphicalToPhysical(__map_object.getX()),
            .y = graphicalToPhysical(__map_object.getY())
        };
        b2_body_def.userData = body;
        b2BodyId b2_body_id = b2CreateBody(m_b2_world_id, &b2_body_def);
        m_bodies.insert(std::make_pair(body->getId(), b2_body_id));
        b2ShapeDef b2_shape_def = b2DefaultShapeDef();
        initShapePhysics(b2_shape_def, _body_options.shape_physics);

        switch(__map_object.getObjectType())
        {
        case TileMapObjectType::Polygon:
        {
            const TileMapPolygon * polygon = static_cast<const TileMapPolygon *>(&__map_object);
            const std::vector<Point> & points = polygon->getPoints();
            if(points.size() < 3 || points.size() > b2_maxPolygonVertices)
                break;
            std::vector<b2Vec2> shape_points(points.size());
            for(size_t i = 0; i < points.size(); ++i)
            {
                shape_points[i].x = graphicalToPhysical(points[i].x);
                shape_points[i].y = graphicalToPhysical(points[i].y);
            }
            b2Hull b2_hull = b2ComputeHull(shape_points.data(), shape_points.size());
            b2Polygon b2_polygon = b2MakePolygon(&b2_hull, .0f);
            b2ShapeId b2_shape_id = b2CreatePolygonShape(b2_body_id, &b2_shape_def, &b2_polygon);
            BodyShape * body_shape = &body->createShape(_body_options.shape_key.value_or(_class), polygon->getId());
            b2Shape_SetUserData(b2_shape_id, body_shape);
        }
        break;
        case TileMapObjectType::Circle:
        {
            const TileMapCircle * circle = static_cast<const TileMapCircle *>(&__map_object);
            const float radius = graphicalToPhysical(circle->getRadius());
            if(radius <= .0f)
                break;
            b2Circle b2_circle
            {
                .center = { .x = .0f, .y = .0f },
                .radius = radius
            };
            b2ShapeId b2_shape_id = b2CreateCircleShape(b2_body_id, &b2_shape_def, &b2_circle);
            BodyShape * body_shape = &body->createShape(_body_options.shape_key.value_or(_class), circle->getId());
            b2Shape_SetUserData(b2_shape_id, body_shape);
        }
        break;
        default: return;
        }
    });
}

bool Scene::destroyBody(uint64_t _body_id)
{
    b2BodyId b2_body_id = findBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return false;
    if(B2_ID_EQUALS(m_followed_body_id, b2_body_id))
        m_followed_body_id = b2_nullBodyId;
    m_bodies.erase(_body_id);
    destroyBody(b2_body_id);
    return true;
}

void Scene::destroyBody(b2BodyId _body_id)
{
    Body * body = getUserData(_body_id);
    b2DestroyBody(_body_id);
    delete body;
}

b2BodyId Scene::findBody(uint64_t _body_id) const
{
    auto it = m_bodies.find(_body_id);
    return it == m_bodies.end() ? b2_nullBodyId : it->second;
}

b2BodyType Scene::mapBodyType(BodyType _type)
{
    switch(_type)
    {
    case BodyType::Dynamic:
        return b2_dynamicBody;
    case BodyType::Kinematic:
        return b2_kinematicBody;
    default:
        return b2_staticBody;
    }
}

bool Scene::setFollowedBody(uint64_t _body_id)
{
    m_followed_body_id = findBody(_body_id);
    return B2_IS_NON_NULL(m_followed_body_id);
}

void Scene::resetFollowedBody()
{
    m_followed_body_id = b2_nullBodyId;
}

bool Scene::setBodyLayer(uint64_t _body_id, const std::string & _layer)
{
    b2BodyId b2_body_id = findBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return false;
    getUserData(b2_body_id)->setLayer(_layer);
    return true;
}

GraphicsPack * Scene::getBodyShapeGraphicsPack(
    uint64_t _body_id,
    const PreHashedKey<std::string> & _shape_key,
    const PreHashedKey<std::string> & _graphics_key)
{
    b2BodyId b2_body_id = findBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return nullptr;
    BodyShape * shape = getUserData(b2_body_id)->findShape(_shape_key);
    if(shape == nullptr)
        return nullptr;
    return shape->getGraphics(_graphics_key);
}

bool Scene::setBodyShapeCurrentGraphics(
    uint64_t _body_id,
    const PreHashedKey<std::string> & _shape_key,
    const PreHashedKey<std::string> & _graphic_key)
{
    b2BodyId b2_body_id = findBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return false;
    BodyShape * shape = getUserData(b2_body_id)->findShape(_shape_key);
    if(shape == nullptr)
        return false;
    return shape->setCurrentGraphics(_graphic_key);
}

bool Scene::flipBodyShapeGraphics(
    uint64_t _body_id,
    const PreHashedKey<std::string> & _shape_key,
    const PreHashedKey<std::string> & _graphic_key,
    bool _flip_horizontally,
    bool _flip_vertically)
{
    b2BodyId b2_body_id = findBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return false;
    BodyShape * shape = getUserData(b2_body_id)->findShape(_shape_key);
    if(shape == nullptr)
        return false;
    return shape->flipGraphics(_graphic_key, _flip_horizontally, _flip_vertically);
}

bool Scene::loadTileMap(const std::filesystem::path & _file_path)
{
    deinitializeTileMap();
    m_tile_heap_ptr.reset();
    m_tile_map_ptr.reset();
    m_object_heap_ptr.reset();
    Tmx tmx = loadTmx(mr_renderer, mr_workspace, _file_path); // TODO: handle exceptions
    m_tile_heap_ptr = std::move(tmx.tile_heap);
    m_tile_map_ptr = std::move(tmx.tile_map);
    m_object_heap_ptr = std::move(tmx.object_heap);
    return m_tile_map_ptr != nullptr; // TODO: only exceptions
}

void Scene::render(const RenderState & _state)
{
    if(!m_tile_map_ptr)
    {
        return;
    }
    executeDefers();
    b2World_Step(m_b2_world_id, _state.time_passed.count() / 1000.0f, 4); // TODO: stable rate (1.0f / 60.0f), all from user settings
    handleBox2dContactEvents();
    syncWorldWithFollowedBody();
    const Color & bg_color = m_tile_map_ptr->getBackgroundColor();
    SDL_SetRenderDrawColor(&mr_renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderClear(&mr_renderer);

    std::unordered_set<uint64_t> bodies_to_render(m_bodies.size());
    for(const auto & pair : m_bodies)
        bodies_to_render.insert(pair.first);
    drawLayersAndBodies(*m_tile_map_ptr, bodies_to_render, _state.time_passed);
    for(const uint64_t body_id : bodies_to_render)
        drawBody(m_bodies[body_id], _state.time_passed);

    if(mp_box2d_debug_draw)
        mp_box2d_debug_draw->draw();
}

void Scene::executeDefers()
{
    if(!m_defers.empty())
    {
        for(const auto & action : m_defers)
            action();
        m_defers.clear();
    }
}

bool Scene::box2dPreSolveContact(b2ShapeId _shape_id_a, b2ShapeId _shape_id_b, b2Manifold * _manifold, void * _context)
{
    Scene * self = static_cast<Scene *>(_context);
    bool result = true;
    PreSolveContact contact;
    if(!tryGetContactSide(_shape_id_a, contact.side_a) || !tryGetContactSide(_shape_id_b, contact.side_b))
        return true;
    contact.manifold = _manifold;
    self->forEachObserver([&result, &contact](ContactObserver & __observer) {
        if(!__observer.preSolveContact(contact))
            result = false;
        return result;
    });
    return result;
}

void Scene::handleBox2dContactEvents()
{
    {
        Contact contact;
        b2ContactEvents contact_events = b2World_GetContactEvents(m_b2_world_id);
        for(int i = 0; i < contact_events.beginCount; ++i)
        {
            const b2ContactBeginTouchEvent & event = contact_events.beginEvents[i];
            if(tryGetContactSide(event.shapeIdA, contact.side_a) && tryGetContactSide(event.shapeIdB, contact.side_b))
                callObservers(&ContactObserver::beginContact, contact);
        }
        for(int i = 0; i < contact_events.endCount; ++i)
        {
            const b2ContactEndTouchEvent & event = contact_events.endEvents[i];
            if(tryGetContactSide(event.shapeIdA, contact.side_a) && tryGetContactSide(event.shapeIdB, contact.side_b))
                callObservers(&ContactObserver::endContact, contact);
        }
    }

    {
        SensorContact contact;
        b2SensorEvents sensor_events = b2World_GetSensorEvents(m_b2_world_id);
        for(int i = 0; i < sensor_events.beginCount; ++i)
        {
            const b2SensorBeginTouchEvent & event = sensor_events.beginEvents[i];
            if(tryGetContactSide(event.sensorShapeId, contact.sensor) &&
                tryGetContactSide(event.visitorShapeId, contact.visitor))
            {
                callObservers(&ContactObserver::beginSensorContact, contact);
            }
        }
        for(int i = 0; i < sensor_events.endCount; ++i)
        {
            const b2SensorEndTouchEvent & event = sensor_events.endEvents[i];
            if(tryGetContactSide(event.sensorShapeId, contact.sensor) &&
                tryGetContactSide(event.visitorShapeId, contact.visitor))
            {
                callObservers(&ContactObserver::endSensorContact, contact);
            }
        }
    }
}

bool Scene::tryGetContactSide(b2ShapeId _shape_id, ContactSide & _contact_side)
{
    b2BodyId b2_body_id = b2Shape_GetBody(_shape_id);
    const BodyShape * shape = getUserData(_shape_id);
    const Body * body = getUserData(b2_body_id);
    if(shape && body)
    {
        _contact_side.body_id = body->getId();
        _contact_side.shape_key = shape->getKey();
        _contact_side.tile_map_object_id = shape->getTileMapObjectId();
        return true;
    }
    return false;
}

void Scene::syncWorldWithFollowedBody()
{
    if(B2_IS_NULL(m_followed_body_id))
    {
        return;
    }
    int output_width, output_height;
    SDL_GetCurrentRenderOutputSize(&mr_renderer, &output_width, &output_height);
    b2Vec2 followed_body_position = b2Body_GetPosition(m_followed_body_id);
    m_world_offset.x = physicalToGraphical(followed_body_position.x) - static_cast<float>(output_width) / 2;
    m_world_offset.y = physicalToGraphical(followed_body_position.y) - static_cast<float>(output_height) / 2;
    const int32_t map_x = m_tile_map_ptr->getX() * m_tile_map_ptr->getTileWidth();
    const int32_t map_y = m_tile_map_ptr->getY() * m_tile_map_ptr->getTileHeight();
    if(m_world_offset.x < map_x)
    {
        m_world_offset.x = map_x;
    }
    else
    {
        int32_t max_offset_x = m_tile_map_ptr->getWidth() * m_tile_map_ptr->getTileWidth() - output_width;
        if(m_world_offset.x > max_offset_x)
            m_world_offset.x = max_offset_x;
    }
    if(m_world_offset.y < map_y)
    {
        m_world_offset.y = map_y;
    }
    else
    {
        int32_t max_offset_y = m_tile_map_ptr->getHeight() * m_tile_map_ptr->getTileHeight() - output_height;
        if(m_world_offset.y > max_offset_y)
            m_world_offset.y = max_offset_y;
    }
}

void Scene::drawBody(b2BodyId _body_id, std::chrono::milliseconds _time_passed)
{
    Point body_position;
    {
        b2Vec2 pos = b2Body_GetPosition(_body_id);
        body_position = toAbsoluteCoords(physicalToGraphical(pos.x), physicalToGraphical(pos.y));
    }
    int shape_count = b2Body_GetShapeCount(_body_id);
    std::vector<b2ShapeId> shapes(shape_count);
    b2Body_GetShapes(_body_id, shapes.data(), shape_count);
    for(const b2ShapeId & shape_id : shapes)
    {
        BodyShape * shape = getUserData(shape_id);
        GraphicsPack * graphics = shape->getCurrentGraphics();
        if(graphics)
        {
            // TODO: rotation
            // TODO: How to rotate multiple shapes?
            graphics->render(body_position, .0f, _time_passed);
        }
    }
}

void Scene::drawLayersAndBodies(
    const TileMapLayerContainer & _container,
    std::unordered_set<uint64_t> & _bodies_to_render,
    std::chrono::milliseconds _time_passed)
{
    _container.forEachLayer([&_bodies_to_render, _time_passed, this](const TileMapLayer & __layer) {
        if(!__layer.isVisible()) return;
        switch(__layer.getType())
        {
        case TileMapLayerType::Tile:
            drawTileLayer(dynamic_cast<const TileMapTileLayer &>(__layer));
            break;
        case TileMapLayerType::Object:
            if(mr_workspace.isDebugRenderingEnabled())
                drawObjectLayer(dynamic_cast<const TileMapObjectLayer &>(__layer)); // TODO: _viewport
            break;
        case TileMapLayerType::Image:
            drawImageLayer(dynamic_cast<const TileMapImageLayer &>(__layer)); // TODO: _viewport
            break;
        case TileMapLayerType::Group:
        {
            const TileMapGroupLayer & group = dynamic_cast<const TileMapGroupLayer &>(__layer);
            if(group.isVisible())
                drawLayersAndBodies(group, _bodies_to_render, _time_passed);
            break;
        }}
        for(const auto & pair : m_bodies)
        {
            b2BodyId box2d_body_id = pair.second;
            const Body * body = getUserData(box2d_body_id);
            if(body->getLayer() == __layer.getName() && _bodies_to_render.erase(pair.first))
                drawBody(box2d_body_id, _time_passed);
        }
    });
}

void Scene::drawObjectLayer(const TileMapObjectLayer & _layer)
{
    SDL_SetRenderDrawColor(&mr_renderer, 255, 20, 0, 255);
    _layer.forEachObject([this](const TileMapObject & __object) {
        if(!__object.isVisible()) return;
        // TODO: if in viewport
        switch (__object.getObjectType())
        {
        case TileMapObjectType::Polygon:
            drawPolyXObject(dynamic_cast<const TileMapPolygon &>(__object), true);
            break;
        case TileMapObjectType::Polyline:
            drawPolyXObject(dynamic_cast<const TileMapPolyline &>(__object), false);
            break;
        case TileMapObjectType::Circle:
            drawCircle(dynamic_cast<const TileMapCircle &>(__object));
            break;
        default:
            // TODO: point
            break;
        }
    });
}

void Scene::drawPolyXObject(const TileMapPolyX & _poly, bool _close)
{
    const std::vector<Point> & poly_points = _poly.getPoints();
    size_t poly_points_count = poly_points.size();
    if(poly_points_count < 2) return;
    size_t total_points_count = _close ? poly_points_count + 1 : poly_points_count;
    Point base_point = toAbsoluteCoords(_poly.getX(), _poly.getY());
    std::vector<Point> points(total_points_count);
    for(size_t i = 0; i < poly_points_count; ++i)
    {
        points[i].x = base_point.x + poly_points[i].x;
        points[i].y = base_point.y + poly_points[i].y;
    }
    if(_close)
    {
        points[poly_points_count].x = points[0].x;
        points[poly_points_count].y = points[0].y;
    }
    SDL_RenderLines(&mr_renderer, points.data()->toSdlPtr(), total_points_count);
}

void Scene::drawCircle(const TileMapCircle & _circle)
{
    Point position = toAbsoluteCoords(_circle.getX(), _circle.getY());
    sdlRenderCircle(&mr_renderer, position, _circle.getRadius());
}

void Scene::drawTileLayer(const TileMapTileLayer & _layer)
{
    const Rect viewport = calculateViewport(_layer);
    const float first_col = std::floor(viewport.x / m_tile_map_ptr->getTileWidth());
    const float first_row = std::floor(viewport.y / m_tile_map_ptr->getTileHeight());
    const float last_col = std::ceil((viewport.x + viewport.w) / m_tile_map_ptr->getTileWidth());
    const float last_row = std::ceil((viewport.y + viewport.h) / m_tile_map_ptr->getTileHeight());
    const Point start_position =
    {
        .x = first_col * m_tile_map_ptr->getTileWidth() - viewport.x,
        .y = first_row * m_tile_map_ptr->getTileHeight() - viewport.y
    };
    const Size tile_map_cell_size =
    {
        .w = static_cast<float>(m_tile_map_ptr->getTileWidth()),
        .h = static_cast<float>(m_tile_map_ptr->getTileHeight())
    };

    std::unordered_set<const TileMapTileLayerCell *> extra_cells;
    SDL_FRect tile_rect;
    SDL_FRect dest_rect;

    for(
        int32_t row = static_cast<int32_t>(first_row), dest_row = 0;
        row <= static_cast<int32_t>(last_row);
        ++row, ++dest_row
    ) {
        for(
            int32_t col = static_cast<int32_t>(first_col), dest_col = 0;
            col <= static_cast<int32_t>(last_col);
            ++col, ++dest_col
        ) {
            const TileMapTileLayerCell * cell = _layer.getCell(col, row);
            if(!cell) continue;

            if(!cell->master_cells.empty())
            {
                // The large tiles are partially visible, but their main tile is outside the viewport
                extra_cells.insert(cell->master_cells.cbegin(), cell->master_cells.cend());
            }

            if(!cell->tile) continue;

            tile_rect.x = cell->tile->getSourceX();
            tile_rect.y = cell->tile->getSourceY();
            tile_rect.w = cell->tile->getWidth();
            tile_rect.h = cell->tile->getHeight();

            dest_rect.x = start_position.x + dest_col * tile_map_cell_size.w;
            dest_rect.y = start_position.y + dest_row * tile_map_cell_size.h;
            dest_rect.w = tile_rect.w;
            dest_rect.h = tile_rect.h;

            if(tile_rect.h < tile_map_cell_size.h)
                dest_rect.y += tile_map_cell_size.h - tile_rect.h;
            else if(tile_rect.h > tile_map_cell_size.h)
                dest_rect.y -= tile_rect.h - tile_map_cell_size.h;

            SDL_RenderTexture(&mr_renderer, &cell->tile->getSource(), &tile_rect, &dest_rect);
        }
    }

    for(const auto * cell : extra_cells)
    {
        if(cell->x >= first_col && cell->x < last_col && cell->y >= first_row && cell->y < last_row)
        {
            // Already drawn by loop above
            continue;
        }
        tile_rect.x = cell->tile->getSourceX();
        tile_rect.y = cell->tile->getSourceY();
        tile_rect.w = cell->tile->getWidth();
        tile_rect.h = cell->tile->getHeight();

        dest_rect.x = (_layer.getX() + cell->x) * tile_map_cell_size.w - viewport.x;
        dest_rect.y = (_layer.getY() + cell->y) * tile_map_cell_size.h - viewport.y;
        dest_rect.w = tile_rect.w;
        dest_rect.h = tile_rect.h;

        if(tile_rect.h > tile_map_cell_size.h)
            dest_rect.y -= tile_rect.h - tile_map_cell_size.h;

        SDL_RenderTexture(&mr_renderer, &cell->tile->getSource(), &tile_rect, &dest_rect);
    }
}

Rect Scene::calculateViewport(const TileMapTileLayer & _layer) const
{
    Rect viewport;
    Point layer_offset = makePoint(_layer.getOffsetX(), _layer.getOffsetY());
    Point layer_parallax = makePoint(_layer.getParallaxX(), _layer.getParallaxY());
    for(const auto * parent_layer = _layer.getParent(); parent_layer; parent_layer = parent_layer->getParent())
    {
        layer_offset.x += parent_layer->getOffsetX();
        layer_offset.y += parent_layer->getOffsetY();
        layer_parallax.x *= parent_layer->getParallaxX();
        layer_parallax.y *= parent_layer->getParallaxY();
    }
    viewport.x = m_world_offset.x * layer_parallax.x - layer_offset.x;
    viewport.y = m_world_offset.y * layer_parallax.y - layer_offset.y;
    int w, h;
    SDL_GetCurrentRenderOutputSize(&mr_renderer, &w, &h);
    viewport.w = static_cast<float>(w);
    viewport.h = static_cast<float>(h);
    return viewport;
}

void Scene::drawImageLayer(const TileMapImageLayer & _layer)
{
    std::shared_ptr<SDL_Texture> image = _layer.getImage();
    float width, height;
    SDL_GetTextureSize(image.get(), &width, &height);
    SDL_FRect dim { .0f, .0f, width, height };
    SDL_RenderTexture(&mr_renderer, image.get(), nullptr, &dim);
}

bool Scene::doesBodyExist(uint64_t _body_id) const
{
    b2BodyId b2_body_id = findBody(_body_id);
    return B2_IS_NON_NULL(b2_body_id);
}

bool Scene::doesBodyShapeExist(uint64_t _body_id, const PreHashedKey<std::string> & _shape_key) const
{
    b2BodyId b2_body_id = findBody(_body_id);
    if(B2_IS_NON_NULL(b2_body_id))
    {
        Body * body = getUserData(b2_body_id);
        return body->findShape(_shape_key) != nullptr;
    }
    return false;
}

void Scene::applyForceToBodyCenter(uint64_t _body_id, const Point & _force)
{
    m_defers.push_front([this, _body_id, _force]() {
        b2BodyId b2_body_id = findBody(_body_id);
        if(B2_IS_NON_NULL(b2_body_id))
            b2Body_ApplyForceToCenter(b2_body_id, _force, true); // TODO: what is wake?
    });
}

void Scene::applyImpulseToBodyCenter(uint64_t _body_id, const Point & _impulse)
{
    m_defers.push_front([this, _body_id, _impulse]() {
        b2BodyId b2_body_id = findBody(_body_id);
        if(B2_IS_NON_NULL(b2_body_id))
            b2Body_ApplyLinearImpulseToCenter(b2_body_id, _impulse, true); // TODO: what is wake?
    });
}

std::optional<Point> Scene::getBodyLinearVelocity(uint64_t _body_id) const
{
    b2BodyId b2_body = findBody(_body_id);
    if(B2_IS_NULL(b2_body))
        return std::nullopt;
    return asPoint(b2Body_GetLinearVelocity(b2_body));
}

bool Scene::setBodyLinearVelocity(uint64_t _body_id, const Point & _velocity) const
{
    b2BodyId b2_body = findBody(_body_id);
    if(B2_IS_NULL(b2_body))
        return false;
    b2Body_SetLinearVelocity(b2_body, asBox2dVec2(_velocity));
    return true;
}

std::optional<float> Scene::getBodyMass(uint64_t _body_id) const
{
    b2BodyId b2_body = findBody(_body_id);
    if(B2_IS_NULL(b2_body))
        return std::nullopt;
    return b2Body_GetMass(b2_body);
}

void Scene::setBodyPosition(uint64_t _body_id, const Point & _position)
{
    m_defers.push_front([this, _body_id, _position]() {
        b2BodyId b2_body_id = findBody(_body_id);
        if(B2_IS_NON_NULL(b2_body_id))
            b2Body_SetTransform(b2_body_id, _position, b2Body_GetRotation(b2_body_id));
    });
}

std::optional<Point> Scene::getBodyPosition(uint64_t _body_id) const
{
    b2BodyId b2_body_id = findBody(_body_id);
    if(B2_IS_NON_NULL(b2_body_id))
        return asPoint(b2Body_GetPosition(b2_body_id));
    return std::nullopt;
}

std::optional<std::vector<Point>> Scene::findPath(
    uint64_t _body_id,
    const Point & _destination,
    bool _allow_diagonal_steps,
    bool _avoid_sensors) const
{
    const b2BodyId b2_body_id = findBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return std::nullopt;
    AStarOptions options;
    options.allow_diagonal_steps = _allow_diagonal_steps;
    options.avoid_sensors = _avoid_sensors;
    auto b2_result = aStarFindPath(m_b2_world_id, b2_body_id, _destination, options);
    if(!b2_result.has_value())
        return std::nullopt;
    std::vector<Point> result;
    result.reserve(b2_result.value().size());
    for(size_t i = 0; i < b2_result.value().size(); ++i)
        result.push_back(makePoint(b2_result.value()[i].x, b2_result.value()[i].y));
    return result;
}
