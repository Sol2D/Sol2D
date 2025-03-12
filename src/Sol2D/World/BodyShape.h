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
        Renderer & _renderer, const Utils::PreHashedKey<std::string> & _key, const GraphicsPackDefinition & _definition
    );
    bool setCurrentGraphics(const Utils::PreHashedKey<std::string> & _key);
    GraphicsPack * getCurrentGraphics();
    std::optional<Utils::PreHashedKey<std::string>> getCurrentGraphicsKey() const;
    GraphicsPack * getGraphics(const Utils::PreHashedKey<std::string> & _key);
    bool flipGraphics(const Utils::PreHashedKey<std::string> & _key, bool _flip_horizontally, bool _flip_vertically);

private:
    const std::string m_key;
    const std::optional<uint32_t> m_tile_map_object_id;
    Utils::PreHashedMap<std::string, GraphicsPack *> m_graphics;
    GraphicsPack * mp_current_graphics;
    std::optional<Utils::PreHashedKey<std::string>> m_current_graphics_key;
};

inline GraphicsPack * BodyShape::getCurrentGraphics()
{
    return mp_current_graphics;
}

inline std::optional<Utils::PreHashedKey<std::string>> BodyShape::getCurrentGraphicsKey() const
{
    return m_current_graphics_key;
}

} // namespace Sol2D::World
