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

#include <Sol2D/Lua/LuaAux.h>

using namespace Sol2D::Lua;

void Sol2D::Lua::executeScript(lua_State * _lua, const Workspace & _workspace, const std::filesystem::path & _path)
{
    std::filesystem::path path = _workspace.toAbsolutePath(_path);
    if(luaL_loadfile(_lua, path.c_str()) == LUA_OK)
    {
        if(lua_pcall(_lua, 0, LUA_MULTRET, 0) != LUA_OK)
            _workspace.getMainLogger().error(lua_tostring(_lua, -1));
    }
    else
    {
        _workspace.getMainLogger().error("Unable to load file \"{0}\"", path.c_str());
    }
}

void Sol2D::Lua::executeScriptWithContext(lua_State * _lua, const Workspace & _workspace, const std::filesystem::path & _path)
{
    const char global_name[] = "self";
    lua_setglobal(_lua, global_name);
    executeScript(_lua, _workspace, _path);
    lua_pushnil(_lua);
    lua_setglobal(_lua, global_name);
}

MetatablePushResult Sol2D::Lua::pushMetatable(lua_State * _lua, const char * _name)
{
    if(luaL_newmetatable(_lua, _name) != 0)
    {
        lua_pushstring(_lua, "__index");
        lua_pushvalue(_lua, -2);
        lua_settable(_lua, -3);
        return MetatablePushResult::Created;
    }
    return MetatablePushResult::Loaded;
}

void Sol2D::Lua::pushTableFromRegistry(lua_State * _lua, void * _key)
{
    auto load = [_lua, _key]() {
        lua_pushlightuserdata(_lua, _key);
        if(lua_gettable(_lua, LUA_REGISTRYINDEX) == LUA_TTABLE)
            return true;
        lua_pop(_lua, 1);
        return false;
    };

    if(!load())
    {
        lua_pushlightuserdata(_lua, _key);
        lua_newtable(_lua);
        lua_settable(_lua, LUA_REGISTRYINDEX);
        load();
    }
}

bool LuaTable::tryGetNumber(const char * _key, lua_Number * _value) const
{
    lua_pushstring(mp_lua, _key);
    lua_gettable(mp_lua, m_idx);
    bool result = lua_isnumber(mp_lua, -1);
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
    lua_gettable(mp_lua, m_idx);
    bool result = lua_isboolean(mp_lua, -1);
    if(result)
        *_value = lua_toboolean(mp_lua, -1);
    lua_pop(mp_lua, 1);
    return result;
}

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


void WeakRegistryStorage::save(void * _key, int _idx)
{
    int object_abs_idx = lua_absindex(mp_lua, _idx);
    if(lua_rawgetp(mp_lua, LUA_REGISTRYINDEX, mp_storage_key) != LUA_TTABLE)
    {
        lua_pop(mp_lua, 1);
        LuaTopStackTable table = LuaTopStackTable::pushNew(mp_lua);
        table.setStringValue("__mode", "v");
        lua_pushvalue(mp_lua, -1);
        lua_rawsetp(mp_lua, LUA_REGISTRYINDEX, mp_storage_key);
    }
    lua_pushvalue(mp_lua, object_abs_idx);
    lua_rawsetp(mp_lua, -2, _key);
    lua_pop(mp_lua, 1);
}

bool WeakRegistryStorage::tryGet(void * _key, int _expected_type)
{
    if(lua_rawgetp(mp_lua, LUA_REGISTRYINDEX, mp_storage_key) != LUA_TTABLE)
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
