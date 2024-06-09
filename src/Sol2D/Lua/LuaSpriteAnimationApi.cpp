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

#include <Sol2D/Lua/LuaSpriteAnimationApi.h>
#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteSheetApi.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_metatable_sprite_animation[] = "sol.SpriteAnimation";
const char gc_message_duration_expected[] = "duration expected";
const char gc_message_sprite_sheet_expected[] = "sprite sheet expected";

struct Self : LuaUserData<Self, gc_metatable_sprite_animation>
{
    SpriteAnimation * animation;
};

// 1 self
// 2 duration
// 3 sprite
int luaApi_addFrameFromSprite(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_duration_expected);
    uint32_t duration = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    Sprite * sprite;
    luaL_argcheck(_lua, tryGetSprite(_lua, 3, &sprite), 3, "sprite expected");
    bool result = self->animation->addFrame(std::chrono::milliseconds(duration), *sprite);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 duration
// 3 sprite sheet
// 4 sprite index
int luaApi_addFrameFromSpriteSheet(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_duration_expected);
    uint32_t duration = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    SpriteSheet * sprite_sheet;
    luaL_argcheck(_lua, tryGetSpriteSheet(_lua, 3, &sprite_sheet), 3, gc_message_sprite_sheet_expected);
    luaL_argcheck(_lua, lua_isinteger(_lua, 4), 4, "sprite index expected");
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 4));
    bool result = self->animation->addFrame(std::chrono::milliseconds(duration), *sprite_sheet, index);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 duration
// 3 sprite sheet
// 4 sprite indices
int luaApi_addFrames(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_duration_expected);
    uint32_t duration = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    SpriteSheet * sprite_sheet;
    luaL_argcheck(_lua, tryGetSpriteSheet(_lua, 3, &sprite_sheet), 3, gc_message_sprite_sheet_expected);
    luaL_argcheck(_lua, lua_istable(_lua, 4), 4, "sprite indices array expected");
    size_t index_count = lua_rawlen(_lua, 4);
    if(index_count == 0)
    {
        lua_pushboolean(_lua, false);
        return 1;
    }
    std::vector<size_t> indices(index_count);
    for(size_t i = 1; i <= index_count; ++i)
    {
        lua_rawgeti(_lua, 4, i);
        if(!lua_isinteger(_lua, -1))
            luaL_error(_lua, "the item %d of the indices array is not an integer", i);
        indices[i - 1] = static_cast<size_t>(lua_tointeger(_lua, -1));
        lua_pop(_lua, 1);
    }
    bool result = self->animation->addFrames(std::chrono::milliseconds(duration), *sprite_sheet, indices);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace name

void Sol2D::Lua::pushSpriteAnimationApi(lua_State * _lua, SpriteAnimation & _animation)
{
    Self * self = Self::pushUserData(_lua);
    self->animation = &_animation;
    if(Self::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "addFrameFromSprite", luaApi_addFrameFromSprite },
            { "addFrameFromSpriteSheet", luaApi_addFrameFromSpriteSheet },
            { "addFrames", luaApi_addFrames },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

bool Sol2D::Lua::tryGetSpriteAnimation(lua_State * _lua, int _idx, SpriteAnimation ** _animation)
{
    Self * self = Self::getUserData(_lua, _idx);
    if(self)
    {
        *_animation = self->animation;
        return true;
    }
    return false;
}
