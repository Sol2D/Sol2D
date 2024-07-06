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

#include <Sol2D/SpriteAnimation.h>
#include <Sol2D/BodyShapeGraphic.h>
#include <Sol2D/BodyShapeType.h>
#include <unordered_map>
#include <functional>

namespace Sol2D {

class BodyShapePrototype
{
    friend class BodyPolygonShapePrototype;
    friend class BodyCircleShapePrototype;

    S2_DISABLE_COPY_AND_MOVE(BodyShapePrototype)

private:
    explicit BodyShapePrototype(BodyShapeType _type) :
        m_type(_type),
        m_is_sensor(false)
    {
    }

public:
    virtual ~BodyShapePrototype();

    BodyShapeType getType() const
    {
        return m_type;
    }

    void setIsSensor(bool _is_sensor)
    {
        m_is_sensor = _is_sensor;
    }

    bool isSensor() const
    {
        return m_is_sensor;
    }

    void addGraphic(const std::string & _key, const Sprite & _sprite, const BodyShapeGraphicOptions & _options);
    void addGraphic(const std::string & _key, const SpriteAnimation & _animation, const BodyShapeGraphicOptions & _options);
    bool removeGraphic(const std::string & _key);
    void forEachGraphic(std::function<void(const std::string & _key, const BodyShapeGraphic & _graphic)> _callback) const;

private:
    void addGraphic(const std::string & _key, BodyShapeGraphic * _graphic);

private:
    BodyShapeType m_type;
    bool m_is_sensor;
    std::unordered_map<std::string, BodyShapeGraphic *> m_graphic_map;
};

class BodyPolygonShapePrototype : public BodyShapePrototype
{
public:
    explicit BodyPolygonShapePrototype(const std::vector<Point> & _points) :
        BodyShapePrototype(BodyShapeType::Polygon),
        m_points(_points)
    {
    }

    explicit BodyPolygonShapePrototype(const Rect & _rect) :
        BodyShapePrototype(BodyShapeType::Polygon),
        m_points(4)
    {
        m_points[0].x = m_points[3].x = _rect.x;
        m_points[1].x = m_points[2].x = _rect.x + _rect.w;
        m_points[0].y = m_points[1].y = _rect.y;
        m_points[2].y = m_points[3].y = _rect.y + _rect.h;
    }

    const std::vector<Point> & getPoints() const
    {
        return m_points;
    }

private:
    std::vector<Point> m_points;
};

class BodyCircleShapePrototype : public BodyShapePrototype
{

public:
    BodyCircleShapePrototype(Point _center, float _radius) :
        BodyShapePrototype(BodyShapeType::Circle),
        m_center(_center),
        m_radius(_radius)
    {
    }

    Point getCenter() const
    {
        return m_center;
    }

    float getRadius() const
    {
        return m_radius;
    }

private:
    Point m_center;
    float m_radius;
};

} // namespace Sol2D
