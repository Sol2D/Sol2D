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

#include <Sol2D/Lua/Aux/LuaMetatable.h>

using namespace Sol2D::Lua::Aux;

MetatablePushResult Sol2D::Lua::Aux::pushMetatable(lua_State * _lua, const char * _name)
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

void Sol2D::Lua::Aux::pushTableFromRegistry(lua_State * _lua, void * _key)
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
