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

#include <Sol2D/Box2D.h>
#include <Sol2D/Body.h>
#include <Sol2D/Observable.h>

using namespace Sol2D;
using namespace Sol2D::Tiles;


struct Box2D::FixtureUserData
{
    TileMapObject * tile_map_object;
    Body * body;
};


class Box2D::DestructionListener : public b2DestructionListener
{
public:
    void SayGoodbye(b2Joint *) { }
    void SayGoodbye(b2Fixture * _fixture);
};


class Box2D::ContactListener : public b2ContactListener, public Observable<ContactObserver>
{
public:
    void BeginContact(b2Contact * _contact);
    void EndContact(b2Contact * _contact);
};


void Box2D::DestructionListener::SayGoodbye(b2Fixture * _fixture)
{
    delete reinterpret_cast<FixtureUserData *>(_fixture->GetUserData().pointer);
}

void Box2D::ContactListener::BeginContact(b2Contact * _contact)
{
    if(_contact->IsTouching())
    {
        FixtureUserData * data_a = reinterpret_cast<FixtureUserData *>(_contact->GetFixtureA()->GetUserData().pointer);
        FixtureUserData * data_b = reinterpret_cast<FixtureUserData *>(_contact->GetFixtureB()->GetUserData().pointer);
        if(data_a->body && data_b->body)
        {
            callObservers([data_a, data_b](ContactObserver & __observer) {
                __observer.onContacted(*data_a->body, *data_b->body);
            });
        }
        else
        {
            Body * body = data_a->body;
            TileMapObject * object = data_b->tile_map_object;
            if(!body || !object)
            {
                body = data_b->body;
                object = data_a->tile_map_object;
            }
            if(!body || !object) return;
            callObservers([body, object](ContactObserver & __observer) {
                __observer.onContacted(*body, *object);
            });
        }
    }
}

void Box2D::ContactListener::EndContact(b2Contact * _contact)
{
    // TODO: delete
    FixtureUserData * data_a = reinterpret_cast<FixtureUserData *>(_contact->GetFixtureA()->GetUserData().pointer);
    FixtureUserData * data_b = reinterpret_cast<FixtureUserData *>(_contact->GetFixtureB()->GetUserData().pointer);
    if(data_a->body && data_b->body)
    {
        callObservers([data_a, data_b](ContactObserver & __observer) {
            __observer.onDiscontacted(*data_a->body, *data_b->body);
        });
    }
    else
    {
        Body * body = data_a->body;
        TileMapObject * object = data_b->tile_map_object;
        if(!body || !object)
        {
            body = data_b->body;
            object = data_a->tile_map_object;
        }
        if(!body || !object) return;
        callObservers([body, object](ContactObserver & __observer) {
            __observer.onDiscontacted(*body, *object);
        });
    }
}

Box2D::Box2D(const b2Vec2 &  _gravity, float _scale_factor) :
    mp_world(new b2World(_gravity)),
    m_scale_factor(_scale_factor),
    mp_contact_listener(new ContactListener),
    mp_destruction_listener(new DestructionListener)
{
    mp_world->SetContactListener(mp_contact_listener);
    mp_world->SetDestructionListener(mp_destruction_listener);
}

Box2D::~Box2D()
{
    delete mp_world;
    delete mp_contact_listener;
    delete mp_destruction_listener;
}

void Box2D::reset()
{
    b2Body * body = mp_world->GetBodyList();
    while(body)
    {
        b2Body * next = body->GetNext();
        mp_world->DestroyBody(body);
        body = next;
    }
}

void Box2D::step(float _time_step, int32 _velocity_iterations, int32 _position_iterations)
{
    for(auto & action : m_deferred)
        action();
    m_deferred.clear();
    mp_world->Step(_time_step, _velocity_iterations, _position_iterations);
}

b2Body * Box2D::createDynamicBody(Body & _body)
{
    const float half_width = (static_cast<float>(_body.getWidth()) / 2) / m_scale_factor;
    const float half_height = (static_cast<float>(_body.getHeight()) / 2) / m_scale_factor;
    b2BodyDef b2_body_def;
    b2_body_def.type = b2_dynamicBody;
    b2_body_def.position.Set(
        half_width / m_scale_factor,
        half_height / m_scale_factor
    );
    b2_body_def.linearDamping = 100; // TODO: for top-down
    b2_body_def.angularDamping = 100; // TODO: must be controlled by user (prevent infinite rotation)
    b2_body_def.fixedRotation = true; // TODO: must be controlled by user
    b2Body * b2_body = mp_world->CreateBody(&b2_body_def); // TODO: destroy among with the Body object
    b2PolygonShape shape;
    shape.SetAsBox(half_width, half_height);
    b2FixtureDef b2_fix_def;
    b2_fix_def.shape = &shape;
    b2_fix_def.density = 0.3f;
    b2_fix_def.friction = 1.0f; // TODO: for side-scroller
    FixtureUserData * fixture_user_data = new FixtureUserData { };
    fixture_user_data->body = &_body;
    b2_fix_def.userData.pointer = reinterpret_cast<uintptr_t>(fixture_user_data);
    b2_body->CreateFixture(&b2_fix_def); // TODO: from user settings
    return b2_body;
}

b2Body * Box2D::createStaticBody(Tiles::TileMapObject & _tile_map_object, StaticObjectKind _kind)
{
    // TODO: polyline(?), point(?)

    switch(_tile_map_object.getObjectType())
    {
    case TileMapObjectType::Polygon:
        return createPolygonStaticBody(dynamic_cast<TileMapPolygon &>(_tile_map_object), _kind);
    case TileMapObjectType::Circle:
        return createCircleStaticBody(dynamic_cast<TileMapCircle &>(_tile_map_object), _kind);
    default:
        break;
    }
    return nullptr;
}

b2Body * Box2D::createPolygonStaticBody(Tiles::TileMapPolygon & _polygon, StaticObjectKind _kind)
{
    const std::vector<SDL_FPoint> & points = _polygon.getPoints();
    if(points.size() < 3 || points.size() > b2_maxPolygonVertices)
        return nullptr;
    const float x = _polygon.getX() / m_scale_factor;
    const float y = _polygon.getY() / m_scale_factor;
    b2PolygonShape shape;
    std::vector<b2Vec2> shape_points(points.size());
    for(size_t i = 0; i < points.size(); ++i)
    {
        shape_points[i].x = points[i].x / m_scale_factor;
        shape_points[i].y = points[i].y / m_scale_factor;
    }
    shape.Set(shape_points.data(), shape_points.size());
    b2BodyDef bd;
    bd.position.Set(x, y);
    b2Body * body = mp_world->CreateBody(&bd);
    b2FixtureDef fx_def;
    fx_def.shape = &shape;
    fx_def.isSensor = _kind == StaticObjectKind::Sensor;
    FixtureUserData * fixture_user_data = new FixtureUserData { };
    fixture_user_data->tile_map_object = &_polygon;
    fx_def.userData.pointer = reinterpret_cast<uintptr_t>(fixture_user_data);
    body->CreateFixture(&fx_def);
    return body;
}

b2Body * Box2D::createCircleStaticBody(Tiles::TileMapCircle & _circle, StaticObjectKind _kind)
{
    const float radius = _circle.getRadius() / m_scale_factor;
    if(radius <= .0f)
        return nullptr;
    b2BodyDef bd;
    bd.position.Set(
        _circle.getX() / m_scale_factor,
        _circle.getY() / m_scale_factor
    );
    b2Body * body = mp_world->CreateBody(&bd);
    b2CircleShape shape;
    shape.m_p.Set(.0f, .0f);
    shape.m_radius = radius;
    b2FixtureDef fx_def;
    fx_def.shape = &shape;
    fx_def.isSensor = _kind == StaticObjectKind::Sensor;
    FixtureUserData * fixture_user_data = new FixtureUserData { };
    fixture_user_data->tile_map_object = &_circle;
    fx_def.userData.pointer = reinterpret_cast<uintptr_t>(fixture_user_data);
    body->CreateFixture(&fx_def);
    return body;
}

void Box2D::addContactObserver(ContactObserver & _observer)
{
    mp_contact_listener->addObserver(_observer);
}

bool Box2D::removeContactObserver(ContactObserver & _observer)
{
    return mp_contact_listener->removeObserver(_observer);
}
