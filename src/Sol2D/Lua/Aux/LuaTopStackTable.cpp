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

using namespace Sol2D::Lua::Aux;

void LuaTopStackTable::setValueFromTop(const char * _key)
{
    lua_pushstring(mp_lua, _key);
    int top = lua_gettop(mp_lua);
    lua_insert(mp_lua, top -1);
    lua_settable(mp_lua, -3);
}

void LuaTopStackTable::setIntegerValue(const char * _key, lua_Integer _value) const
{
    lua_pushstring(mp_lua, _key);
    lua_pushinteger(mp_lua, _value);
    lua_settable(mp_lua, -3);
}

void LuaTopStackTable::setNumberValue(const char * _key, lua_Number _value) const
{
    lua_pushstring(mp_lua, _key);
    lua_pushnumber(mp_lua, _value);
    lua_settable(mp_lua, -3);
}

void LuaTopStackTable::setBooleanValue(const char * _key, bool _value) const
{
    lua_pushstring(mp_lua, _key);
    lua_pushboolean(mp_lua, _value);
    lua_settable(mp_lua, -3);
}

void LuaTopStackTable::setStringValue(const char * _key, const char * _value) const
{
    lua_pushstring(mp_lua, _key);
    lua_pushstring(mp_lua, _value);
    lua_settable(mp_lua, -3);
}

void LuaTopStackTable::setNullValue(const char * _key) const
{
    lua_pushstring(mp_lua, _key);
    lua_pushnil(mp_lua);
    lua_settable(mp_lua, -3);
}
