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

#include <Sol2D/Utils/Math.h>

using namespace Sol2D::Utils;

Rotation::Rotation(double _value, Unit _unit)
{
    if(_unit == Unit::Degree)
        m_degrees = _value;
    else
        m_radians = _value;
    m_is_rotated = _value != 0.0;
}

Rotation::Rotation(double _sine, double _cosine) :
    m_sine(_sine),
    m_cosine(_cosine)
{
    m_is_rotated = _sine != 0.0;
}

double Rotation::getRadians() const
{
    if(m_radians.has_value())
    {
        return m_radians.value();
    }
    else if(!m_is_rotated)
    {
        m_radians = 0.0;
        return 0.0;
    }
    else if(m_degrees.has_value())
    {
        m_radians = degreesToRadians(m_degrees.value());
    }
    else
    {
        m_radians = atan2(m_sine.value(), m_cosine.value());
    }
    return m_radians.value();
}

double Rotation::getDegrees() const
{
    if(m_degrees.has_value())
    {
        return m_degrees.value();
    }
    else if(!m_is_rotated)
    {
        m_degrees = 0.0;
        return 0.0;
    }
    m_degrees = radiansToDegrees(getRadians());
    return m_degrees.value();
}

double Rotation::getSine() const
{
    if(m_sine.has_value())
    {
        return m_sine.value();
    }
    else if(!m_is_rotated)
    {
        m_sine = 0.0;
        return 0.0;
    }
    m_sine = sin(getRadians());
    return m_sine.value();
}

double Rotation::getCosine() const
{
    if(m_cosine.has_value())
    {
        return m_cosine.value();
    }
    else if(!m_is_rotated)
    {
        m_cosine = 1.0;
        return 1.0;
    }
    m_cosine = cos(getRadians());
    return m_cosine.value();
}

void Sol2D::Utils::rotateVectors(std::vector<Point> & _vectors, const Rotation & _rotation)
{
    VectorRotator rotator(_rotation);
    for(size_t i = 0; i < _vectors.size(); ++i)
        _vectors[i] = rotator.rotate(_vectors[i]);
}
