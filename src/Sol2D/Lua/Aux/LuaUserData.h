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
#include <cstring>
#include <vector>
#include <concepts>
#include <sstream>

namespace Sol2D::Lua::Aux {

struct LuaSelfBase
{
public:
    virtual ~LuaSelfBase() { }
};

template<std::derived_from<LuaSelfBase> LuaSelf, const char metatable[]>
struct LuaUserData
{
    template<typename ...CtorArgs>
    static LuaSelf * pushUserData(lua_State * _lua, CtorArgs && ... _ctor_args)
    {
        void * data = lua_newuserdata(_lua, sizeof(LuaSelf));
        std::memset(data, 0, sizeof(LuaSelf));
        return new(data) LuaSelf(std::forward<CtorArgs>(_ctor_args)...);
    }

    static MetatablePushResult pushMetatable(lua_State * _lua)
    {
        return Sol2D::Lua::Aux::pushMetatable(_lua, metatable);
    }

    static LuaSelf * getUserData(lua_State * _lua, int _idx)
    {
        return static_cast<LuaSelf *>(luaL_checkudata(_lua, _idx, metatable));
    }

    static LuaSelf * tryGetUserData(lua_State * _lua, int _idx)
    {
        return static_cast<LuaSelf *>(luaL_testudata(_lua, _idx, metatable));
    }

    static int luaGC(lua_State * _lua)
    {
        LuaSelf * self = getUserData(_lua, 1);
        self->~LuaSelf();
        return 0;
    }
};

template<std::derived_from<LuaSelfBase> LuaSelf>
LuaSelf * tryGetLuaUserData(lua_State * _lua, int _idx, const std::vector<const char *> & _metatables)
{
    void * user_data = lua_touserdata(_lua, _idx);
    if(!user_data || !lua_getmetatable(_lua, _idx))
    {
        return nullptr;
    }
    LuaSelf * self = nullptr;
    for(const char * metatable : _metatables)
    {
        if(luaL_getmetatable(_lua, metatable) && lua_rawequal(_lua, -1, -2))
        {
            self = static_cast<LuaSelf *>(user_data);
        }
        lua_pop(_lua, 1);
        if(self)
        {
            break;
        }
    }
    lua_pop(_lua, 1);
    return self;
}

template<std::derived_from<LuaSelfBase> LuaSelf>
LuaSelf * getLuaUserData(lua_State * _lua, int _idx, const std::vector<const char *> & _metatables)
{
    LuaSelf * self = tryGetLuaUserData<LuaSelf>(_lua, _idx, _metatables);
    if(!self)
    {
        std::stringstream ss;
        ss << "one of the following type is expected: ";
        for(size_t i = 0; i < _metatables.size(); ++i)
        {
            if(i > 0)
                ss << ", ";
            ss << _metatables[i];
        }
        luaL_argerror(_lua, _idx, ss.str().c_str());
    }
    return self;
}

} // namespace Sol2D::Lua::Aux
