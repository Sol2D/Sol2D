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

#include <Sol2D/Lua/Aux/LuaForward.h>
#include <Sol2D/World/Scene.h>

namespace Sol2D::Lua {

void pushBodyShapeApi(
    lua_State * _lua,
    std::shared_ptr<World::Scene> _scene,
    uint64_t _body_id,
    const Utils::PreHashedKey<std::string> & _shape_key);

} // namespace Sol2D::Lua
