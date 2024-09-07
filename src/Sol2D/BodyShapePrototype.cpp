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


#include <Sol2D/BodyShapePrototype.h>

using namespace Sol2D;

BodyShapePrototype::~BodyShapePrototype()
{
    for(auto & pair : m_graphic_map)
        delete pair.second;
}


void BodyShapePrototype::addGraphics(
    const std::string & _key,
    const GraphicsPack & _graphics,
    const BodyShapeGraphicsOptions & _options)
{
    auto existent_graphic_it = m_graphic_map.find(_key);
    auto * body_shape_graphics = new BodyShapeGraphics { .graphics = _graphics, .options = _options };
    if(existent_graphic_it == m_graphic_map.end())
    {
        m_graphic_map.insert(std::make_pair(_key, body_shape_graphics));
    }
    else
    {
        delete existent_graphic_it->second;
        existent_graphic_it->second = body_shape_graphics;
    }
}

bool BodyShapePrototype::removeGraphics(const std::string & _key)
{
    auto it = m_graphic_map.find(_key);
    if(it != m_graphic_map.end())
    {
        delete it->second;
        m_graphic_map.erase(it);
        return true;
    }
    return false;
}

void BodyShapePrototype::forEachGraphic(
    std::function<void (const std::string &, const BodyShapeGraphics &)> _callback) const
{
    for(auto & pair : m_graphic_map)
        _callback(pair.first, *pair.second);
}
