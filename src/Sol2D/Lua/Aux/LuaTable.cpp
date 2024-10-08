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

using namespace Sol2D::Lua::Aux;

bool LuaTable::tryGetNumber(const char * _key, lua_Number * _value) const
{
    lua_pushstring(mp_lua, _key);
    bool result = lua_gettable(mp_lua, m_idx) == LUA_TNUMBER;
    if(result)
        *_value = lua_tonumber(mp_lua, -1);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetInteger(const char * _key, lua_Integer * _value) const
{
    lua_pushstring(mp_lua, _key);
    lua_gettable(mp_lua, m_idx);
    bool result = lua_isinteger(mp_lua, -1);
    if(result)
        *_value = lua_tointeger(mp_lua, -1);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetBoolean(const char * _key, bool * _value) const
{
    lua_pushstring(mp_lua, _key);
    bool result =  lua_gettable(mp_lua, m_idx) == LUA_TBOOLEAN;
    if(result)
        *_value = lua_toboolean(mp_lua, -1);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetString(const char * _key, std::string & _value) const
{
    lua_pushstring(mp_lua, _key);
    bool result = lua_gettable(mp_lua, m_idx) == LUA_TSTRING;
    if(result)
        _value = lua_tostring(mp_lua, -1);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetValue(const char * _key) const
{
    lua_pushstring(mp_lua, _key);
    if(lua_gettable(mp_lua, m_idx) == LUA_TNIL)
    {
        lua_pop(mp_lua, 1);
        return false;
    }
    return true;
}

void LuaTable::setValueFromTop(const char * _key)
{
    lua_pushstring(mp_lua, _key);
    int top = lua_gettop(mp_lua);
    lua_insert(mp_lua, top -1);
    lua_settable(mp_lua, m_idx);
}

void LuaTable::setIntegerValue(const char * _key, lua_Integer _value)
{
    lua_pushstring(mp_lua, _key);
    lua_pushinteger(mp_lua, _value);
    lua_settable(mp_lua, m_idx);
}

void LuaTable::setNumberValue(const char * _key, lua_Number _value)
{
    lua_pushstring(mp_lua, _key);
    lua_pushnumber(mp_lua, _value);
    lua_settable(mp_lua, m_idx);
}

void LuaTable::setBooleanValue(const char * _key, bool _value)
{
    lua_pushstring(mp_lua, _key);
    lua_pushboolean(mp_lua, _value);
    lua_settable(mp_lua, m_idx);
}

void LuaTable::setStringValue(const char * _key, const char * _value)
{
    lua_pushstring(mp_lua, _key);
    lua_pushstring(mp_lua, _value);
    lua_settable(mp_lua, m_idx);
}

void LuaTable::setNullValue(const char * _key)
{
    lua_pushstring(mp_lua, _key);
    lua_pushnil(mp_lua);
    lua_settable(mp_lua, m_idx);
}
