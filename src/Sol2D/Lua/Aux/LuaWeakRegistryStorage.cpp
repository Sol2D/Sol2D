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

#include <Sol2D/Lua/Aux/LuaTopStackTable.h>
#include <Sol2D/Lua/Aux/LuaWeakRegistryStorage.h>

using namespace Sol2D::Lua::Aux;

const char LuaWeakRegistryStorage::sc_storage_key = '\0';

void LuaWeakRegistryStorage::save(void * _key, int _idx)
{
    int object_abs_idx = lua_absindex(mp_lua, _idx);
    if(lua_rawgetp(mp_lua, LUA_REGISTRYINDEX, &sc_storage_key) != LUA_TTABLE)
    {
        lua_pop(mp_lua, 1);
        LuaTopStackTable table = LuaTopStackTable::pushNew(mp_lua);
        table.setStringValue("__mode", "v");
        lua_pushvalue(mp_lua, -1);
        lua_rawsetp(mp_lua, LUA_REGISTRYINDEX, &sc_storage_key);
    }
    lua_pushvalue(mp_lua, object_abs_idx);
    lua_rawsetp(mp_lua, -2, _key);
    lua_pop(mp_lua, 1);
}

bool LuaWeakRegistryStorage::tryGet(void * _key, int _expected_type)
{
    if(lua_rawgetp(mp_lua, LUA_REGISTRYINDEX, &sc_storage_key) != LUA_TTABLE)
    {
        lua_pop(mp_lua, 1);
        return false;
    }
    if(lua_rawgetp(mp_lua, -1, _key) != _expected_type)
    {
        lua_pop(mp_lua, 2);
        return false;
    }
    lua_remove(mp_lua, -2);
    return true;
}
