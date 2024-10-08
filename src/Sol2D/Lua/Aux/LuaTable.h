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

#include <Sol2D/Def.h>
#include <lua.hpp>
#include <string>

namespace Sol2D::Lua::Aux {

class LuaTable final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(LuaTable)

    explicit LuaTable(lua_State * _lua);
    LuaTable(lua_State * _lua, int _idx);

    static LuaTable pushNew(lua_State * _lua);

    lua_State * getLua() const;

    bool tryGetNumber(const char * _key, lua_Number * _value) const;
    bool tryGetInteger(const char * _key, lua_Integer * _value) const;
    bool tryGetBoolean(const char * _key, bool * _value) const;
    bool tryGetString(const char * _key, std::string & _value) const;
    bool tryGetValue(const char * _key) const;

    void setValueFromTop(const char * _key);
    void setIntegerValue(const char * _key, lua_Integer _value);
    void setNumberValue(const char * _key, lua_Number _value);
    void setBooleanValue(const char * _key, bool _value);
    void setStringValue(const char * _key, const char * _value);
    void setNullValue(const char * _key);
    void setStringValue(const char * _key, const std::string & _value);

private:
    lua_State * mp_lua;
    int m_idx;
};

inline LuaTable::LuaTable(lua_State * _lua) :
    LuaTable(_lua, -1)
{
}

inline LuaTable::LuaTable(lua_State * _lua, int _idx) :
    mp_lua(_lua),
    m_idx(lua_absindex(_lua, _idx))
{
}

inline LuaTable LuaTable::pushNew(lua_State * _lua)
{
    lua_newtable(_lua);
    return LuaTable(_lua);
}

inline lua_State * LuaTable::getLua() const
{
    return mp_lua;
}

inline void LuaTable::setStringValue(const char * _key, const std::string & _value)
{
    setStringValue(_key, _value.c_str());
}

} // namespace Sol2D::Lua::Aux
