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

#include <Sol2D/Lua/LuaWorldApi.h>
#include <Sol2D/Lua/LuaSceneApi.h>
#include <Sol2D/Lua/LuaLarderApi.h>
#include <Sol2D/Lua/LuaAux.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

const char gc_metatable_world[] = "sol.World";
const char gc_message_larder_key_expected[] = "a larder key expected";

struct Self : LuaUserData<Self, gc_metatable_world>
{
    World * world;
    const Workspace * workspace;
};

// 1 self
// 2 name
int luaApi_CreateScene(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * name = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, name != nullptr, 2, "the scene name expected");
    pushSceneApi(_lua, *self->workspace, self->world->createScene(name));
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateLarder(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_larder_key_expected);
    Larder & larder = self->world->createLarder(key);
    pushLarderApi(_lua, *self->workspace, larder);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetLarder(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_larder_key_expected);
    Larder * larder = self->world->getLarder(key);
    if(larder)
        pushLarderApi(_lua, *self->workspace, *larder);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_DeleteLarder(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_larder_key_expected);
    lua_pushboolean(_lua, self->world->deleteLarder(key));
    return 1;
}

} // namespace

void Sol2D::Lua::pushWorldApi(lua_State * _lua, const Workspace & _workspace, World & _world)
{
    Self * self = Self::pushUserData(_lua);
    self->world = &_world;
    self->workspace = &_workspace;
    if(Self::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "createScene", luaApi_CreateScene },
            { "createLarder", luaApi_CreateLarder },
            { "getLarder", luaApi_GetLarder },
            { "deleteLarder", luaApi_DeleteLarder },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
