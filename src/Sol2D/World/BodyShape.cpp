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

#include <Sol2D/World/BodyShape.h>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Utils;

BodyShape::BodyShape(const std::string & _key, std::optional<uint32_t> _tile_map_object_id) :
    m_key(_key),
    m_tile_map_object_id(_tile_map_object_id),
    mp_current_graphics(nullptr)
{
}

BodyShape::~BodyShape()
{
    for(const auto & pair : m_graphics)
        delete pair.second;
}

const std::string & BodyShape::getKey() const
{
    return m_key;
}

const std::optional<uint32_t> BodyShape::getTileMapObjectId() const
{
    return m_tile_map_object_id;
}

void BodyShape::addGraphics(
    SDL_Renderer & _renderer,
    const PreHashedKey<std::string> & _key,
    const GraphicsPackDefinition & _definition)
{
    auto it = m_graphics.find(_key);
    if(it != m_graphics.end())
        delete it->second;
    m_graphics[_key] = new GraphicsPack(_renderer, _definition);
}

bool BodyShape::setCurrentGraphics(const PreHashedKey<std::string> & _key)
{
    GraphicsPack * graphics = getGraphics(_key);
    if(graphics)
    {
        mp_current_graphics = graphics;
        m_current_graphics_key = _key;
        mp_current_graphics->switchToFirstVisibleFrame();
        return true;
    }
    return false;
}

GraphicsPack * BodyShape::getGraphics(const PreHashedKey<std::string> & _key)
{
    auto it = m_graphics.find(_key);
    return it == m_graphics.end() ? nullptr : it->second;
}

bool BodyShape::flipGraphics(
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
