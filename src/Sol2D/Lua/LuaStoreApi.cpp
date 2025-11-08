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

#include <Sol2D/Lua/LuaSceneOptionsApi.h>
#include <Sol2D/Lua/LuaStoreApi.h>
#include <Sol2D/Lua/LuaLayoutingApi.h>
#include <Sol2D/Lua/LuaViewApi.h>
#include <Sol2D/Lua/LuaSceneApi.h>
#include <Sol2D/Lua/LuaBodyDefinitionApi.h>
#include <Sol2D/Lua/LuaUIApi.h>
#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteSheetApi.h>
#include <Sol2D/Lua/LuaGraphicsPackApi.h>
#include <Sol2D/Lua/LuaFontApi.h>
#include <Sol2D/Lua/LuaAudioTrackApi.h>
#include <Sol2D/Lua/LuaAudioApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaUtils.h>
#include <Sol2D/Lua/Aux/LuaReferenceHolder.h>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    explicit Self(
        const Workspace & _workspace,
        Renderer & _renderer,
        MIX_Mixer & _mixer,
        std::shared_ptr<Store> & _store
    ) :
        workspace(_workspace),
        renderer(_renderer),
        mixer(_mixer),
        store(_store)
    {
    }

    void beforeDelete(lua_State * _lua) override
    {
        LuaSelfBase::beforeDelete(_lua);
        LuaReferenceHolder holder(_lua);
        holder.releaseAll(this);
    }

    std::shared_ptr<Store> getStore(lua_State * _lua) const
    {
        std::shared_ptr<Store> ptr = store.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::store_is_destroyed);
        return ptr;
    }

    void holdReference(lua_State * _lua, const char * _type, const char * _key) const
    {
        LuaReferenceHolder holder(_lua);
        holder.hold(this, _type, _key);
    }

    void releaseReference(lua_State * _lua, const char * _type, const char * _key) const
    {
        LuaReferenceHolder holder(_lua);
        holder.release(this, _type, _key);
    }

    const Workspace & workspace;
    Renderer & renderer;
    MIX_Mixer & mixer;
    std::weak_ptr<Store> store;
};

using UserData = LuaUserData<Self, LuaTypeName::store>;

// 1 self
// 2 key
// 3 style (optional)
int luaApi_CreateView(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    Style style;
    if(lua_gettop(_lua) > 2)
        luaL_argexpected(_lua, tryGetStyle(_lua, 3, style), 3, LuaTypeName::style);
    std::shared_ptr<View> view = self->getStore(_lua)->createObject<View>(key, self->renderer, style);
    pushViewApi(_lua, view);
    self->holdReference(_lua, LuaTypeName::view, key);
    return 1;
}

// 1 self
// 2 key
// 3 style (optional)
int luaApi_GetView(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<View> view = self->getStore(_lua)->getObject<View>(argToStringOrError(_lua, 2)))
        pushViewApi(_lua, view);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
// 3 node
// 4 options (optional)
int luaApi_CreateScene(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    SceneOptions options;
    tryGetSceneOptions(_lua, 4, options);
    Node * node = tryGetNode(_lua, 3);
    luaL_argexpected(_lua, node, 3, LuaTypeName::node);
    std::shared_ptr<Scene> scene =
        self->getStore(_lua)->createObject<Scene>(key, *node, options, self->workspace, self->renderer);
    pushSceneApi(_lua, self->workspace, scene);
    self->holdReference(_lua, LuaTypeName::scene, key);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetScene(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<Scene> scene = self->getStore(_lua)->getObject<Scene>(argToStringOrError(_lua, 2)))
        pushSceneApi(_lua, self->workspace, scene);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateUI(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    std::shared_ptr<UI> ui = self->getStore(_lua)->createObject<UI>(key);
    pushUIApi(_lua, ui);
    self->holdReference(_lua, LuaTypeName::ui, key);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetUI(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<UI> ui = self->getStore(_lua)->getObject<UI>(argToStringOrError(_lua, 2)))
        pushUIApi(_lua, ui);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateSprite(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    std::shared_ptr<Sprite> sprite = self->getStore(_lua)->createObject<Sprite>(key, self->renderer);
    pushSpriteApi(_lua, self->workspace, sprite);
    self->holdReference(_lua, LuaTypeName::sprite, key);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSprite(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<Sprite> sprite = self->getStore(_lua)->getObject<Sprite>(argToStringOrError(_lua, 2)))
        pushSpriteApi(_lua, self->workspace, sprite);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateSpriteSheet(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    std::shared_ptr<SpriteSheet> sprite_sheet = self->getStore(_lua)->createObject<SpriteSheet>(key, self->renderer);
    pushSpriteSheetApi(_lua, self->workspace, sprite_sheet);
    self->holdReference(_lua, LuaTypeName::sprite_sheet, key);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSpriteSheet(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<SpriteSheet> sprite_sheet =
           self->getStore(_lua)->getObject<SpriteSheet>(argToStringOrError(_lua, 2)))
        pushSpriteSheetApi(_lua, self->workspace, sprite_sheet);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
// 3 file path
// 4 font size
int luaApi_CreateFont(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * path = argToStringOrError(_lua, 3);
    luaL_argexpected(_lua, lua_isinteger(_lua, 4), 4, LuaTypeName::integer);
    const char * key = argToStringOrError(_lua, 2);
    std::shared_ptr<TTF_Font> font = self->getStore(_lua)->createObject<TTF_Font>(
        key,
        self->workspace.getResourceFullPath(std::filesystem::path(path)),
        static_cast<uint16_t>(lua_tointeger(_lua, 4))
    );
    if(font)
    {
        pushFontApi(_lua, font);
        self->holdReference(_lua, LuaTypeName::font, key);
    }
    else
    {
        lua_pushnil(_lua);
    }
    return 1;
}

// 1 self
// 2 key
int luaApi_GetFont(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<TTF_Font> font = self->getStore(_lua)->getObject<TTF_Font>(argToStringOrError(_lua, 2)))
        pushFontApi(_lua, font);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateAudioTrack(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    std::shared_ptr<MIX_Track> track = self->getStore(_lua)->createObject<MIX_Track>(key, self->mixer);
    if(track)
    {
        pushAudioTrackApi(_lua, track);
        self->holdReference(_lua, LuaTypeName::audio_track, key);
    }
    else
    {
        lua_pushnil(_lua);
    }
    return 1;
}

// 1 self
// 2 key
int luaApi_GetAudioTrack(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<MIX_Track> track = self->getStore(_lua)->getObject<MIX_Track>(argToStringOrError(_lua, 2)))
        pushAudioTrackApi(_lua, track);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
// 3 file path
// 4 predecode (optional)
int luaApi_CreateAudio(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    const char * path = argToStringOrError(_lua, 3);
    bool predecode = lua_gettop(_lua) > 3 ? lua_toboolean(_lua, 4) : false;
    std::shared_ptr<MIX_Audio> audio = self->getStore(_lua)->createObject<MIX_Audio>(
        key,
        self->mixer,
        self->workspace.getResourceFullPath(std::filesystem::path(path)),
        predecode
    );
    if(audio)
    {
        pushAudioApi(_lua, self->mixer, audio);
        self->holdReference(_lua, LuaTypeName::audio, key);
    }
    else
    {
        lua_pushnil(_lua);
    }
    return 1;
}

// 1 self
// 2 key
int luaApi_GetAudio(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<MIX_Audio> audio = self->getStore(_lua)->getObject<MIX_Audio>(argToStringOrError(_lua, 2)))
        pushAudioApi(_lua, self->mixer, audio);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
template<typename T, const char type[]>
int luaApi_FreeObject(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    bool result = self->getStore(_lua)->freeObject<T>(key);
    if(result)
        self->releaseReference(_lua, type, key);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushStoreApi(
    lua_State * _lua,
    const Workspace & _workspace,
    Renderer & _renderer,
    MIX_Mixer & _mixer,
    std::shared_ptr<Store> _store)
{
    UserData::pushUserData(_lua, _workspace, _renderer, _mixer, _store);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "createView", luaApi_CreateView },
            { "getView", luaApi_GetView },
            { "freeViewe", luaApi_FreeObject<View, LuaTypeName::view> },
            { "createScene", luaApi_CreateScene },
            { "getScene", luaApi_GetScene },
            { "freeScene", luaApi_FreeObject<Scene, LuaTypeName::scene> },
            { "createUI", luaApi_CreateUI },
            { "getUI", luaApi_GetUI },
            { "freeUI", luaApi_FreeObject<UI, LuaTypeName::ui> },
            { "createSprite", luaApi_CreateSprite },
            { "getSprite", luaApi_GetSprite },
            { "freeSprite", luaApi_FreeObject<Sprite, LuaTypeName::sprite> },
            { "createSpriteSheet", luaApi_CreateSpriteSheet },
            { "getSpriteSheet", luaApi_GetSpriteSheet },
            { "freeSpriteSheet", luaApi_FreeObject<SpriteSheet, LuaTypeName::sprite_sheet> },
            { "createAudioTrack", luaApi_CreateAudioTrack },
            { "getAudioTrack", luaApi_GetAudioTrack },
            { "freeAudioTrack", luaApi_FreeObject<MIX_Track, LuaTypeName::audio_track> },
            { "createAudio", luaApi_CreateAudio },
            { "getAudio", luaApi_GetAudio },
            { "freeAudio", luaApi_FreeObject<MIX_Audio, LuaTypeName::audio> },
            { "createFont", luaApi_CreateFont },
            { "getFont", luaApi_GetFont },
            { "freeFont", luaApi_FreeObject<TTF_Font, LuaTypeName::font> },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
