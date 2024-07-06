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

#include <Sol2D/BodyShapePrototype.h>
#include <Sol2D/BodyType.h>
#include <Sol2D/Utils/KeyValueStorage.h>
#include <functional>

namespace Sol2D {

class BodyPrototype final
{
    S2_DISABLE_COPY_AND_MOVE(BodyPrototype)

public:
    explicit BodyPrototype(BodyType _type);
    BodyType getType() const;
    BodyCircleShapePrototype & createCircleShape(const std::string & _key, Point _position, float _radius);
    BodyPolygonShapePrototype & createPolygonShape(const std::string & _key, const Rect & _rect);
    BodyPolygonShapePrototype & createPolygonShape(const std::string & _key, const std::vector<Point> & _points);
    void forEachShape(std::function<void (const std::string &, const BodyShapePrototype &)> _callback) const;

private:
    BodyType m_type;
    Utils::KeyValueStorage<std::string, BodyShapePrototype> m_shapes;
};

inline BodyPrototype::BodyPrototype(BodyType _type) :
    m_type(_type)
{
}

inline BodyType BodyPrototype::getType() const
{
    return m_type;
}

inline BodyCircleShapePrototype & BodyPrototype::createCircleShape(
    const std::string & _key,
    Point _position, float _radius)
{
    BodyCircleShapePrototype * shape = new BodyCircleShapePrototype(_position, _radius);
    m_shapes.addOrReplaceItem(_key, shape);
    return *shape;
}

inline BodyPolygonShapePrototype & BodyPrototype::createPolygonShape(const std::string & _key, const Rect & _rect)
{
    BodyPolygonShapePrototype * shape = new BodyPolygonShapePrototype(_rect);
    m_shapes.addOrReplaceItem(_key, shape);
    return *shape;
}

inline BodyPolygonShapePrototype & BodyPrototype::createPolygonShape(const std::string & _key,
                                                                    const std::vector<Point> & _points)
{
    BodyPolygonShapePrototype * shape = new BodyPolygonShapePrototype(_points);
    m_shapes.addOrReplaceItem(_key, shape);
    return *shape;
}

inline void BodyPrototype::forEachShape(
    std::function<void(const std::string & _key, const BodyShapePrototype &)> _callback) const
{
    for(const auto & pair : m_shapes)
        _callback(pair.first, *pair.second);
}

} // namespace Sol2D
