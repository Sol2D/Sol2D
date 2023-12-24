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

#include <Sol2D/Lua/LuaBodyApi.h>
#include <filesystem>
#include <new>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace fs = std::filesystem;

namespace {

const char * gc_mtable_body = "sol.Body";

struct Self
{
    explicit Self(Body & _body) :
        body(_body)
    {
    }

    Body & body;
};

void luaPushBodyApiMetatableOntoStack(lua_State * _lua);
int luaApi_Move(lua_State * _lua);
int luaApi_SetPosition(lua_State * _lua);
int luaApi_SetActiveSprite(lua_State * _lua);
int luaApi_Flip(lua_State * _lua);
int luaApi_SetLayer(lua_State * _lua);

void luaPushBodyApiMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_mtable_body) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "move", luaApi_Move },
        { "setPosition", luaApi_SetPosition },
        { "setActiveSprite", luaApi_SetActiveSprite },
        { "flip", luaApi_Flip },
        { "setLayer", luaApi_SetLayer },
        { nullptr, nullptr }
    };

    luaL_newmetatable(_lua, gc_mtable_body);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
    luaL_setfuncs(_lua, funcs, 0);
}

// 1 self
// 2 delta x
// 3 delta y
int luaApi_Move(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_body));
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "delta X expected");
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, "delta Y expected");
    int dx = static_cast<int>(lua_tointeger(_lua, 2));
    int dy = static_cast<int>(lua_tointeger(_lua, 3));
    self->body.move(dx, dy);
    return 0;
}

// 1 self
// 2 x
// 3 y
int luaApi_SetPosition(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_body));
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "the X coordinate expected");
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, "the Y coordinate expected");
    int x = static_cast<int>(lua_tointeger(_lua, 2));
    int y = static_cast<int>(lua_tointeger(_lua, 3));
    self->body.setPosition(x, y);
    return 0;
}

// 1 self
// 2 sprite index
int luaApi_SetActiveSprite(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_body));
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "sprite index expected");
    lua_Integer idx = lua_tonumber(_lua, 2);
    if(idx == 0)
        return luaL_argerror(_lua, 2, "the sprite index must be greater than 0");
    self->body.setActiveSprite(static_cast<size_t>(idx - 1));
    return 0;
}

// 1 self
// 2 horizontal
// 3 vertical
int luaApi_Flip(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_body));
    luaL_argcheck(_lua, lua_isboolean(_lua, 2), 2, "horizontal flip expected");
    luaL_argcheck(_lua, lua_isboolean(_lua, 3), 3, "vertical flip expected");
    self->body.flip(lua_toboolean(_lua, 2), lua_toboolean(_lua, 3));
    return 0;
}

// 1 self
// 2 layer name
int luaApi_SetLayer(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_body));
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, "layer name expected");
    self->body.setLayer(lua_tostring(_lua, 2));
    return 0;
}

} // namespace

void Sol2D::Lua::luaPushBodyApiOntoStack(lua_State * _lua, Body & _body)
{
    void * self = lua_newuserdata(_lua, sizeof(Self));
    new(self) Self(_body);
    luaPushBodyApiMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
}

bool Sol2D::Lua::luaTryGetBody(lua_State * _lua, int _idx, Sol2D::Body ** _body)
{
    if(luaL_testudata(_lua, _idx, gc_mtable_body) == nullptr)
        return false;
    Self * self = static_cast<Self *>(lua_touserdata(_lua, _idx));
    *_body = &self->body;
    return true;
}
