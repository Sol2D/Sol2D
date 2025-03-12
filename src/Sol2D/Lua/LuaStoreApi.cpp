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
#include <Sol2D/Lua/LuaFormApi.h>
#include <Sol2D/Lua/LuaSceneApi.h>
#include <Sol2D/Lua/LuaBodyDefinitionApi.h>
#include <Sol2D/Lua/LuaViewApi.h>
#include <Sol2D/Lua/LuaUIApi.h>
#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteSheetApi.h>
#include <Sol2D/Lua/LuaGraphicsPackApi.h>
#include <Sol2D/Lua/LuaFontApi.h>
#include <Sol2D/Lua/LuaSoundEffectApi.h>
#include <Sol2D/Lua/LuaMusicApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaUtils.h>
#include <Sol2D/Lua/Aux/LuaReferenceHolder.h>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Forms;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    explicit Self(const Workspace & _workspace, Renderer & _renderer, std::shared_ptr<Store> & _store) :
        workspace(_workspace),
        renderer(_renderer),
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
    std::weak_ptr<Store> store;
};

using UserData = LuaUserData<Self, LuaTypeName::store>;

// 1 self
// 2 key
int luaApi_CreateView(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    std::shared_ptr<View> view = self->getStore(_lua)->createObject<View>(key, self->renderer);
    pushViewApi(_lua, view);
    self->holdReference(_lua, LuaTypeName::view, key);
    return 1;
}

// 1 self
// 2 key
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
// 3 options (optional)
int luaApi_CreateScene(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    SceneOptions options;
    tryGetSceneOptions(_lua, 3, options);
    const char * key = argToStringOrError(_lua, 2);
    std::shared_ptr<Scene> scene = self->getStore(_lua)->createObject<Scene>(
        key,
        options,
        self->workspace,
        self->renderer);
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
int luaApi_CreateForm(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    std::shared_ptr<Form> form = self->getStore(_lua)->createObject<Form>(key, self->renderer);
    pushFormApi(_lua, self->workspace, form);
    self->holdReference(_lua, LuaTypeName::form, key);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetForm(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<Form> form = self->getStore(_lua)->getObject<Form>(argToStringOrError(_lua, 2)))
        pushFormApi(_lua, self->workspace, form);
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
    std::shared_ptr<SpriteSheet> sprite_sheet =
        self->getStore(_lua)->createObject<SpriteSheet>(key, self->renderer);
    pushSpriteSheetApi(_lua, self->workspace, sprite_sheet);
    self->holdReference(_lua, LuaTypeName::sprite_sheet, key);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSpriteSheet(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<SpriteSheet> sprite_sheet = self->getStore(_lua)->getObject<SpriteSheet>(argToStringOrError(_lua, 2)))
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
        static_cast<uint16_t>(lua_tointeger(_lua, 4)));
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
// 3 file path
int luaApi_CreateSoundEffect(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    const char * path = argToStringOrError(_lua, 3);
    std::shared_ptr<Mix_Chunk> chunk = self->getStore(_lua)->createObject<Mix_Chunk>(
        key,
        self->workspace.getResourceFullPath(std::filesystem::path(path)));
    if(chunk)
    {
        pushSoundEffectApi(_lua, chunk);
        self->holdReference(_lua, LuaTypeName::sound_effect, key);
    }
    else
    {
        lua_pushnil(_lua);
    }
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSoundEffect(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<Mix_Chunk> chunk = self->getStore(_lua)->getObject<Mix_Chunk>(argToStringOrError(_lua, 2)))
        pushSoundEffectApi(_lua, chunk);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
// 3 file path
int luaApi_CreateMusic(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = argToStringOrError(_lua, 2);
    const char * path = argToStringOrError(_lua, 3);
    std::shared_ptr<Mix_Music> music = self->getStore(_lua)->createObject<Mix_Music>(
        key,
        self->workspace.getResourceFullPath(std::filesystem::path(path)));
    if(music)
    {
        pushMusicApi(_lua, music);
        self->holdReference(_lua, LuaTypeName::music, key);
    }
    else
    {
        lua_pushnil(_lua);
    }
    return 1;
}

// 1 self
// 2 key
int luaApi_GetMusic(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<Mix_Music> music = self->getStore(_lua)->getObject<Mix_Music>(argToStringOrError(_lua, 2)))
        pushMusicApi(_lua, music);
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
    if(result) self->releaseReference(_lua, type, key);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushStoreApi(
    lua_State * _lua,
    const Workspace & _workspace,
    Renderer & _renderer,
    std::shared_ptr<Store> _store)
{
    UserData::pushUserData(_lua, _workspace, _renderer, _store);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "createView", luaApi_CreateView },
            { "getView", luaApi_GetView },
            { "freeView", luaApi_FreeObject<View, LuaTypeName::view> },
            { "createScene", luaApi_CreateScene },
            { "getScene", luaApi_GetScene },
            { "freeScene", luaApi_FreeObject<Scene, LuaTypeName::scene> },
            { "createForm", luaApi_CreateForm },
            { "getForm", luaApi_GetForm },
            { "freeForm", luaApi_FreeObject<Form, LuaTypeName::form> },
            { "createUI", luaApi_CreateUI },
            { "getUI", luaApi_GetUI },
            { "freeUI", luaApi_FreeObject<UI, LuaTypeName::ui> },
            { "createSprite", luaApi_CreateSprite },
            { "getSprite", luaApi_GetSprite },
            { "freeSprite", luaApi_FreeObject<Sprite, LuaTypeName::sprite> },
            { "createSpriteSheet", luaApi_CreateSpriteSheet },
            { "getSpriteSheet", luaApi_GetSpriteSheet },
            { "freeSpriteSheet", luaApi_FreeObject<SpriteSheet, LuaTypeName::sprite_sheet> },
            { "createSoundEffect", luaApi_CreateSoundEffect },
            { "getSoundEffect", luaApi_GetSoundEffect },
            { "freeSoundEffect", luaApi_FreeObject<Mix_Chunk, LuaTypeName::sound_effect> },
            { "createMusic", luaApi_CreateMusic },
            { "getMusic", luaApi_GetMusic },
            { "freeMusic", luaApi_FreeObject<Mix_Music, LuaTypeName::music> },
            { "createFont", luaApi_CreateFont },
            { "getFont", luaApi_GetFont },
            { "freeFont", luaApi_FreeObject<TTF_Font, LuaTypeName::font> },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
