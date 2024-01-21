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

#include <Sol2D/Lua/LuaLibrary.h>
#include <Sol2D/Lua/LuaHeartbeatApi.h>
#include <Sol2D/Lua/LuaWorldController.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
namespace fs = std::filesystem;

LuaWorldController::LuaWorldController(const Workspace & _workspace, World & _world) :
    mp_lua(luaL_newstate()),
    mr_workspace(_workspace),
    mr_world(_world)
{
    luaL_openlibs(mp_lua);
}

LuaWorldController::~LuaWorldController()
{
    lua_close(mp_lua);
}

void LuaWorldController::prepare()
{
    luaRegisterLibrary(mp_lua, mr_workspace, mr_world);
    executeMainScript();
}

void LuaWorldController::executeMainScript()
{
    luaL_loadfile(mp_lua, mr_workspace.getMainScriptPath().c_str());
    if(lua_pcall(mp_lua, 0, LUA_MULTRET, 0) != LUA_OK)
        mr_workspace.getMainLogger().error(lua_tostring(mp_lua, -1));
}


void LuaWorldController::render(const SDL_FRect & _viewport)
{
    luaDoHeartbeat(mp_lua, mr_workspace);
    mr_world.render(_viewport);
}
