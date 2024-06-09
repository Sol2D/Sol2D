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

#include <Sol2D/Lua/LuaSpriteSheetApi.h>
#include <Sol2D/Lua/LuaSpriteSheetOptionsApi.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_metatable_sprite_sheet[] = "sol.SpriteSheet";

struct Self : LuaUserData<Self, gc_metatable_sprite_sheet>
{
    const Workspace * workspace;
    SpriteSheet * sprite_sheet;
};

// 1 self
// 2 path
// 3 options
int luaApi_LoadFromFile(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, "path expected");
    const char * path = lua_tostring(_lua, 2);
    SpriteSheetOptions options;
    luaL_argcheck(_lua,  tryGetSpriteSheetOptions(_lua, 3, options), 3, "sprite sheet options expected");
    bool result = self->sprite_sheet->loadFromFile(self->workspace->toAbsolutePath(path), options);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace name

void Sol2D::Lua::pushSpriteSheetApi(lua_State * _lua, const Workspace & _workspace, SpriteSheet & _sprite_sheet)
{
    Self * self = Self::pushUserData(_lua);
    self->workspace = &_workspace;
    self->sprite_sheet = &_sprite_sheet;
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

bool Sol2D::Lua::tryGetSpriteSheet(lua_State * _lua, int _idx, SpriteSheet ** _spire_sheet)
{
    Self * self = Self::getUserData(_lua, _idx);
    if(self)
    {
        *_spire_sheet = self->sprite_sheet;
        return true;
    }
    return false;
}
