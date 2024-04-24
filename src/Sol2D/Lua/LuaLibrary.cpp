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
#include <Sol2D/Lua/LuaAux.h>
#include <Sol2D/Lua/LuaWorldApi.h>
#include <Sol2D/Lua/LuaHeartbeatApi.h>
#include <Sol2D/Lua/LuaScancodeApi.h>
#include <Sol2D/Lua/LuaBodyTypeApi.h>
#include <Sol2D/Lua/LuaBodyShapePrototypeApi.h>
#include <Sol2D/Lua/LuaBodyShapeTypeApi.h>
#include <Sol2D/Lua/LuaKeyboardApi.h>
#include <Sol2D/Lua/LuaTileMapObjectApi.h>
#include <Sol2D/Lua/LuaFragmentApi.h>
#include <functional>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

const char * gc_metatable_lib = "sol.Library";

void addSublibrary(lua_State * _lua, const char * _sublib_name, std::function<void()> _push_sublib)
{
    int library_table_idx = lua_gettop(_lua);
    lua_pushstring(_lua, _sublib_name);
    _push_sublib();
    lua_settable(_lua, library_table_idx);
}

} // namespace name

LuaLibrary::LuaLibrary(const Workspace & _workspace, World & _world) :
    mp_lua(luaL_newstate()),
    mr_workspace(_workspace)
{
    luaL_openlibs(mp_lua);
    lua_newuserdata(mp_lua, 1);
    if(pushMetatable(mp_lua, gc_metatable_lib) == MetatablePushResult::Created)
    {
        addSublibrary(mp_lua, "world", [this, &_world]() { pushWorldApi(mp_lua, mr_workspace, _world); });
        addSublibrary(mp_lua, "heartbeat", [this]() { pushHeartbeatApi(mp_lua); });
        addSublibrary(mp_lua, "keyboard", [this]() { pushKeyboardApiOntoStack(mp_lua); });
        addSublibrary(mp_lua, "Scancode", [this]() { pushScancodeEnum(mp_lua); });
        addSublibrary(mp_lua, "BodyType", [this]() { pushBodyTypeEnum(mp_lua); });
        addSublibrary(mp_lua, "BodyShapeType", [this]() { pushBodyShapeTypeEnum(mp_lua); });
        addSublibrary(mp_lua, "TileMapObjectType", [this]() { pushTileMapObjectTypeEnum(mp_lua); });
        addSublibrary(mp_lua, "FragmentSizeUnit", [this]() { pushFragmentSizeUnitEnum(mp_lua); });
    }
    lua_setmetatable(mp_lua, -2);
    lua_setglobal(mp_lua, "sol");
}

LuaLibrary::~LuaLibrary()
{
    lua_close(mp_lua);
}

void LuaLibrary::executeMainScript()
{
    executeScript(mp_lua, mr_workspace, mr_workspace.getMainScriptPath());
}

void LuaLibrary::step(std::chrono::milliseconds _time_passed)
{
    S2_UNUSED(_time_passed)

    doHeartbeat(mp_lua, mr_workspace);
}
