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

#include <Sol2D/GraphicsPack.h>
#include <Sol2D/Utils/PreHashedMap.h>

namespace Sol2D::World {

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
        const Utils::PreHashedKey<std::string> & _key,
        const GraphicsPackDefinition & _definition);
    bool setCurrentGraphics(const Utils::PreHashedKey<std::string> & _key);
    GraphicsPack * getCurrentGraphics();
    GraphicsPack * getGraphics(const Utils::PreHashedKey<std::string> & _key);
    bool flipGraphics(const Utils::PreHashedKey<std::string> & _key, bool _flip_horizontally, bool _flip_vertically);

private:
    const std::string m_key;
    const std::optional<uint32_t> m_tile_map_object_id;
    Utils::PreHashedMap<std::string, GraphicsPack *> m_graphics;
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
    const Utils::PreHashedKey<std::string> & _key,
    const GraphicsPackDefinition & _definition)
{
    auto it = m_graphics.find(_key);
    if(it != m_graphics.end())
        delete it->second;
    m_graphics[_key] = new GraphicsPack(_renderer, _definition);
}

inline bool BodyShape::setCurrentGraphics(const Utils::PreHashedKey<std::string> & _key)
{
    GraphicsPack * graphics = getGraphics(_key);
    if(graphics)
    {
        mp_current_graphic = graphics;
        return true;
    }
    return false;
}

inline GraphicsPack * BodyShape::getGraphics(const Utils::PreHashedKey<std::string> & _key)
{
    auto it = m_graphics.find(_key);
    return it == m_graphics.end() ? nullptr : it->second;
}

inline GraphicsPack * BodyShape::getCurrentGraphics()
{
    return mp_current_graphic;
}

inline bool BodyShape::flipGraphics(
    const Utils::PreHashedKey<std::string> & _key,
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

} // namespace Sol2D::World
