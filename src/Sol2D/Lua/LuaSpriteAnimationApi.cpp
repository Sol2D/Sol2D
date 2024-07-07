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
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_message_sprite_sheet_expected[] = "sprite sheet expected";

struct Self : LuaSelfBase
{
    Self(std::shared_ptr<SpriteAnimation> & _animation) :
        animation(_animation)
    {
    }

    std::shared_ptr<SpriteAnimation> getAnimation(lua_State * _lua) const
    {
        std::shared_ptr<SpriteAnimation> ptr = animation.lock();
        if(!ptr)
            luaL_error(_lua, "the sprite animation is destroyed");
        return ptr;
    }

    std::weak_ptr<SpriteAnimation> animation;
};

using UserData = LuaUserData<Self, LuaTypeName::sprite_animation>;

void readSpriteAnimationOptions(lua_State * _lua, int _idx, SpriteAnimationOptions & _options)
{
    if(!lua_istable(_lua, _idx))
        return;

    LuaTable table(_lua, _idx);

    {
        lua_Integer duration;
        if(table.tryGetInteger("duration", &duration))
            _options.duration = std::chrono::milliseconds(duration);
    }

    if(table.tryGetValue("position"))
    {
        Point position;
        if(tryGetPoint(_lua, -1, position))
            _options.position = position;
    }
}

// 1 self
// 2 sprite
// 3 options (optional)
int luaApi_addFrameFromSprite(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::shared_ptr<Sprite> sprite = tryGetSprite(_lua, 2);
    luaL_argcheck(_lua, sprite, 2, "sprite expected");
    SpriteAnimationOptions options;
    readSpriteAnimationOptions(_lua, 3, options);
    bool result = self->getAnimation(_lua)->addFrame(*sprite, options);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 sprite sheet
// 3 sprite index
// 4 options (optional)
int luaApi_addFrameFromSpriteSheet(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::shared_ptr<SpriteSheet> sprite_sheet = tryGetSpriteSheet(_lua, 2);
    luaL_argcheck(_lua, sprite_sheet, 2, gc_message_sprite_sheet_expected);
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, "sprite index expected");
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 4));
    SpriteAnimationOptions options;
    readSpriteAnimationOptions(_lua, 4, options);
    bool result = self->getAnimation(_lua)->addFrame(*sprite_sheet, index, options);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 sprite sheet
// 3 sprite indices
// 4 options (optional)
int luaApi_addFrames(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::shared_ptr<SpriteSheet> sprite_sheet = tryGetSpriteSheet(_lua, 2);
    luaL_argcheck(_lua, sprite_sheet, 2, gc_message_sprite_sheet_expected);
    luaL_argcheck(_lua, lua_istable(_lua, 3), 3, "sprite indices array expected");
    size_t index_count = lua_rawlen(_lua, 3);
    if(index_count == 0)
    {
        lua_pushboolean(_lua, false);
        return 1;
    }
    std::vector<size_t> indices(index_count);
    for(size_t i = 1; i <= index_count; ++i)
    {
        lua_rawgeti(_lua, 3, i);
        if(!lua_isinteger(_lua, -1))
            luaL_error(_lua, "the item %d of the indices array is not an integer", i);
        indices[i - 1] = static_cast<size_t>(lua_tointeger(_lua, -1));
        lua_pop(_lua, 1);
    }
    SpriteAnimationOptions options;
    readSpriteAnimationOptions(_lua, 4, options);
    bool result = self->getAnimation(_lua)->addFrames(*sprite_sheet, indices, options);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace name

void Sol2D::Lua::pushSpriteAnimationApi(lua_State * _lua, std::shared_ptr<SpriteAnimation> _animation)
{
    UserData::pushUserData(_lua, _animation);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "__gc", UserData::luaGC },
            { "addFrameFromSprite", luaApi_addFrameFromSprite },
            { "addFrameFromSpriteSheet", luaApi_addFrameFromSpriteSheet },
            { "addFrames", luaApi_addFrames },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<SpriteAnimation> Sol2D::Lua::tryGetSpriteAnimation(lua_State * _lua, int _idx)
{
    Self * self = UserData::getUserData(_lua, _idx);
    return self ? self->animation.lock() : nullptr;
}
