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

void BodyShapePrototype::addGraphic(
    const std::string & _key,
    const Sprite & _sprite,
    const BodyShapeGraphicOptions & _options)
{
    addGraphic(_key, new BodyShapeGraphic { .graphic = _sprite, .options = _options });
}

void BodyShapePrototype::addGraphic(
    const std::string & _key,
    const SpriteAnimation & _animation,
    const BodyShapeGraphicOptions & _options)
{
    addGraphic(_key, new BodyShapeGraphic { .graphic = _animation, .options = _options });
}

void BodyShapePrototype::addGraphic(const std::string & _key, BodyShapeGraphic * _graphic)
{
    auto existent_graphic_it = m_graphic_map.find(_key);
    if(existent_graphic_it == m_graphic_map.end())
    {
        m_graphic_map.insert(std::make_pair(_key, _graphic));
    }
    else
    {
        delete existent_graphic_it->second;
        existent_graphic_it->second = _graphic;
    }
}

bool BodyShapePrototype::removeGraphic(const std::string & _key)
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

void BodyShapePrototype::forEachGraphic(std::function<void (const std::string &, const BodyShapeGraphic &)> _callback) const
{
    for(auto & pair : m_graphic_map)
        _callback(pair.first, *pair.second);
}
