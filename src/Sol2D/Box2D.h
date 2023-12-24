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

#include <Sol2D/Tiles/TileMapObject.h>
#include <Sol2D/Def.h>
#include <box2d/box2d.h>
#include <boost/container/slist.hpp>
#include <functional>

namespace Sol2D {

class Body;


class ContactObserver
{
public:
    virtual ~ContactObserver() { }
    virtual void onContacted(Body & _body, Tiles::TileMapObject & _object) = 0;
    virtual void onContacted(Body & _body1, Body & _body2) = 0;
    virtual void onDiscontacted(Body & _body, Tiles::TileMapObject & _object) = 0;
    virtual void onDiscontacted(Body & _body1, Body & _body2) = 0;
};


class Box2D final
{
    DISABLE_COPY(Box2D)

private:
    struct FixtureUserData;
    class ContactListener;
    class DestructionListener;

public:
    enum class StaticObjectKind
    {
        obstacle,
        sensor
    };

public:
    Box2D(const b2Vec2 &  _gravity, float _scale_factor);
    ~Box2D();
    void reset();
    bool isLocked() const;
    void step(float _time_step, int32 _velocity_iterations, int32 _position_iterations);
    void defer(std::function<void()> _action);
    b2Body * getBodyList() const;
    float getScaleFactor() const;
    b2Body * createDynamicBody(Body & _body);
    b2Body * createStaticBody(Tiles::TileMapObject & _tile_map_object, StaticObjectKind _kind);
    b2Body * createPolygonStaticBody(Tiles::TileMapPolygon & _polygon, StaticObjectKind _kind);
    b2Body * createCircleStaticBody(Tiles::TileMapCircle & _circle, StaticObjectKind _kind);
    void addContactObserver(ContactObserver & _observer);
    bool removeContactObserver(ContactObserver & _observer);

private:
    b2World * mp_world;
    float m_scale_factor;
    boost::container::slist<std::function<void()>> m_deferred;
    ContactListener * mp_contact_listener;
    DestructionListener * mp_destruction_listener;
};

inline bool Box2D::isLocked() const
{
    return mp_world->IsLocked();
}

inline void Box2D::defer(std::function<void()> _action)
{
    m_deferred.push_front(_action);
}

inline b2Body * Box2D::getBodyList() const
{
    return mp_world->GetBodyList();
}

inline float Box2D::getScaleFactor() const
{
    return m_scale_factor;
}

} // namespace Sol2D
