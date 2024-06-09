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

    lua_State * getLua() const
    {
        return mp_lua;
    }

    void setValueFromTop(const char * _key);
    void setIntegerValue(const char * _key, lua_Integer _value) const;
    void setNumberValue(const char * _key, lua_Number _value) const;
    void setBooleanValue(const char * _key, bool _value) const;
    void setStringValue(const char * _key, const char * _value) const;
    void setNullValue(const char * _key) const;

    void setStringValue(const char * _key, const std::string & _value) const
    {
        setStringValue(_key, _value.c_str());
    }

private:
    lua_State * mp_lua;
};

} // namespace Sol2D::Lua::Aux
