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

#include <Sol2D/Lua/LuaStoreApi.h>
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

struct Self : LuaSelfBase
{
    explicit Self(const Workspace & _workspace, SDL_Renderer & _renderer, std::shared_ptr<Store> & _store) :
        workspace(_workspace),
        renderer(_renderer),
        store(_store)
    {
    }

    std::shared_ptr<Store> getStore(lua_State * _lua) const
    {
        std::shared_ptr<Store> ptr = store.lock();
        if(!ptr)
            luaL_error(_lua, "the store is destroyed");
        return ptr;
    }

    const Workspace & workspace;
    SDL_Renderer & renderer;
    std::weak_ptr<Store> store;
    // static const std::unordered_map<std::string, ObjectApi> object_apis;
};

// const std::unordered_map<std::string, ObjectApi> Self::object_apis
// {
//     {
//         LuaTypeName::sprite,
//         { createSprite, getSprite, freeObject<Sprite> }
//     },
//     {
//         LuaTypeName::sprite_sheet,
//         { createSpriteSheet, getSpriteSheet, freeObject<SpriteSheet> }
//     },
//     {
//         LuaTypeName::sprite_animation,
//         { createSpriteAnimation, getSpriteAnimation, freeObject<SpriteAnimation> }
//     },
//     {
//         LuaTypeName::body_prototype,
//         { createBodyPrototype, getBodyPrototype, freeObject<BodyPrototype> }
//     },
//     {
//         LuaTypeName::font,
//         { createFont, getFont, freeObject<TTF_Font> }
//     },
//     {
//         LuaTypeName::sound_effect,
//         { createSoundEffect, getSoundEffect, freeObject<Mix_Chunk> }
//     },
//     {
//         LuaTypeName::music,
//         { createMusic, getMusic, freeObject<Mix_Music> }
//     }
// };

int createSprite(lua_State * _lua, const Self & _self, const std::string & _key);
int createSpriteSheet(lua_State * _lua, const Self & _self, const std::string & _key);
int createSpriteAnimation(lua_State * _lua, const Self & _self, const std::string & _key);
int createBodyPrototype(lua_State * _lua, const Self & _self, const std::string & _key);
int createFont(lua_State * _lua, const Self & _self, const std::string & _key);
int createSoundEffect(lua_State * _lua, const Self & _self, const std::string & _key);
int createMusic(lua_State * _lua, const Self & _self, const std::string & _key);

int getSprite(lua_State * _lua, const Self & _self, const std::string & _key);
int getSpriteSheet(lua_State * _lua, const Self & _self, const std::string & _key);
int getSpriteAnimation(lua_State * _lua, const Self & _self, const std::string & _key);
int getBodyPrototype(lua_State * _lua, const Self & _self, const std::string & _key);
int getFont(lua_State * _lua, const Self & _self, const std::string & _key);
int getSoundEffect(lua_State * _lua, const Self & _self, const std::string & _key);
int getMusic(lua_State * _lua, const Self & _self, const std::string & _key);

template<typename T>
int freeObject(lua_State * _lua, const Self & _self, const std::string & _key);

struct ObjectApi
{
public:
    int (* create)(lua_State * _lua, const Self & _self, const std::string & _key);
    int (* get)(lua_State * _lua, const Self & _self, const std::string & _key);
    int (* free)(lua_State * _lua, const Self & _self, const std::string & _key);
};

using UserData = LuaUserData<Self, LuaTypeName::store>;

struct ObjectDescription
{
    explicit ObjectDescription(lua_State * _lua);

    ObjectApi api;
    const Self * self;
    const char * key;
};

// 1 self
// 2 object type
// 3 key
ObjectDescription::ObjectDescription(lua_State * _lua) :
    api{},
    self(nullptr),
    key(nullptr)
{
    self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, "an object type expected");
    const char * object_type = lua_tostring(_lua, 2);
    if(std::strcmp(LuaTypeName::sprite, object_type) == 0)
        api = { createSprite, getSprite, freeObject<Sprite> };
    else if(std::strcmp(LuaTypeName::sprite_sheet, object_type) == 0)
        api = { createSpriteSheet, getSpriteSheet, freeObject<SpriteSheet> };
    else if(std::strcmp(LuaTypeName::sprite_animation, object_type) == 0)
        api = { createSpriteAnimation, getSpriteAnimation, freeObject<SpriteAnimation> };
    else if(std::strcmp(LuaTypeName::body_prototype, object_type) == 0)
        api = { createBodyPrototype, getBodyPrototype, freeObject<BodyPrototype> };
    else if(std::strcmp(LuaTypeName::font, object_type) == 0)
        api = { createFont, getFont, freeObject<TTF_Font> };
    else if(std::strcmp(LuaTypeName::sound_effect, object_type) == 0)
        api = { createSoundEffect, getSoundEffect, freeObject<Mix_Chunk> };
    else if(std::strcmp(LuaTypeName::music, object_type) == 0)
        api = { createMusic, getMusic, freeObject<Mix_Music> };
    else
    {
        luaL_argerror(_lua, 2, std::format("unsupported type: {}", object_type).c_str());
        throw std::runtime_error(nullptr); // For static analyzers, luaL_argerror never returns.
    }
    luaL_argcheck(_lua, lua_isstring(_lua, 3), 3, "an object key expected");
    key = lua_tostring(_lua, 3);
}

// 1 self
// 2 object type
// 3 key
// ... Object specific
int luaApi_CreateObject(lua_State * _lua)
{
    ObjectDescription description(_lua);
    return description.api.create(_lua, *description.self, description.key);
}

// 1 self
// 2 object type
// 3 key
int createSprite(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<Sprite> sprite = _self.getStore(_lua)->createObject<Sprite>(_key, _self.renderer);
    pushSpriteApi(_lua, _self.workspace, sprite);
    return 1;
}

// 1 self
// 2 object type
// 3 key
int createSpriteSheet(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<SpriteSheet> sprite_sheet = _self.getStore(_lua)->createObject<SpriteSheet>(_key, _self.renderer);
    pushSpriteSheetApi(_lua, _self.workspace, sprite_sheet);
    return 1;
}

// 1 self
// 2 object type
// 3 key
int createSpriteAnimation(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<SpriteAnimation> sprite_animation = _self.getStore(_lua)->createObject<SpriteAnimation>(
        _key,
        _self.renderer);
    pushSpriteAnimationApi(_lua, sprite_animation);
    return 1;
}

// 1 self
// 2 object type
// 3 key
// 4 body type
int createBodyPrototype(lua_State * _lua, const Self & _self, const std::string & _key)
{
    luaL_argcheck(_lua, lua_isinteger(_lua, 4), 4, "a body type expected");
    std::optional<BodyType> body_type = castToBodyType(lua_tointeger(_lua, 4));
    std::shared_ptr<BodyPrototype> proto = _self.getStore(_lua)->createObject<BodyPrototype>(
        _key,
        body_type.value_or(BodyType::Static));
    pushBodyPrototypeApi(_lua, proto);
    return 1;
}

// 1 self
// 2 object type
// 3 key
// 4 file path
// 5 font size
int createFont(lua_State * _lua, const Self & _self, const std::string & _key)
{
    luaL_argcheck(_lua, lua_isstring(_lua, 4), 4, "a font file path expected");
    const char * path = lua_tostring(_lua, 4);
    luaL_argcheck(_lua, lua_isinteger(_lua, 5), 5, "a font size expected");
    std::shared_ptr<TTF_Font> font = _self.getStore(_lua)->createObject<TTF_Font>(
        _key,
        _self.workspace.getResourceFullPath(std::filesystem::path(path)),
        static_cast<uint16_t>(lua_tointeger(_lua, 5)));
    if(font)
        pushFontApi(_lua, font);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 object type
// 3 key
// 4 file path
int createSoundEffect(lua_State * _lua, const Self & _self, const std::string & _key)
{
    luaL_argcheck(_lua, lua_isstring(_lua, 4), 4, "a sound effect file path expected");
    const char * path = lua_tostring(_lua, 4);
    std::shared_ptr<Mix_Chunk> chunk = _self.getStore(_lua)->createObject<Mix_Chunk>(
        _key,
        _self.workspace.getResourceFullPath(std::filesystem::path(path)));
    if(chunk)
        pushSoundEffectApi(_lua, chunk);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 object type
// 3 key
// 4 file path
int createMusic(lua_State * _lua, const Self & _self, const std::string & _key)
{
    luaL_argcheck(_lua, lua_isstring(_lua, 4), 4, "a music file path expected");
    const char * path = lua_tostring(_lua, 4);
    std::shared_ptr<Mix_Music> chunk = _self.getStore(_lua)->createObject<Mix_Music>(
        _key,
        _self.workspace.getResourceFullPath(std::filesystem::path(path)));
    if(chunk)
        pushMusicApi(_lua, chunk);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 object type
// 3 key
int luaApi_GetObject(lua_State * _lua)
{
    ObjectDescription description(_lua);
    return description.api.get(_lua, *description.self, description.key);
}

int getSprite(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<Sprite> sprite = _self.getStore(_lua)->getObject<Sprite>(_key);
    if(sprite)
        pushSpriteApi(_lua, _self.workspace, sprite);
    else
        lua_pushnil(_lua);
    return 1;
}

int getSpriteSheet(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<SpriteSheet> sprite_sheet = _self.getStore(_lua)->getObject<SpriteSheet>(_key);
    if(sprite_sheet)
        pushSpriteSheetApi(_lua, _self.workspace, sprite_sheet);
    else
        lua_pushnil(_lua);
    return 1;
}

int getSpriteAnimation(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<SpriteAnimation> sprite_animation = _self.getStore(_lua)->getObject<SpriteAnimation>(_key);
    if(sprite_animation)
        pushSpriteAnimationApi(_lua, sprite_animation);
    else
        lua_pushnil(_lua);
    return 1;
}

int getBodyPrototype(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<BodyPrototype> proto = _self.getStore(_lua)->getObject<BodyPrototype>(_key);
    if(proto)
        pushBodyPrototypeApi(_lua, proto);
    else
        lua_pushnil(_lua);
    return 1;
}

int getFont(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<TTF_Font> font = _self.getStore(_lua)->getObject<TTF_Font>(_key);
    if(font)
        pushFontApi(_lua, font);
    else
        lua_pushnil(_lua);
    return 1;
}

int getSoundEffect(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<Mix_Chunk> chunk = _self.getStore(_lua)->getObject<Mix_Chunk>(_key);
    if(chunk)
        pushSoundEffectApi(_lua, chunk);
    else
        lua_pushnil(_lua);
    return 1;
}

int getMusic(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<Mix_Music> chunk = _self.getStore(_lua)->getObject<Mix_Music>(_key);
    if(chunk)
        pushMusicApi(_lua, chunk);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 object type
// 3 key
int luaApi_FreeObject(lua_State * _lua)
{
    ObjectDescription description(_lua);
    return description.api.free(_lua, *description.self, description.key);
}

template<typename T>
int freeObject(lua_State * _lua, const Self & _self, const std::string & _key)
{
    bool result = _self.getStore(_lua)->freeObject<T>(_key);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushStoreApi(
    lua_State * _lua,
    const Workspace & _workspace,
    SDL_Renderer & _renderer,
    std::shared_ptr<Store> _store)
{
    UserData::pushUserData(_lua, _workspace, _renderer, _store);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "createObject", luaApi_CreateObject },
            { "getObject", luaApi_GetObject },
            { "freeObject", luaApi_FreeObject },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
