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

#include <Sol2D/Lua/LuaForward.h>
#include <Sol2D/Workspace.h>
#include <Sol2D/World.h>

namespace Sol2D::Lua {

class LuaLibrary final
{
public:
    S2_DISABLE_COPY_AND_MOVE(LuaLibrary)
    LuaLibrary(const Workspace & _workspace, World & _world);
    ~LuaLibrary();
    void executeMainScript();
    void step(const SDL_FRect & _viewport, std::chrono::milliseconds _time_passed);

private:
    lua_State * mp_lua;
    const Workspace & mr_workspace;
};

} // namespace Sol2D::Lua
