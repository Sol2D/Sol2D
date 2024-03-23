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

#include <Sol2D/Lua/LuaLarderApi.h>
#include <Sol2D/Lua/LuaBodyPrototypeApi.h>
#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteSheetApi.h>
#include <Sol2D/Lua/LuaSpriteAnimationApi.h>
#include <Sol2D/Lua/LuaAux.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

const char gc_metatable_larder[] = "sol.Larder";
const char gc_message_body_prototype_key_expected[] = "a body prototype key expected";
const char gc_message_sprite_key_expected[] = "a sprite key expected";
const char gc_message_sprite_sheet_key_expected[] = "a sprite sheet key expected";
const char gc_message_sprite_animation_key_expected[] = "a sprite animation key expected";

struct Self : LuaUserData<Self, gc_metatable_larder>
{
    Larder * larder;
    const Workspace * workspace;
};

// 1 self
// 2 key
// 3 body type
int luaApi_CreateBodyPrototype(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_body_prototype_key_expected);
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, "a body type expected");
    std::optional<BodyType> body_type = castToBodyType(lua_tointeger(_lua, 3));
    BodyPrototype & proto = self->larder->createBodyPrototype(
        key,
        body_type.has_value() ? body_type.value() : BodyType::Static);
    pushBodyPrototypeApi(_lua, proto);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetBodyPrototype(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_body_prototype_key_expected);
    BodyPrototype * proto = self->larder->getBodyPrototype(key);
    if(proto)
        pushBodyPrototypeApi(_lua, *proto);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_DeleteBodyPrototype(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_body_prototype_key_expected);
    lua_pushboolean(_lua, self->larder->deleteBodyPrototype(key));
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateSprite(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_sprite_key_expected);
    Sprite & sprite = self->larder->createSprite(key);
    pushSpriteApi(_lua, *self->workspace, sprite);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSprite(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_sprite_key_expected);
    Sprite * sprite = self->larder->getSprite(key);
    if(sprite)
        pushSpriteApi(_lua, *self->workspace, *sprite);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_DeleteSprite(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_sprite_key_expected);
    lua_pushboolean(_lua, self->larder->deleteSprite(key));
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateSpriteSheet(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_sprite_sheet_key_expected);
    SpriteSheet & sprite_sheet = self->larder->createSpriteSheet(key);
    pushSpriteSheetApi(_lua, *self->workspace, sprite_sheet);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSpriteSheet(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_sprite_sheet_key_expected);
    SpriteSheet * sprite_sheet = self->larder->getSpriteSheet(key);
    if(sprite_sheet)
        pushSpriteSheetApi(_lua, *self->workspace, *sprite_sheet);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_DeleteSpriteSheet(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_sprite_sheet_key_expected);
    lua_pushboolean(_lua, self->larder->deleteSpriteSheet(key));
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateSpriteAnimation(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_sprite_animation_key_expected);
    SpriteAnimation & sprite_animation = self->larder->createSpriteAnimation(key);
    pushSpriteAnimationApi(_lua, sprite_animation);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSpriteAnimation(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_sprite_animation_key_expected);
    SpriteAnimation * sprite_animation = self->larder->getSpriteAnimation(key);
    if(sprite_animation)
        pushSpriteAnimationApi(_lua, *sprite_animation);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_DeleteSpriteAnimation(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_sprite_animation_key_expected);
    lua_pushboolean(_lua, self->larder->deleteSpriteAnimation(key));
    return 1;
}

} // namespace name

void Sol2D::Lua::pushLarderApi(lua_State * _lua,  const Workspace & _workspace, Larder & _larder)
{
    Self * self = Self::pushUserData(_lua);
    self->larder = &_larder;
    self->workspace = &_workspace;
    if(Self::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "createBodyPrototype", luaApi_CreateBodyPrototype },
            { "getBodyPrototype", luaApi_GetBodyPrototype },
            { "deleteBodyPrototype", luaApi_DeleteBodyPrototype },
            { "createSprite", luaApi_CreateSprite },
            { "getSprite", luaApi_GetSprite },
            { "deleteSprite", luaApi_DeleteSprite },
            { "createSpriteSheet", luaApi_CreateSpriteSheet },
            { "getSpriteSheet", luaApi_GetSpriteSheet },
            { "deleteSpriteSheet", luaApi_DeleteSpriteSheet },
            { "createSpriteAnimation", luaApi_CreateSpriteAnimation },
            { "getSpriteAnimation", luaApi_GetSpriteAnimation },
            { "deleteSpriteAnimation", luaApi_DeleteSpriteAnimation },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
