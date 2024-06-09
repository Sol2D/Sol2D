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

#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/Aux/LuaCallbackStorage.h>

using namespace Sol2D;
using namespace Sol2D::Lua::Aux;

const char LuaCallbackStorage::sc_registry_key = '\0';
uint32_t LuaCallbackStorage::s_next_callback_id = 1;
uint32_t LuaCallbackStorage::s_next_set_id = 1;

uint32_t LuaCallbackStorage::addCallback(uint32_t _unique_set_id, int _callback_idx)
{
    const int callback_abs_idx = lua_absindex(mp_lua, _callback_idx);
    const uint32_t id = s_next_callback_id++;

    getIndexTable();
    if(!tryGetCallbackTableFromIndexTable(_unique_set_id))
    {
        LuaTable callback_table = LuaTable::pushNew(mp_lua);
        callback_table.setStringValue("__mode", "v");
        lua_pushinteger(mp_lua, static_cast<lua_Integer>(_unique_set_id));
        lua_pushvalue(mp_lua, -2);
        lua_settable(mp_lua, -4);
    }

    lua_pushinteger(mp_lua, id);
    lua_pushvalue(mp_lua, callback_abs_idx);
    lua_settable(mp_lua, -3);

    lua_pop(mp_lua, 2);
    return id;
}

bool LuaCallbackStorage::tryGetCallbackTableFromIndexTable(uint32_t _unique_set_id)
{
    lua_pushinteger(mp_lua, static_cast<lua_Integer>(_unique_set_id));
    if(lua_gettable(mp_lua, -2) != LUA_TTABLE)
    {
        lua_pop(mp_lua, 1);
        return false;
    }
    return true;
}

void LuaCallbackStorage::getIndexTable()
{
    if(lua_rawgetp(mp_lua, LUA_REGISTRYINDEX, &sc_registry_key) != LUA_TTABLE)
    {
        lua_pop(mp_lua, 1);
        lua_newtable(mp_lua);
        lua_pushvalue(mp_lua, -1);
        lua_rawsetp(mp_lua, LUA_REGISTRYINDEX, &sc_registry_key);
    }
}

void LuaCallbackStorage::removeCallback(uint32_t _unique_set_id, uint32_t _callback_id)
{
    getIndexTable();
    if(!tryGetCallbackTableFromIndexTable(_unique_set_id))
    {
        lua_pop(mp_lua, 1);
        return;
    }

    lua_pushinteger(mp_lua, static_cast<lua_Integer>(_callback_id));
    lua_pushnil(mp_lua);
    lua_settable(mp_lua, -3);

    lua_pop(mp_lua, 2);
}

void LuaCallbackStorage::callSet(const Workspace & _workspace, uint32_t _unique_set_id, uint16_t _args_count)
{
    const int args_top = lua_gettop(mp_lua);
    getIndexTable();
    if(!tryGetCallbackTableFromIndexTable(_unique_set_id))
        return;
    const int callbacks_table_idx = lua_gettop(mp_lua);
    lua_pushnil(mp_lua);
    while(lua_next(mp_lua, callbacks_table_idx))
    {
        if(lua_type(mp_lua, -2) != LUA_TNUMBER)
        {
            lua_pop(mp_lua, 1);
            continue;
        }
        for(uint32_t i = 1; i <= _args_count; ++i)
            lua_pushvalue(mp_lua, args_top - _args_count + i);
        if(lua_pcall(mp_lua, _args_count, 0, 0) != LUA_OK)
        {
            _workspace.getMainLogger().error(lua_tostring(mp_lua, -1));
            lua_pop(mp_lua, 1);
        }
    }
    lua_pop(mp_lua, _args_count + 2);
}

void LuaCallbackStorage::destroyCallbackSet(int _set_unique_id)
{
    getIndexTable();
    lua_pushinteger(mp_lua, _set_unique_id);
    lua_pushnil(mp_lua);
    lua_settable(mp_lua, -3);
}
