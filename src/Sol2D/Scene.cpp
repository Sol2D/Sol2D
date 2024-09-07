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

#include <Sol2D/Scene.h>
#include <Sol2D/Tiles/Tmx.h>
#include <Sol2D/Utils/Observable.h>
#include <Sol2D/SDL/SDL.h>
#include <Sol2D/AStar.h>
#include <box2d/box2d.h>

using namespace Sol2D;
using namespace Sol2D::SDL;
using namespace Sol2D::Tiles;
using namespace Sol2D::Utils;

namespace {

class BodyShape final
{
    S2_DISABLE_COPY_AND_MOVE(BodyShape)

public:
    BodyShape(const std::string & _key, std::optional<uint32_t> _tile_map_object_id);
    ~BodyShape();
    const std::string & getKey() const;
    const std::optional<uint32_t> getTileMapObjectId() const;
    void addGraphic(const std::string & _key, const BodyShapeGraphics & _graphic);
    bool setCurrentGraphic(const std::string & _key);
    BodyShapeGraphics * getCurrentGraphics();
    bool flipGraphic(const std::string & _key, bool _flip_horizontally, bool _flip_vertically);

private:
    const std::string m_key;
    const std::optional<uint32_t> m_tile_map_object_id;
    std::unordered_map<std::string, BodyShapeGraphics *> m_graphics;
    BodyShapeGraphics * mp_current_graphic;
};

inline BodyShape::BodyShape(const std::string & _key, std::optional<uint32_t> _tile_map_object_id) :
    m_key(_key),
    m_tile_map_object_id(_tile_map_object_id),
    mp_current_graphic(nullptr)
{
}

inline BodyShape::~BodyShape()
{
    for(auto & pair : m_graphics)
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

inline void BodyShape::addGraphic(const std::string & _key, const BodyShapeGraphics & _graphic)
{
    auto it = m_graphics.find(_key);
    if(it != m_graphics.end())
        delete it->second;
    m_graphics[_key] = new BodyShapeGraphics(_graphic);
}

inline bool BodyShape::setCurrentGraphic(const std::string & _key)
{
    auto it = m_graphics.find(_key);
    if(it == m_graphics.end())
        return false;
    mp_current_graphic = it->second;
    return true;
}

inline BodyShapeGraphics * BodyShape::getCurrentGraphics()
{
    return mp_current_graphic;
}

inline bool BodyShape::flipGraphic(const std::string & _key, bool _flip_horizontally, bool _flip_vertically)
{
    auto it = m_graphics.find(_key);
    if(it == m_graphics.end())
        return false;
    it->second->options.is_flipped_horizontally = _flip_horizontally;
    it->second->options.is_flipped_vertically = _flip_vertically;
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
        std::optional<uint32_t> _tile_map_object_id = std::optional<uint32_t>());
    BodyShape * findShape(const std::string & _key);

private:
    static uint64_t s_next_id;
    uint64_t m_id;
    std::optional<std::string> m_layer;
    std::unordered_multimap<std::string, BodyShape *> m_shapes;
};

uint64_t Body::s_next_id = 1;

inline Body::Body() :
    m_id(s_next_id++)
{
}

inline Body::~Body()
{
    for(auto & shape : m_shapes)
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
    std::optional<uint32_t> _tile_map_object_id /*= std::optional<uint32_t>()*/)
{
    BodyShape * shape = new BodyShape(_key, _tile_map_object_id);
    m_shapes.insert(std::make_pair(_key, shape));
    return *shape;
}

inline BodyShape * Body::findShape(const std::string & _key)
{
    auto it = m_shapes.find(_key);
    return it == m_shapes.end() ? nullptr : it->second;
}

inline Body * getUserData(b2Body * _body)
{
    return reinterpret_cast<Body *>(_body->GetUserData().pointer);
}

inline BodyShape * getUserData(b2Fixture * _fixture)
{
    return reinterpret_cast<BodyShape *>(_fixture->GetUserData().pointer);
}

} // namespace

namespace Sol2D::Private {

class SceneContactListener final : public b2ContactListener, public Observable<ContactObserver>
{
public:
    void BeginContact(b2Contact * _contact) override;
    void EndContact(b2Contact * _contact) override;

private:
    bool tryGetContact(b2Contact & _b2_contact, Contact & _contact);
};

void SceneContactListener::BeginContact(b2Contact * _contact)
{
    if(!_contact->IsTouching())
        return;
    Contact contact;
    if(tryGetContact(*_contact, contact))
        callObservers(&ContactObserver::beginContact, contact);
}

bool SceneContactListener::tryGetContact(b2Contact & _b2_contact, Contact & _contact)
{
    b2Fixture * b2_fixture_a = _b2_contact.GetFixtureA();
    b2Fixture * b2_fixture_b = _b2_contact.GetFixtureB();
    b2Body * b2_body_a = b2_fixture_a->GetBody();
    b2Body * b2_body_b = b2_fixture_b->GetBody();
    BodyShape * shape_a = getUserData(b2_fixture_a);
    BodyShape * shape_b = getUserData(b2_fixture_b);
    Body * body_a = getUserData(b2_body_a);
    Body * body_b = getUserData(b2_body_b);
    if(shape_a && shape_b && body_a && body_b)
    {
        _contact.side_a = {
            .body_id = body_a->getId(),
            .shape_key = shape_a->getKey(),
            .tile_map_object_id = shape_a->getTileMapObjectId()
        };
        _contact.side_b = {
            .body_id = body_b->getId(),
            .shape_key = shape_b->getKey(),
            .tile_map_object_id = shape_b->getTileMapObjectId()
        };
        return true;
    }
    return false;
}

void SceneContactListener::EndContact(b2Contact * _contact)
{
    Contact contact;
    if(tryGetContact(*_contact, contact))
        callObservers(&ContactObserver::endContact, contact);
}

} // namespace Sol2D::Private

Scene::Scene(const Workspace & _workspace, SDL_Renderer & _renderer) :
    mr_workspace(_workspace),
    mr_renderer(_renderer),
    m_world_offset{.0f, .0f},
    mp_b2_world(new b2World({ .0f, .0f })), // TODO: from parameters
    m_scale_factor(20.0f), // TODO: from parameters
    mp_followed_body(nullptr),
    mp_contact_listener(new Private::SceneContactListener)
{
    mp_b2_world->SetContactListener(mp_contact_listener);
}

Scene::~Scene()
{
    deinitialize();
    delete mp_b2_world;
    delete mp_contact_listener;
}

void Scene::deinitialize()
{
    for(auto & pair : m_bodies)
        destroyBody(pair.second);
    m_bodies.clear();
    m_tile_heap_ptr.reset();
    m_object_heap_ptr.reset();
    m_tile_map_ptr.reset();
    mp_followed_body = nullptr;
}

void Scene::setGravity(const Point & _vector)
{
    if(mp_b2_world->IsLocked())
        m_defers.push_front([_vector, this]() { setGravity(_vector); });
    else
        mp_b2_world->SetGravity(*_vector.toBox2DPtr()); // TODO: scale factor?
}

uint64_t Scene::createBody(const Point & _position, const BodyPrototype & _prototype)
{
    Body * body = new Body;
    b2BodyDef b2_body_def;
    b2_body_def.type = mapBodyType(_prototype.getType());
    b2_body_def.position.Set(
        _position.x / m_scale_factor,
        _position.y / m_scale_factor
    );
    b2_body_def.linearDamping = 100; // TODO: for top-down
    b2_body_def.angularDamping = 100; // TODO: must be controlled by user (prevent infinite rotation)
    b2_body_def.fixedRotation = true; // TODO: must be controlled by user
    b2_body_def.userData.pointer = reinterpret_cast<uintptr_t>(body);
    b2Body * b2_body = mp_b2_world->CreateBody(&b2_body_def);
    m_bodies.insert(std::make_pair(body->getId(), b2_body));

    _prototype.forEachShape([this, body, b2_body, &_prototype](
                                const std::string & __key,
                                const BodyShapePrototype & __shape_proto) {
        BodyShape * body_shape = nullptr;
        switch(__shape_proto.getType())
        {
        case BodyShapeType::Polygon:
        {
            const BodyPolygonShapePrototype * polygon_proto =
                dynamic_cast<const BodyPolygonShapePrototype *>(&__shape_proto);
            if(!polygon_proto)
                break;
            const std::vector<Point> & points = polygon_proto->getPoints();
            if(points.size() < 3 || points.size() > b2_maxPolygonVertices)
                break;
            b2PolygonShape b2_shape;
            std::vector<b2Vec2> shape_points(points.size());
            for(size_t i = 0; i < points.size(); ++i)
            {
                shape_points[i].x = points[i].x / m_scale_factor;
                shape_points[i].y = points[i].y / m_scale_factor;
            }
            b2_shape.Set(shape_points.data(), shape_points.size());
            b2FixtureDef fx_def;
            fx_def.shape = &b2_shape;
            fx_def.isSensor = polygon_proto->isSensor();
            if(_prototype.getType() == BodyType::Dynamic) // TODO: duplicated
            {
                fx_def.density = .002f; // TODO: real value from user
            }
            body_shape = &body->createShape(__key);
            fx_def.userData.pointer = reinterpret_cast<uintptr_t>(body_shape);
            b2_body->CreateFixture(&fx_def);
        }
        break;
        case BodyShapeType::Circle:
        {
            const BodyCircleShapePrototype * circle_proto =
                dynamic_cast<const BodyCircleShapePrototype *>(&__shape_proto);
            if(!circle_proto)
                break;
            const float radius = circle_proto->getRadius() / m_scale_factor;
            if(radius <= .0f)
                break;
            Point position = circle_proto->getCenter();
            position.x /= m_scale_factor;
            position.y /= m_scale_factor;
            b2CircleShape b2_shape;
            b2_shape.m_p.Set(position.x, position.y);
            b2_shape.m_radius = radius;
            b2FixtureDef fx_def;
            fx_def.shape = &b2_shape;
            fx_def.isSensor = circle_proto->isSensor();
            if(_prototype.getType() == BodyType::Dynamic) // TODO: duplicated
            {
                fx_def.density = .002f; // TODO: real value from user
            }
            body_shape = &body->createShape(__key);
            fx_def.userData.pointer = reinterpret_cast<uintptr_t>(body_shape);
            b2_body->CreateFixture(&fx_def);
        }
        break;
        default: return;
        }
        __shape_proto.forEachGraphic(
            [body_shape](const std::string & __key, const BodyShapeGraphics & __graphic) {
                body_shape->addGraphic(__key, __graphic);
            });
    });

    return body->getId();
}

void Scene::createBodiesFromMapObjects(
    const std::string & _class,
    const BodyOptions & _body_options,
    const BodyShapeOptions & _shape_options)
{
    b2BodyType body_type = mapBodyType(_body_options.type);

    m_object_heap_ptr->forEachObject([&](const TileMapObject & __map_object) {
        if(__map_object.getClass() != _class) return;
        Body * body = new Body;
        b2BodyDef b2_body_def;
        b2_body_def.type = body_type;
        b2_body_def.position.Set(
            __map_object.getX() / m_scale_factor,
            __map_object.getY() / m_scale_factor
        );
        b2_body_def.linearDamping = _body_options.linear_damping;
        b2_body_def.angularDamping = _body_options.angular_damping;
        b2_body_def.fixedRotation = _body_options.fixed_rotation;
        b2_body_def.userData.pointer = reinterpret_cast<uintptr_t>(body);
        b2Body * b2_body = mp_b2_world->CreateBody(&b2_body_def);
        m_bodies.insert(std::make_pair(body->getId(), b2_body));

        BodyShape * body_shape = nullptr;
        switch(__map_object.getObjectType())
        {
        case TileMapObjectType::Polygon:
        {
            const TileMapPolygon * polygon = static_cast<const TileMapPolygon *>(&__map_object);
            const std::vector<Point> & points = polygon->getPoints();
            if(points.size() < 3 || points.size() > b2_maxPolygonVertices)
                break;
            b2PolygonShape b2_shape;
            std::vector<b2Vec2> shape_points(points.size());
            for(size_t i = 0; i < points.size(); ++i)
            {
                shape_points[i].x = points[i].x / m_scale_factor;
                shape_points[i].y = points[i].y / m_scale_factor;
            }
            b2_shape.Set(shape_points.data(), shape_points.size());
            b2FixtureDef fx_def;
            fx_def.shape = &b2_shape;
            fx_def.isSensor = _shape_options.is_sensor;
            if(_body_options.type == BodyType::Dynamic)
            {
                fx_def.density = _shape_options.density;
            }
            body_shape = &body->createShape(_class, polygon->getId());
            fx_def.userData.pointer = reinterpret_cast<uintptr_t>(body_shape);
            b2_body->CreateFixture(&fx_def);
        }
        break;
        case TileMapObjectType::Circle:
        {
            const TileMapCircle * circle = static_cast<const TileMapCircle *>(&__map_object);
            const float radius = circle->getRadius() / m_scale_factor;
            if(radius <= .0f)
                break;
            b2CircleShape b2_shape;
            b2_shape.m_p.Set(0, 0);
            b2_shape.m_radius = radius;
            b2FixtureDef fx_def;
            fx_def.shape = &b2_shape;
            fx_def.isSensor = _shape_options.is_sensor;
            if(_body_options.type == BodyType::Dynamic)
            {
                fx_def.density = _shape_options.density;
            }
            body_shape = &body->createShape(_class, circle->getId());
            fx_def.userData.pointer = reinterpret_cast<uintptr_t>(body_shape);
            b2_body->CreateFixture(&fx_def);
        }
        break;
        default: return;
        }
    });
}

bool Scene::destroyBody(uint64_t _body_id)
{
    b2Body * body = findBody(_body_id);
    if(!body)
        return false;
    mp_b2_world->DestroyBody(body);
    if(mp_followed_body == body)
        mp_followed_body = nullptr;
    m_bodies.erase(_body_id);
    destroyBody(body);
    return true;
}

void Scene::destroyBody(b2Body * _body)
{
    Body * body = getUserData(_body);
    mp_b2_world->DestroyBody(_body);
    delete body;
}

b2Body * Scene::findBody(uint64_t _body_id) const
{
    auto it = m_bodies.find(_body_id);
    return it == m_bodies.end() ? nullptr : it->second;
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
    mp_followed_body = findBody(_body_id);
    return mp_followed_body != nullptr;
}

void Scene::resetFollowedBody()
{
    mp_followed_body = nullptr;
}

bool Scene::setBodyLayer(uint64_t _body_id, const std::string & _layer)
{
    b2Body * body = findBody(_body_id);
    if(body == nullptr)
        return false;
    getUserData(body)->setLayer(_layer);
    return true;
}

bool Scene::setBodyShapeCurrentGraphic(
    uint64_t _body_id,
    const std::string & _shape_key,
    const std::string & _graphic_key)
{
    b2Body * body = findBody(_body_id);
    if(body == nullptr)
        return false;
    BodyShape * shape = getUserData(body)->findShape(_shape_key);
    if(shape == nullptr)
        return false;
    return shape->setCurrentGraphic(_graphic_key);
}

bool Scene::flipBodyShapeGraphic(
    uint64_t _body_id,
    const std::string & _shape_key,
    const std::string & _graphic_key,
    bool _flip_horizontally,
    bool _flip_vertically)
{
    b2Body * body = findBody(_body_id);
    if(body == nullptr)
        return false;
    BodyShape * shape = getUserData(body)->findShape(_shape_key);
    if(shape == nullptr)
        return false;
    return shape->flipGraphic(_graphic_key, _flip_horizontally, _flip_vertically);
}

bool Scene::loadTileMap(const std::filesystem::path & _file_path)
{
    deinitialize();
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
    mp_b2_world->Step(_state.time_passed.count() / 1000.0f, 8, 3); // TODO: stable rate (1.0f / 60.0f), all from user settings
    syncWorldWithFollowedBody();
    const Color & bg_color = m_tile_map_ptr->getBackgroundColor();
    SDL_SetRenderDrawColor(&mr_renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderClear(&mr_renderer);

    std::unordered_set<uint64_t> bodies_to_render(m_bodies.size());
    for(const auto & pair : m_bodies)
        bodies_to_render.insert(pair.first);
    drawLayersAndBodies(*m_tile_map_ptr, bodies_to_render, _state.time_passed);
    for(const uint64_t body_id : bodies_to_render)
        drawBody(*m_bodies[body_id], _state.time_passed);

    drawBox2D();
}

void Scene::executeDefers()
{
    if(!m_defers.empty())
    {
        for(auto & action : m_defers)
            action();
        m_defers.clear();
    }
}

void Scene::syncWorldWithFollowedBody()
{
    if(mp_followed_body == nullptr)
    {
        return;
    }
    int output_width, output_height;
    SDL_GetCurrentRenderOutputSize(&mr_renderer, &output_width, &output_height);
    b2Vec2 followed_body_position = mp_followed_body->GetPosition();
    m_world_offset.x = followed_body_position.x * m_scale_factor - static_cast<float>(output_width) / 2;
    m_world_offset.y = followed_body_position.y * m_scale_factor - static_cast<float>(output_height) / 2;
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

void Scene::drawBody(b2Body & _body, std::chrono::milliseconds _time_passed)
{
    GraphicsRenderOptions options; // TODO: to FixtureUserData? How to rotate multiple shapes?
    Point body_position;
    {
        b2Vec2 pos = _body.GetPosition();
        body_position = toAbsoluteCoords(pos.x * m_scale_factor, pos.y * m_scale_factor);
    }
    for(b2Fixture * fixture = _body.GetFixtureList(); fixture; fixture = fixture->GetNext())
    {
        BodyShape * shape = getUserData(fixture);
        BodyShapeGraphics * shape_graphic = shape->getCurrentGraphics();
        if(shape_graphic)
        {
            options.flip = shape_graphic->options.getFlip();
            shape_graphic->graphics.render(body_position + shape_graphic->options.position, _time_passed, options);
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
            b2Body * box2d_body = pair.second;
            Body * body = getUserData(box2d_body);
            if(body->getLayer() == __layer.getName() && _bodies_to_render.erase(pair.first))
                drawBody(*box2d_body, _time_passed);
        }
    });
}

void Scene::drawObjectLayer(const TileMapObjectLayer & _layer)
{
    SDL_SetRenderDrawColor(&mr_renderer, 10, 0, 200, 100);
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
    Rect camera;
    camera.x = m_world_offset.x;
    camera.y = m_world_offset.y;
    {
        int w, h;
        SDL_GetCurrentRenderOutputSize(&mr_renderer, &w, &h);
        camera.w = static_cast<float>(w);
        camera.h = static_cast<float>(h);
    }

    const float first_col = std::floor(camera.x / m_tile_map_ptr->getTileWidth());
    const float first_row = std::floor(camera.y / m_tile_map_ptr->getTileHeight());
    const float last_col = std::ceil((camera.x + camera.w) / m_tile_map_ptr->getTileWidth());
    const float last_row = std::ceil((camera.y + camera.h) / m_tile_map_ptr->getTileHeight());
    const float start_x = first_col * m_tile_map_ptr->getTileWidth() - camera.x;
    const float start_y = first_row * m_tile_map_ptr->getTileHeight() - camera.y;

    SDL_FRect src_rect;
    SDL_FRect dest_rect =
    {
        .x = .0f,
        .y = .0f,
        .w = static_cast<float>(m_tile_map_ptr->getTileWidth()),
        .h = static_cast<float>(m_tile_map_ptr->getTileHeight())
    };

    for(int32_t row = static_cast<int32_t>(first_row), dest_row = 0; row <= static_cast<int32_t>(last_row); ++row, ++dest_row)
    {
        for(int32_t col = static_cast<int32_t>(first_col), dest_col = 0; col <= static_cast<int32_t>(last_col); ++col, ++dest_col)
        {
            const Tile * tile = _layer.getTile(col, row);
            if(!tile) continue;

            src_rect.x = tile->getSourceX();
            src_rect.y = tile->getSourceY();
            src_rect.w = tile->getWidth();
            src_rect.h = tile->getHeight();

            dest_rect.x = start_x + dest_col * m_tile_map_ptr->getTileWidth();
            dest_rect.y = start_y + dest_row * m_tile_map_ptr->getTileHeight();

            SDL_RenderTexture(&mr_renderer, &tile->getSource(), &src_rect, &dest_rect);
        }
    }
}

void Scene::drawImageLayer(const TileMapImageLayer & _layer)
{
    std::shared_ptr<SDL_Texture> image = _layer.getImage();
    int width, height;
    SDL_QueryTexture(image.get(), NULL, NULL, &width, &height);
    SDL_FRect dim { .0f, .0f, static_cast<float>(width), static_cast<float>(height) };
    SDL_RenderTexture(&mr_renderer, image.get(), nullptr, &dim);
}

void Scene::drawBox2D()
{
    for(b2Body * body = mp_b2_world->GetBodyList(); body; body = body->GetNext())
    {
        const b2Vec2 & body_pos = body->GetPosition();
        const Point body_abs_pos = toAbsoluteCoords(body_pos.x * m_scale_factor, body_pos.y * m_scale_factor);
        SDL_SetRenderDrawColor(&mr_renderer, 20, 255, 255, 100);
        SDL_RenderLine(&mr_renderer, body_abs_pos.x, body_abs_pos.y - 8, body_abs_pos.x, body_abs_pos.y + 8);
        SDL_RenderLine(&mr_renderer, body_abs_pos.x - 8, body_abs_pos.y, body_abs_pos.x + 8, body_abs_pos.y);
        for(b2Fixture * fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            if(fixture->IsSensor())
                SDL_SetRenderDrawColor(&mr_renderer, 20, 255, 20, 100);
            else
                SDL_SetRenderDrawColor(&mr_renderer, 255, 20, 20, 100);
            b2Shape * shape = fixture->GetShape();
            switch(shape->GetType()) {
            case b2Shape::e_polygon:
            {
                b2PolygonShape * polygon = dynamic_cast<b2PolygonShape *>(shape);
                float angle = body->GetAngle();
                std::unique_ptr<VectorRotator> rotator;
                if(angle)
                    rotator.reset(new VectorRotator(angle));
                for(int32 i = 1; i < polygon->m_count; ++i)
                {
                    Point a = makePoint(polygon->m_vertices[i - 1].x, polygon->m_vertices[i - 1].y);
                    Point b = makePoint(polygon->m_vertices[i].x, polygon->m_vertices[i].y);
                    if(rotator)
                    {
                        a = rotator->rotate(a);
                        b = rotator->rotate(b);
                    }
                    a = toAbsoluteCoords((a.x + body_pos.x) * m_scale_factor, (a.y + body_pos.y) * m_scale_factor);
                    b = toAbsoluteCoords((b.x + body_pos.x) * m_scale_factor, (b.y + body_pos.y) * m_scale_factor);
                    SDL_RenderLine(&mr_renderer, a.x, a.y, b.x, b.y);
                }
                Point a =  makePoint(polygon->m_vertices[0].x, polygon->m_vertices[0].y);
                Point b = makePoint(
                    polygon->m_vertices[polygon->m_count - 1].x,
                    polygon->m_vertices[polygon->m_count - 1].y);
                if(rotator)
                {
                    a = rotator->rotate(a);
                    b = rotator->rotate(b);
                }
                a = toAbsoluteCoords((a.x + body_pos.x) * m_scale_factor, (a.y + body_pos.y) * m_scale_factor);
                b = toAbsoluteCoords((b.x + body_pos.x) * m_scale_factor, (b.y + body_pos.y) * m_scale_factor);
                SDL_RenderLine(&mr_renderer, a.x, a.y, b.x, b.y);
                break;
            }
            case b2Shape::e_circle:
            {
                b2CircleShape * circle = dynamic_cast<b2CircleShape *>(shape);
                Point center = toAbsoluteCoords(
                    body_pos.x * m_scale_factor,
                    body_pos.y * m_scale_factor
                );
                sdlRenderCircle(&mr_renderer, center, static_cast<uint32_t>(circle->m_radius * m_scale_factor));
                break;
            }
            default:
                break;
            }
        }
    }
}

Point Scene::toAbsoluteCoords(float _world_x, float _world_y) const
{
    Point result;
    result.x = _world_x - m_world_offset.x;
    result.y = _world_y - m_world_offset.y;
    return result;
}

void Scene::applyForce(uint64_t _body_id, const Point & _force)
{
    if(mp_b2_world->IsLocked())
        m_defers.push_front([=, this]() { applyForce(_body_id, _force); });
    else if(b2Body * body = findBody(_body_id))
        body->ApplyForceToCenter(b2Vec2(_force.x / m_scale_factor, _force.y / m_scale_factor), true); // TODO: what is wake?
}

void Scene::setBodyPosition(uint64_t _body_id, const Point & _position)
{
    if(mp_b2_world->IsLocked())
        m_defers.push_front([=, this]() { setBodyPosition(_body_id, _position); });
    else if(b2Body * body = findBody(_body_id))
        body->SetTransform(b2Vec2(_position.x / m_scale_factor, _position.y / m_scale_factor), body->GetAngle());
}

std::optional<Point> Scene::getBodyPosition(uint64_t _body_id) const
{
    if(b2Body * body = findBody(_body_id))
    {
        b2Vec2 pos = body->GetPosition();
        return makePoint(pos.x * m_scale_factor, pos.y * m_scale_factor);
    }
    return std::optional<Point>();
}

void Scene::addObserver(ContactObserver & _observer)
{
    mp_contact_listener->addObserver(_observer);
}

void Scene::removeObserver(ContactObserver & _observer)
{
    mp_contact_listener->removeObserver(_observer);
}

std::optional<std::vector<Point>> Scene::findPath(
    uint64_t _body_id,
    const Point & _destination,
    bool _allow_diagonal_steps,
    bool _avoid_sensors) const
{
    const b2Body * body = findBody(_body_id);
    if(!body)
        return std::optional<std::vector<Point>>();
    b2Vec2 dest(_destination.x / m_scale_factor, _destination.y / m_scale_factor);
    AStarOptions options;
    options.allow_diagonal_steps = _allow_diagonal_steps;
    options.avoid_sensors = _avoid_sensors;
    auto b2_result = aStarFindPath(*mp_b2_world, *body, dest, options);
    if(!b2_result.has_value())
        return std::optional<std::vector<Point>>();
    std::vector<Point> result(b2_result.value().size());
    for(size_t i = 0; i < b2_result.value().size(); ++i)
        result[i] = makePoint(b2_result.value()[i].x * m_scale_factor, b2_result.value()[i].y * m_scale_factor);
    return result;
}
