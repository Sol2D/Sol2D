// Sol2D Game Engine
// Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
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
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D::Lua;

bool LuaTableApi::tryGetBoolean(const char * _key, std::optional<bool> _value) const
{
    bool value;
    if(tryGetBoolean(_key, &value))
    {
        _value = value;
        return true;
    }
    return false;
}

bool LuaTableApi::tryGetBoolean(const char * _key, bool * _value) const
{
    bool result = lua_getfield(m_lua, m_idx, _key) == LUA_TBOOLEAN;
    if(result)
        *_value = lua_toboolean(m_lua, -1);
    lua_pop(m_lua, 1);
    return result;
}

bool LuaTableApi::tryGetString(const char * _key, std::optional<std::string> & _value) const
{
    std::string value;
    if(tryGetString(_key, value))
    {
        _value = value;
        return true;
    }
    return false;
}

bool LuaTableApi::tryGetString(const char * _key, std::string & _value) const
{
    bool result = lua_getfield(m_lua, m_idx, _key) == LUA_TSTRING;
    if(result)
        _value = lua_tostring(m_lua, -1);
    lua_pop(m_lua, 1);
    return result;
}

bool LuaTableApi::tryGetPoint(const char * _key, std::optional<SDL_FPoint> & _value)
{
    SDL_FPoint point;
    if(tryGetPoint(_key, point))
    {
        _value = point;
        return true;
    }
    return false;
}

bool LuaTableApi::tryGetPoint(const char * _key, SDL_FPoint & _value)
{
    if(!tryGetTable(_key))
        return false;
    bool result = Lua::tryGetPoint(m_lua, -1, _value);
    lua_pop(m_lua, 1);
    return result;
}

bool LuaTableApi::tryGetSize(const char * _key, std::optional<FSize> & _value)
{
    FSize size;
    if(tryGetSize(_key, size))
    {
        _value = size;
        return true;
    }
    return false;
}

bool LuaTableApi::tryGetSize(const char * _key, FSize & _value)
{
    if(!tryGetTable(_key))
        return false;
    bool result = Lua::tryGetSize(m_lua, -1, _value);
    lua_pop(m_lua, 1);
    return result;
}

bool LuaTableApi::tryGetRect(const char * _key, std::optional<SDL_FRect> & _value)
{
    SDL_FRect rect;
    if(tryGetRect(_key, rect))
    {
        _value = rect;
        return true;
    }
    return false;
}

bool LuaTableApi::tryGetRect(const char * _key, SDL_FRect & _value)
{
    if(!tryGetTable(_key))
        return false;
    bool result = Lua::tryGetRect(m_lua, -1, _value);
    lua_pop(m_lua, 1);
    return result;
}

bool LuaTableApi::tryGetColor(const char * _key, std::optional<SDL_FColor> & _value)
{
    SDL_FColor color;
    if(tryGetColor(_key, color))
    {
        _value = color;
        return true;
    }
    return false;
}

bool LuaTableApi::tryGetColor(const char * _key, SDL_FColor & _value)
{
    if(!tryGetTable(_key))
        return false;
    bool result = Lua::tryGetColor(m_lua, -1, _value);
    lua_pop(m_lua, 1);
    return result;
}

bool LuaTableApi::tryGetTable(const char * _key) const
{
    if(lua_getfield(m_lua, m_idx, _key) != LUA_TTABLE)
    {
        lua_pop(m_lua, 1);
        return false;
    }
    return true;
}

bool LuaTableApi::tryGetValue(const char * _key) const
{
    if(lua_getfield(m_lua, m_idx, _key) == LUA_TNIL)
    {
        lua_pop(m_lua, 1);
        return false;
    }
    return true;
}

void LuaTableApi::setValueFromTop(const char * _key)
{
    lua_setfield(m_lua, m_idx, _key);
}

void LuaTableApi::setIntegerValue(const char * _key, lua_Integer _value)
{
    lua_pushinteger(m_lua, _value);
    lua_setfield(m_lua, m_idx, _key);
}

void LuaTableApi::setNumberValue(const char * _key, lua_Number _value)
{
    lua_pushnumber(m_lua, _value);
    lua_setfield(m_lua, m_idx, _key);
}

void LuaTableApi::setBooleanValue(const char * _key, bool _value)
{
    lua_pushboolean(m_lua, _value);
    lua_setfield(m_lua, m_idx, _key);
}

void LuaTableApi::setStringValue(const char * _key, const char * _value)
{
    lua_pushstring(m_lua, _value);
    lua_setfield(m_lua, m_idx, _key);
}

void LuaTableApi::setPointValue(const char * _key, const SDL_FPoint & _point)
{
    pushPoint(m_lua, _point.x, _point.y);
    lua_setfield(m_lua, m_idx, _key);
}

void LuaTableApi::setNullValue(const char * _key)
{
    lua_pushnil(m_lua);
    lua_setfield(m_lua, m_idx, _key);
}
