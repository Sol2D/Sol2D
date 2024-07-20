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

#include <Sol2D/Lua/LuaHeartbeatApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

char gc_registry_key_heartbeat = '\0';

using UserData = LuaUserData<LuaSelfBase, LuaTypeName::heartbeat>;

// 1 self
// 2 callback
int luaApi_Subscribe(lua_State * _lua)
{
    UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isfunction(_lua, 2), lua_absindex(_lua, 1), "callback function is required");
    pushTableFromRegistry(_lua, &gc_registry_key_heartbeat);
    lua_Unsigned callbacks_count = lua_rawlen(_lua, 3);
    lua_pushvalue(_lua, 2);
    lua_rawseti(_lua, 3, callbacks_count + 1);
    lua_pop(_lua, 1);
    return 0;
}

} // namespace

void Sol2D::Lua::pushHeartbeatApi(lua_State * _lua)
{
    UserData::pushUserData(_lua);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "subscribe", luaApi_Subscribe }, // TODO: how to unsubscribe when the body is dead? The only on callback must exist?
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::doHeartbeat(lua_State * _lua, const Workspace & _workspace)
{
    pushTableFromRegistry(_lua, &gc_registry_key_heartbeat);
    lua_Unsigned callbacks_count = lua_rawlen(_lua, -1);
    for(lua_Unsigned i = 1; i <= callbacks_count; ++i)
    {
        lua_rawgeti(_lua, -1, i);
        if(lua_isfunction(_lua, -1))
        {
            if(lua_pcall(_lua, 0, 0, 0) != LUA_OK)
                _workspace.getMainLogger().error(lua_tostring(_lua, -1));
        }
        else
        {
            lua_pop(_lua, 1);
        }
    }
    lua_pop(_lua, 1);
}
