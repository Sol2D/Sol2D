// Sol2D Game Engine
// Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
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
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaUtils.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    Self(const Workspace & _workplace, std::shared_ptr<SpriteSheet> & _sprite_sheet) :
        workspace(_workplace),
        sprite_sheet(_sprite_sheet)
    {
    }

    std::shared_ptr<SpriteSheet> getSpriteSheet(lua_State * _lua) const
    {
        std::shared_ptr<SpriteSheet> ptr = sprite_sheet.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::sprite_sheet_is_destroyed);
        return ptr;
    }

    const Workspace & workspace;
    std::weak_ptr<SpriteSheet> sprite_sheet;
};

using UserData = LuaUserData<Self, LuaTypeName::sprite_sheet>;

// 1 self
// 2 path
// 3 options
int luaApi_LoadFromFile(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * path = argToStringOrError(_lua, 2);
    SpriteSheetOptions options = {};
    luaL_argexpected(_lua,  tryGetSpriteSheetOptions(_lua, 3, options), 3, LuaTypeName::sprite_sheet_options);
    bool result = self->getSpriteSheet(_lua)->loadFromFile(self->workspace.getResourceFullPath(path), options);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace name

void Sol2D::Lua::pushSpriteSheetApi(lua_State * _lua, const Workspace & _workspace, std::shared_ptr<SpriteSheet> _sprite_sheet)
{
    UserData::pushUserData(_lua, _workspace, _sprite_sheet);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "loadFromFile", luaApi_LoadFromFile },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<SpriteSheet> Sol2D::Lua::tryGetSpriteSheet(lua_State * _lua, int _idx)
{
    Self * self = UserData::getUserData(_lua, _idx);
    return self ? self->sprite_sheet.lock() : nullptr;
}
