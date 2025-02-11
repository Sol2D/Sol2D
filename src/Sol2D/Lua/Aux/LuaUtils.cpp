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

#include <Sol2D/Lua/Aux/LuaUtils.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <lua.hpp>

namespace {

inline const char * __argToString(lua_State * _lua, int _idx)
{
    const char * str = nullptr;
    if(lua_isstring(_lua, _idx))
        str = lua_tostring(_lua, _idx);
    return str;
}

} // namespace

const char * Sol2D::Lua::argToString(lua_State * _lua, int _idx)
{
    return __argToString(_lua, _idx);
}

const char * Sol2D::Lua::argToStringOrError(lua_State * _lua, int _idx)
{
    const char * str = __argToString(_lua, _idx);
    luaL_argexpected(_lua, str, _idx, LuaTypeName::string);
    return str;
}
