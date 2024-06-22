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

#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteOptionsApi.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_metatable_sprite[] = "sol.Sprite";

struct Self : LuaUserData<Self, gc_metatable_sprite>
{
    Sprite * sprite;
    const Workspace * workspace;
};

// 1 self
// 2 path
// 3 options (optional)
int luaApi_LoadFromFile(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * path = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, path != nullptr, 2, "path expected");
    SpriteOptions options;
    tryGetSpriteOptions(_lua, 3, options);
    bool result = self->sprite->loadFromFile(self->workspace->getResourceFullPath(path), options);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace name

void Sol2D::Lua::pushSpriteApi(lua_State * _lua, const Workspace & _workspace, Sprite & _sprite)
{
    Self * self = Self::pushUserData(_lua);
    self->sprite = &_sprite;
    self->workspace = &_workspace;
    if(Self::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "loadFromFile", luaApi_LoadFromFile },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

bool Sol2D::Lua::tryGetSprite(lua_State * _lua, int _idx, Sprite ** _spire)
{
    Self * self = Self::getUserData(_lua, _idx);
    if(self)
    {
        *_spire = self->sprite;
        return true;
    }
    return false;
}
