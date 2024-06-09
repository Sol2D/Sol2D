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

#include <lua.hpp>
#include <Sol2D/Lua/Aux/LuaMetatable.h>

namespace Sol2D::Lua::Aux {

template<typename UserData, const char metatable[]>
struct LuaUserData
{
    static UserData * pushUserData(lua_State * _lua)
    {
        return static_cast<UserData *>(lua_newuserdata(_lua, sizeof(UserData)));
    }

    static MetatablePushResult pushMetatable(lua_State * _lua)
    {
        return Sol2D::Lua::Aux::pushMetatable(_lua, metatable);
    }

    static UserData * getUserData(lua_State * _lua, int _idx)
    {
        return static_cast<UserData *>(luaL_checkudata(_lua, _idx, metatable));
    }
};

} // namespace Sol2D::Lua::Aux
