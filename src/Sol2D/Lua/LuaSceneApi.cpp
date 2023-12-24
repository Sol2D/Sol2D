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

#include <Sol2D/Lua/LuaSceneApi.h>
#include <Sol2D/Lua/LuaBodyPrototypeApi.h>
#include <Sol2D/Lua/LuaLevelApi.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

const char * gc_mtable_scene = "sol.Scene";

void luaPushSceneApiMetatableOntoStack(lua_State * _lua);
int luaApi_LoadLevelFromTmx(lua_State * _lua);
int luaApi_CreateBodyPrototype(lua_State * _lua);

struct Self final
{
    Self(Scene & _scene, const Sol2D::Workspace & _workspace) :
        scene(_scene),
        workspace(_workspace)
    {
    }

    Scene & scene;
    const Sol2D::Workspace & workspace;
};

void luaPushSceneApiMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_mtable_scene) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "loadLevelFromTmx", luaApi_LoadLevelFromTmx },
        { "createBodyPrototype", luaApi_CreateBodyPrototype },
        { nullptr, nullptr }
    };

    luaL_newmetatable(_lua, gc_mtable_scene);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
    luaL_setfuncs(_lua, funcs, 0);
}

// 1 self
// 2 *.tmx file path
int luaApi_LoadLevelFromTmx(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_scene));
    const char * tmx_file = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, tmx_file != nullptr, 2, "the TMX file path expected");
    bool result = self->scene.loadLevelFromTmx(self->workspace.toAbsolutePath(tmx_file));
    lua_pushboolean(_lua, result);
    Level * level = self->scene.getLevel();
    luaCreateLevelApiAndPushOntoStack(_lua, self->workspace, *level);
    return 1;
}

// 1 self
int luaApi_CreateBodyPrototype(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_scene));
    luaPushBodyPrototypeApiOntoStack(_lua, self->workspace, self->scene.createBodyPrototype());
    return 1;
}

} // namespace

void Sol2D::Lua::luaPushSceneApiOntoStack(lua_State * _lua, const Workspace & _workspace, Scene & _scene)
{
    void * self = lua_newuserdata(_lua, sizeof(Self));
    new(self) Self(_scene, _workspace);
    luaPushSceneApiMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
}
