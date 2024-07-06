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
#include <Sol2D/Lua/LuaFontApi.h>
#include <Sol2D/Lua/LuaSoundEffectApi.h>
#include <Sol2D/Lua/LuaMusicApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_message_body_prototype_key_expected[] = "a body prototype key expected";
const char gc_message_sprite_key_expected[] = "a sprite key expected";
const char gc_message_sprite_sheet_key_expected[] = "a sprite sheet key expected";
const char gc_message_sprite_animation_key_expected[] = "a sprite animation key expected";
const char gc_message_font_file_path_expected[] = "a font file path expected";
const char gc_message_font_size_expected[] = "a font size expected";
const char gc_message_sound_effect_file_path_expected[] = "a sound effect file path expected";
const char gc_message_music_file_path_expected[] = "a music file path expected";

struct Self : LuaUserData<Self, LuaTypeName::larder>
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
    std::shared_ptr<BodyPrototype> proto = self->larder->createBodyPrototype(
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
    std::shared_ptr<BodyPrototype> proto = self->larder->getBodyPrototype(key);
    if(proto)
        pushBodyPrototypeApi(_lua, proto);
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
    std::shared_ptr<Sprite> sprite = self->larder->createSprite(key);
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
    std::shared_ptr<Sprite> sprite = self->larder->getSprite(key);
    if(sprite)
        pushSpriteApi(_lua, *self->workspace, sprite);
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
    std::shared_ptr<SpriteSheet> sprite_sheet = self->larder->createSpriteSheet(key);
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
    std::shared_ptr<SpriteSheet> sprite_sheet = self->larder->getSpriteSheet(key);
    if(sprite_sheet)
        pushSpriteSheetApi(_lua, *self->workspace, sprite_sheet);
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
    std::shared_ptr<SpriteAnimation> sprite_animation = self->larder->createSpriteAnimation(key);
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
    std::shared_ptr<SpriteAnimation> sprite_animation = self->larder->getSpriteAnimation(key);
    if(sprite_animation)
        pushSpriteAnimationApi(_lua, sprite_animation);
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

// 1 self
// 2 file path
// 3 font size
int luaApi_GetFont(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * path = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, path != nullptr, 2, gc_message_font_file_path_expected);
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, gc_message_font_size_expected);
    std::shared_ptr<TTF_Font> font = self->larder->getFont(
        self->workspace->getResourceFullPath(std::filesystem::path(path)),
        static_cast<uint16_t>(lua_tointeger(_lua, 3))
    );
    if(font == nullptr)
        lua_pushnil(_lua);
    else
        pushFontApi(_lua, font);
    return 1;
}

// 1 self
// 2 file path
// 3 font size
int luaApi_FreeFont(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * path = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, path != nullptr, 2, gc_message_font_file_path_expected);
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, gc_message_font_size_expected);
    self->larder->freeFont(
        self->workspace->getResourceFullPath(std::filesystem::path(path)),
        static_cast<uint16_t>(lua_tointeger(_lua, 3))
    );
    return 0;
}

// 1 self
// 2 file path
int luaApi_GetSoundEffect(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * path = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, path != nullptr, 2, gc_message_sound_effect_file_path_expected);
    std::shared_ptr<Mix_Chunk> chunk = self->larder->getSoundChunk(
        self->workspace->getResourceFullPath(std::filesystem::path(path))
    );
    if(chunk == nullptr)
        lua_pushnil(_lua);
    else
        pushSoundEffectApi(_lua, chunk);
    return 1;
}

// 1 self
// 2 file path
int luaApi_FreeSoundEffect(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * path = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, path != nullptr, 2, gc_message_sound_effect_file_path_expected);
    self->larder->freeSoundChunk(
        self->workspace->getResourceFullPath(std::filesystem::path(path))
    );
    return 0;
}

// 1 self
// 2 file path
int luaApi_GetMusic(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * path = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, path != nullptr, 2, gc_message_music_file_path_expected);
    std::shared_ptr<Mix_Music> chunk = self->larder->getMusic(
        self->workspace->getResourceFullPath(std::filesystem::path(path))
    );
    if(chunk == nullptr)
        lua_pushnil(_lua);
    else
        pushMusicApi(_lua, chunk);
    return 1;
}

// 1 self
// 2 file path
int luaApi_FreeMusic(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * path = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, path != nullptr, 2, gc_message_music_file_path_expected);
    self->larder->freeMusic(
        self->workspace->getResourceFullPath(std::filesystem::path(path))
    );
    return 0;
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
            { "getFont", luaApi_GetFont },
            { "freeFont", luaApi_FreeFont },
            { "getSoundEffect", luaApi_GetSoundEffect },
            { "freeSoundEffect", luaApi_FreeSoundEffect },
            { "getMusic", luaApi_GetMusic },
            { "freeMusic", luaApi_FreeMusic },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
