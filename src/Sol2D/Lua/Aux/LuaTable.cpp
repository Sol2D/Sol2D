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

#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaRectApi.h>
#include <Sol2D/Lua/LuaSizeApi.h>
#include <Sol2D/Lua/LuaColorApi.h>
#include <Sol2D/Lua/LuaDimensionApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D::Lua::Aux;

bool LuaTable::tryGetBoolean(const char * _key, std::optional<bool> _value) const
{
    bool value;
    if(tryGetBoolean(_key, &value))
    {
        _value = value;
        return true;
    }
    return false;
}

bool LuaTable::tryGetBoolean(const char * _key, bool * _value) const
{
    bool result = lua_getfield(mp_lua, m_idx, _key) == LUA_TBOOLEAN;
    if(result)
        *_value = lua_toboolean(mp_lua, -1);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetString(const char * _key, std::optional<std::string> & _value) const
{
    std::string value;
    if(tryGetString(_key, value))
    {
        _value = value;
        return true;
    }
    return false;
}

bool LuaTable::tryGetString(const char * _key, std::string & _value) const
{
    bool result = lua_getfield(mp_lua, m_idx, _key) == LUA_TSTRING;
    if(result)
        _value = lua_tostring(mp_lua, -1);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetPoint(const char * _key, std::optional<Point> & _value)
{
    Point point;
    if(tryGetPoint(_key, point))
    {
        _value = point;
        return true;
    }
    return false;
}

bool LuaTable::tryGetPoint(const char * _key, Point & _value)
{
    if(!tryGetValue(_key))
        return false;
    bool result = Lua::tryGetPoint(mp_lua, -1, _value);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetSize(const char * _key, std::optional<Size> & _value)
{
    Size size;
    if(tryGetSize(_key, size))
    {
        _value = size;
        return true;
    }
    return false;
}

bool LuaTable::tryGetSize(const char * _key, Size & _value)
{
    if(!tryGetValue(_key))
        return false;
    bool result = Lua::tryGetSize(mp_lua, -1, _value);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetRect(const char * _key, std::optional<Rect> & _value)
{
    Rect rect;
    if(tryGetRect(_key, rect))
    {
        _value = rect;
        return true;
    }
    return false;
}

bool LuaTable::tryGetRect(const char * _key, Rect & _value)
{
    if(!tryGetValue(_key))
        return false;
    bool result = Lua::tryGetRect(mp_lua, -1, _value);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetColor(const char * _key, std::optional<Color> & _value)
{
    Color color;
    if(tryGetColor(_key, color))
    {
        _value = color;
        return true;
    }
    return false;
}

bool LuaTable::tryGetColor(const char * _key, Color & _value)
{
    if(!tryGetValue(_key))
        return false;
    bool result = Lua::tryGetColor(mp_lua, -1, _value);
    lua_pop(mp_lua, 1);
    return result;
}

bool LuaTable::tryGetValue(const char * _key) const
{
    if(lua_getfield(mp_lua, m_idx, _key) == LUA_TNIL)
    {
        lua_pop(mp_lua, 1);
        return false;
    }
    return true;
}

void LuaTable::setValueFromTop(const char * _key)
{
    lua_setfield(mp_lua, m_idx, _key);
}

void LuaTable::setIntegerValue(const char * _key, lua_Integer _value)
{
    lua_pushinteger(mp_lua, _value);
    lua_setfield(mp_lua, m_idx, _key);
}

void LuaTable::setNumberValue(const char * _key, lua_Number _value)
{
    lua_pushnumber(mp_lua, _value);
    lua_setfield(mp_lua, m_idx, _key);
}

void LuaTable::setBooleanValue(const char * _key, bool _value)
{
    lua_pushboolean(mp_lua, _value);
    lua_setfield(mp_lua, m_idx, _key);
}

void LuaTable::setStringValue(const char * _key, const char * _value)
{
    lua_pushstring(mp_lua, _value);
    lua_setfield(mp_lua, m_idx, _key);
}

void LuaTable::setPointValue(const char * _key, const Point & _point)
{
    pushPoint(mp_lua, _point.x, _point.y);
    lua_setfield(mp_lua, m_idx, _key);
}

void LuaTable::setNullValue(const char * _key)
{
    lua_pushnil(mp_lua);
    lua_setfield(mp_lua, m_idx, _key);
}
