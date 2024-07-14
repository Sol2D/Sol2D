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
#include <Sol2D/Lua/LuaFormApi.h>
#include <Sol2D/Lua/LuaSceneApi.h>
#include <Sol2D/Lua/LuaViewApi.h>
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
using namespace Sol2D::Forms;
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
};

using UserData = LuaUserData<Self, LuaTypeName::store>;

int createView(lua_State * _lua, const Self & _self, const std::string & _key);
int createScene(lua_State * _lua, const Self & _self, const std::string & _key);
int createForm(lua_State * _lua, const Self & _self, const std::string & _key);
int createSprite(lua_State * _lua, const Self & _self, const std::string & _key);
int createSpriteSheet(lua_State * _lua, const Self & _self, const std::string & _key);
int createSpriteAnimation(lua_State * _lua, const Self & _self, const std::string & _key);
int createBodyPrototype(lua_State * _lua, const Self & _self, const std::string & _key);
int createFont(lua_State * _lua, const Self & _self, const std::string & _key);
int createSoundEffect(lua_State * _lua, const Self & _self, const std::string & _key);
int createMusic(lua_State * _lua, const Self & _self, const std::string & _key);

int getView(lua_State * _lua, const Self & _self, const std::string & _key);
int getScene(lua_State * _lua, const Self & _self, const std::string & _key);
int getForm(lua_State * _lua, const Self & _self, const std::string & _key);
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

struct ObjectDescription
{
    explicit ObjectDescription(lua_State * _lua);

    const ObjectApi * api;
    const Self * self;
    const char * key;
};

// Ignore: non-POD static (unordered_map) [clazy-non-pod-global-static]
const std::unordered_map<std::string, ObjectApi> gc_object_apis
{
    {
        LuaTypeName::view,
        { createView, getView, freeObject<View> }
    },
    {
        LuaTypeName::scene,
        { createScene, getScene, freeObject<Scene> }
    },
    {
        LuaTypeName::form,
        { createForm, getForm, freeObject<Form> }
    },
    {
        LuaTypeName::sprite,
        { createSprite, getSprite, freeObject<Sprite> }
    },
    {
        LuaTypeName::sprite_sheet,
        { createSpriteSheet, getSpriteSheet, freeObject<SpriteSheet> }
    },
    {
        LuaTypeName::sprite_animation,
        { createSpriteAnimation, getSpriteAnimation, freeObject<SpriteAnimation> }
    },
    {
        LuaTypeName::body_prototype,
        { createBodyPrototype, getBodyPrototype, freeObject<BodyPrototype> }
    },
    {
        LuaTypeName::font,
        { createFont, getFont, freeObject<TTF_Font> }
    },
    {
        LuaTypeName::sound_effect,
        { createSoundEffect, getSoundEffect, freeObject<Mix_Chunk> }
    },
    {
        LuaTypeName::music,
        { createMusic, getMusic, freeObject<Mix_Music> }
    }
};

// 1 self
// 2 object type
// 3 key
ObjectDescription::ObjectDescription(lua_State * _lua) :
    api(nullptr),
    self(nullptr),
    key(nullptr)
{
    self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, "an object type expected");
    const char * object_type = lua_tostring(_lua, 2);
    auto it = gc_object_apis.find(object_type);
    if(it == gc_object_apis.cend())
        luaL_argerror(_lua, 2, std::format("unsupported type: {}", object_type).c_str());
    else
        api = &it->second;
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
    return description.api->create(_lua, *description.self, description.key);
}

// 1 self
// 2 object type
// 3 key
int createView(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<View> view = _self.getStore(_lua)->createObject<View>(_key, _self.renderer);
    pushViewApi(_lua, view);
    return 1;
}

// 1 self
// 2 object type
// 3 key
int createScene(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<Scene> scene = _self.getStore(_lua)->createObject<Scene>(_key, _self.workspace, _self.renderer);
    pushSceneApi(_lua, _self.workspace, scene);
    return 1;
}

// 1 self
// 2 object type
// 3 key
int createForm(lua_State * _lua, const Self & _self, const std::string & _key)
{
    std::shared_ptr<Form> form = _self.getStore(_lua)->createObject<Form>(_key, _self.renderer);
    pushFormApi(_lua, _self.workspace, form);
    return 1;
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
    return description.api->get(_lua, *description.self, description.key);
}

int getView(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<View> view = _self.getStore(_lua)->getObject<View>(_key))
        pushViewApi(_lua, view);
    else
        lua_pushnil(_lua);
    return 1;
}

int getScene(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<Scene> scene = _self.getStore(_lua)->getObject<Scene>(_key))
        pushSceneApi(_lua, _self.workspace, scene);
    else
        lua_pushnil(_lua);
    return 1;
}

int getForm(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<Form> form = _self.getStore(_lua)->getObject<Form>(_key))
        pushFormApi(_lua, _self.workspace, form);
    else
        lua_pushnil(_lua);
    return 1;
}

int getSprite(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<Sprite> sprite = _self.getStore(_lua)->getObject<Sprite>(_key))
        pushSpriteApi(_lua, _self.workspace, sprite);
    else
        lua_pushnil(_lua);
    return 1;
}

int getSpriteSheet(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<SpriteSheet> sprite_sheet = _self.getStore(_lua)->getObject<SpriteSheet>(_key))
        pushSpriteSheetApi(_lua, _self.workspace, sprite_sheet);
    else
        lua_pushnil(_lua);
    return 1;
}

int getSpriteAnimation(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<SpriteAnimation> sprite_animation = _self.getStore(_lua)->getObject<SpriteAnimation>(_key))
        pushSpriteAnimationApi(_lua, sprite_animation);
    else
        lua_pushnil(_lua);
    return 1;
}

int getBodyPrototype(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<BodyPrototype> proto = _self.getStore(_lua)->getObject<BodyPrototype>(_key))
        pushBodyPrototypeApi(_lua, proto);
    else
        lua_pushnil(_lua);
    return 1;
}

int getFont(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<TTF_Font> font = _self.getStore(_lua)->getObject<TTF_Font>(_key))
        pushFontApi(_lua, font);
    else
        lua_pushnil(_lua);
    return 1;
}

int getSoundEffect(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<Mix_Chunk> chunk = _self.getStore(_lua)->getObject<Mix_Chunk>(_key))
        pushSoundEffectApi(_lua, chunk);
    else
        lua_pushnil(_lua);
    return 1;
}

int getMusic(lua_State * _lua, const Self & _self, const std::string & _key)
{
    if(std::shared_ptr<Mix_Music> chunk = _self.getStore(_lua)->getObject<Mix_Music>(_key))
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
    return description.api->free(_lua, *description.self, description.key);
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
