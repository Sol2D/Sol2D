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

#include <Sol2D/Lua/LuaRectApi.h>
#include <Sol2D/Lua/LuaAux.h>

using namespace Sol2D::Lua;

namespace {

const char * gc_mtable_rect = "sol.Rect";
const char * gc_key_x = "x";
const char * gc_key_y = "y";
const char * gc_key_width = "width";
const char * gc_key_height = "height";

void luaPushRectApiMetatableOntoStack(lua_State * _lua);
int luaApi_New(lua_State * _lua);
void luaCreateRect(lua_State * _lua, float _x, float _y, float _width, float _height);

void luaPushRectApiMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_mtable_rect) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "new", luaApi_New },
        { nullptr, nullptr }
    };

    luaL_newmetatable(_lua, gc_mtable_rect);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
    luaL_setfuncs(_lua, funcs, 0);
}

// 1 x
// 2 y
// 3 width
// 4 height
int luaApi_New(lua_State * _lua)
{
    luaL_argcheck(_lua, lua_isinteger(_lua, 1), 1, "x coordinate expected to be integer");
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "y coordinate expected to be integer");
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, "width expected to be integer");
    luaL_argcheck(_lua, lua_isinteger(_lua, 4), 4, "height expected to be integer");
    luaCreateRect(
        _lua,
        lua_tointeger(_lua, 1),
        lua_tointeger(_lua, 2),
        lua_tointeger(_lua, 3),
        lua_tointeger(_lua, 4)
    );
    return 1;
}

void luaCreateRect(lua_State * _lua, float _x, float _y, float _width, float _height)
{
    lua_newtable(_lua);
    lua_pushstring(_lua, gc_key_x);
    lua_pushinteger(_lua, _x);
    lua_settable(_lua, -3);
    lua_pushstring(_lua, gc_key_y);
    lua_pushinteger(_lua, _y);
    lua_settable(_lua, -3);
    lua_pushstring(_lua, gc_key_width);
    lua_pushinteger(_lua, _width);
    lua_settable(_lua, -3);
    lua_pushstring(_lua, gc_key_height);
    lua_pushinteger(_lua, _height);
    lua_settable(_lua, -3);
}

} // namespace

void Sol2D::Lua::luaPushRectApiOntoStack(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    luaPushRectApiMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
}


void Sol2D::Lua::luaPushRectOntoStack(lua_State * _lua, float _x, float _y, float _width, float _height)
{
    luaCreateRect(_lua, _x, _y, _width, _height);
}

bool Sol2D::Lua::luaTryGetRect(lua_State * _lua, int _idx, float * _x, float * _y, float * _width, float * _height)
{
    if(!lua_istable(_lua, _idx))
    {
        return false;
    }
    float x, y, width, height;
    if(
        luaTryGetFloat(_lua, _idx, "x", &x) &&
        luaTryGetFloat(_lua, _idx, "x", &y) &&
        luaTryGetFloat(_lua, _idx, "width", &width) &&
        luaTryGetFloat(_lua, _idx, "height", &height)
    )
    {
        *_x = x;
        *_y = y;
        *_width = width;
        *_height = height;
        return true;
    }
    return false;
}
