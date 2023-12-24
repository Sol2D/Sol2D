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

namespace {

const char * gc_mtable_heartbeat = "sol.Heartbeat";
const char gc_key_heartbeat_callbacks_value = '\0';
constexpr void * gc_key_heartbeat_callbacks = (void *)&gc_key_heartbeat_callbacks_value;

void luaPushApiMetatableHeartbeatOntoStack(lua_State * _lua);
void pushCallbackStorageTableOntoStack(lua_State * _lua);
void initializeCallbackStorageTable(lua_State * _lua);
int luaApi_Subscribe(lua_State * _lua);

void luaPushApiMetatableHeartbeatOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_mtable_heartbeat) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "subscribe", luaApi_Subscribe },
        { nullptr, nullptr }
    };

    luaL_newmetatable(_lua, gc_mtable_heartbeat);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
    luaL_setfuncs(_lua, funcs, 0);
}

// 1 callback
int luaApi_Subscribe(lua_State * _lua)
{
    luaL_argcheck(_lua, lua_isfunction(_lua, 1), lua_absindex(_lua, 1), "callback function is required");
    pushCallbackStorageTableOntoStack(_lua);
    lua_Unsigned callbacks_count = lua_rawlen(_lua, 2);
    lua_pushvalue(_lua, 1);
    lua_rawseti(_lua, 2, callbacks_count + 1);
    lua_pop(_lua, 1);
    return 0;
}

void pushCallbackStorageTableOntoStack(lua_State * _lua)
{
    lua_pushlightuserdata(_lua, gc_key_heartbeat_callbacks);
    if(lua_gettable(_lua, LUA_REGISTRYINDEX) == LUA_TTABLE)
        return;

    lua_pop(_lua, 1);
    initializeCallbackStorageTable(_lua);
    pushCallbackStorageTableOntoStack(_lua);
}

void initializeCallbackStorageTable(lua_State * _lua)
{
    lua_pushlightuserdata(_lua, gc_key_heartbeat_callbacks);
    lua_newtable(_lua);
    lua_settable(_lua, LUA_REGISTRYINDEX);
}

} // namespace

void Sol2D::Lua::luaPushHeartbeatApiOntoStack(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    luaPushApiMetatableHeartbeatOntoStack(_lua);
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::luaDoHeartbeat(lua_State * _lua, const Workspace & _workspace)
{
    pushCallbackStorageTableOntoStack(_lua);
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
