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

#pragma once

#include <Sol2D/Workspace.h>
#include <lua.hpp>
#include <string>

namespace Sol2D::Lua {

void executeScript(lua_State * _lua, const Workspace & _workspace, const std::filesystem::path & _path);
void executeScriptWithContext(lua_State * _lua, const Workspace & _workspace, const std::filesystem::path & _path);

enum class MetatablePushResult
{
    Loaded,
    Created
};

MetatablePushResult pushMetatable(lua_State * _lua, const char * _name);
void pushTableFromRegistry(lua_State * _lua, void * _key);

template<typename UserData, const char metatable[]>
struct LuaUserData
{
    static UserData * pushUserData(lua_State * _lua)
    {
        auto * data = static_cast<UserData *>(lua_newuserdata(_lua, sizeof(UserData)));
        return data;
    }

    static MetatablePushResult pushMetatable(lua_State * _lua)
    {
        return Sol2D::Lua::pushMetatable(_lua, metatable);
    }

    static UserData * getUserData(lua_State * _lua, int _idx)
    {
        return static_cast<UserData *>(luaL_checkudata(_lua, _idx, metatable));
    }
};

class LuaTopStackTable final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(LuaTopStackTable)

    explicit LuaTopStackTable(lua_State * _lua) :
        mp_lua(_lua)
    {
    }

    static LuaTopStackTable pushNew(lua_State * _lua)
    {
        lua_newtable(_lua);
        return LuaTopStackTable(_lua);
    }

    void setValueFromTop(const char * _key);
    void setIntegerValue(const char * _key, lua_Integer _value) const;
    void setNumberValue(const char * _key, lua_Number _value) const;
    void setStringValue(const char * _key, const char * _value) const;
    void setNullValue(const char * _key) const;

    void setStringValue(const char * _key, const std::string & _value) const
    {
        setStringValue(_key, _value.c_str());
    }

private:
    lua_State * mp_lua;
};

class LuaTable final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(LuaTable)

    LuaTable(lua_State * _lua, int _idx) :
        mp_lua(_lua),
        m_idx(lua_absindex(_lua, _idx))
    {
    }

    bool tryGetNumber(const char * _key, lua_Number * _value) const;
    bool tryGetInteger(const char * _key, lua_Integer * _value) const;
    bool tryGetBoolean(const char * _key, bool * _value) const;

private:
    lua_State * mp_lua;
    int m_idx;
};

class WeakRegistryStorage
{
public:
    WeakRegistryStorage(lua_State * _lua, void * _storage_key) :
        mp_lua(_lua),
        mp_storage_key(_storage_key)
    {
    }

    void save(void * _key, int _idx);
    bool tryGet(void * _key, int _expected_type);

private:
    lua_State * mp_lua;
    void * mp_storage_key;
};

} // namespace Sol2D::Lua
