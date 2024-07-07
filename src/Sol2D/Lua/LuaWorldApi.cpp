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
#include <Sol2D/Lua/LuaFormApi.h>
#include <Sol2D/Lua/LuaLarderApi.h>
#include <Sol2D/Lua/LuaFragmentApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_message_larder_key_expected[] = "a larder key expected";
const char gc_message_fragment_id_expected[] = "a fragment ID expected";
const char gc_message_scene_name_expected[] = "a scene name expected";

struct Self : LuaSelfBase
{
    World * world;
    const Workspace * workspace;
};

using UserData = LuaUserData<Self, LuaTypeName::world>;

// 1 self
// 2 name
int luaApi_CreateScene(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * name = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, name != nullptr, 2, "the scene name expected");
    pushSceneApi(_lua, *self->workspace, self->world->createScene(name));
    return 1;
}

// 1 self
// 2 name
int luaApi_CreateForm(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * name = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, name != nullptr, 2, "the scene name expected");
    pushFormApi(_lua, *self->workspace, self->world->createForm(name));
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateLarder(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
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
    Self * self = UserData::getUserData(_lua, 1);
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
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_larder_key_expected);
    lua_pushboolean(_lua, self->world->deleteLarder(key));
    return 1;
}

// 1 self
// 2 fragment
int luaApi_CreateFragment(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    Fragment fragment;
    luaL_argcheck(_lua, tryGetFragment(_lua, 2, fragment), 2, "");
    lua_pushinteger(_lua, self->world->createFragment(fragment));
    return 1;
}

// 1 self
// 2 fragment ID
// 3 fragment
int luaApi_UpdateFragment(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_fragment_id_expected);
    FragmentID fragment_id = static_cast<FragmentID>(lua_tointeger(_lua, 2));
    Fragment fragment;
    luaL_argcheck(_lua, tryGetFragment(_lua, 3, fragment), 3, "");
    lua_pushboolean(_lua, self->world->updateFragment(fragment_id, fragment));
    return 1;
}

// 1 self
// 2 fragment ID
int luaApi_GetFragment(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_fragment_id_expected);
    FragmentID fragment_id = static_cast<FragmentID>(lua_tointeger(_lua, 2));
    const Fragment * fragment = self->world->getFragment(fragment_id);
    if(fragment)
        pushFragment(_lua, *fragment);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 fragment ID
int luaApi_DeleteFragment(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_fragment_id_expected);
    FragmentID fragment_id = static_cast<FragmentID>(lua_tointeger(_lua, 2));
    lua_pushboolean(_lua, self->world->deleteFragment(fragment_id));
    return 1;
}

// 1 self
// 2 fragment ID
// 3 scene key
int luaApi_BindFragment(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_fragment_id_expected);
    FragmentID fragment_id = static_cast<FragmentID>(lua_tointeger(_lua, 2));
    luaL_argcheck(_lua, lua_isstring(_lua, 3), 3, gc_message_scene_name_expected);
    std::string scene_name(lua_tostring(_lua, 3));
    lua_pushboolean(_lua, self->world->bindFragment(fragment_id, scene_name));
    return 1;
}

} // namespace

void Sol2D::Lua::pushWorldApi(lua_State * _lua, const Workspace & _workspace, World & _world)
{
    Self * self = UserData::pushUserData(_lua);
    self->world = &_world;
    self->workspace = &_workspace;
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "__gc", UserData::luaGC },
            { "createScene", luaApi_CreateScene },
            // TODO: getScene
            // TODO: deleteScene
            { "createForm", luaApi_CreateForm },
            // TODO: getForm
            // TODO: deleteForm
            { "createLarder", luaApi_CreateLarder },
            { "getLarder", luaApi_GetLarder },
            { "deleteLarder", luaApi_DeleteLarder },
            { "createFragment", luaApi_CreateFragment },
            { "updateFragment", luaApi_UpdateFragment },
            { "getFragment", luaApi_GetFragment },
            { "deleteFragment", luaApi_DeleteFragment },
            { "bindFragment", luaApi_BindFragment },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
