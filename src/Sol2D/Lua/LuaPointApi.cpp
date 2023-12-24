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
#include <Sol2D/Lua/LuaAux.h>

using namespace Sol2D::Lua;

namespace {

const char * gc_mtable_point = "sol.Point";
const char * gc_key_x = "x";
const char * gc_key_y = "y";

void luaPushPointApiMetatableOntoStack(lua_State * _lua);
int luaApi_New(lua_State * _lua);
void luaCreatePoint(lua_State * _lua, float _x, float _y);

void luaPushPointApiMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_mtable_point) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "new", luaApi_New },
        { nullptr, nullptr }
    };

    luaL_newmetatable(_lua, gc_mtable_point);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
    luaL_setfuncs(_lua, funcs, 0);
}

// 1 x
// 2 y
int luaApi_New(lua_State * _lua)
{
    luaL_argcheck(_lua, lua_isinteger(_lua, 1), 1, "x coordinate expected to be integer");
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "y coordinate expected to be integer");
    luaCreatePoint(_lua, lua_tointeger(_lua, 1), lua_tointeger(_lua, 2));
    return 1;
}

void luaCreatePoint(lua_State * _lua, float _x, float _y)
{
    lua_newtable(_lua);
    lua_pushstring(_lua, gc_key_x);
    lua_pushinteger(_lua, _x);
    lua_settable(_lua, -3);
    lua_pushstring(_lua, gc_key_y);
    lua_pushinteger(_lua, _y);
    lua_settable(_lua, -3);
}

} // namespace

void Sol2D::Lua::luaPushPointApiOntoStack(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    luaPushPointApiMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
}


void Sol2D::Lua::luaPushPointOntoStack(lua_State * _lua, float _x, float _y)
{
    luaCreatePoint(_lua, _x, _y);
}

bool Sol2D::Lua::luaTryGetPoint(lua_State * _lua, int _idx, float * _x, float * _y)
{
    if(!lua_istable(_lua, _idx))
    {
        return false;
    }
    float x, y;
    if(luaTryGetFloat(_lua, _idx, gc_key_x, &x) && luaTryGetFloat(_lua, _idx, gc_key_y, &y))
    {
        *_x = x;
        *_y = y;
        return true;
    }
    return false;
}
