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

#include <Sol2D/Lua/LuaSceneOptionsApi.h>
#include <Sol2D/Lua/LuaStoreApi.h>
#include <Sol2D/Lua/LuaFormApi.h>
#include <Sol2D/Lua/LuaSceneApi.h>
#include <Sol2D/Lua/LuaBodyDefinitionApi.h>
#include <Sol2D/Lua/LuaBodyPrototypeApi.h>
#include <Sol2D/Lua/LuaViewApi.h>
#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteSheetApi.h>
#include <Sol2D/Lua/LuaGraphicsPackApi.h>
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

const char gc_message_key_required[] = "key required";

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

// 1 self
// 2 key
int luaApi_CreateView(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    std::shared_ptr<View> view = self->getStore(_lua)->createObject<View>(lua_tostring(_lua, 2), self->renderer);
    pushViewApi(_lua, view);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetView(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    if(std::shared_ptr<View> view = self->getStore(_lua)->getObject<View>(lua_tostring(_lua, 2)))
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
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    SceneOptions options;
    tryGetSceneOptions(_lua, 3, options);
    std::shared_ptr<Scene> scene = self->getStore(_lua)->createObject<Scene>(
        lua_tostring(_lua, 2),
        options,
        self->workspace,
        self->renderer);
    pushSceneApi(_lua, self->workspace, scene);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetScene(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    if(std::shared_ptr<Scene> scene = self->getStore(_lua)->getObject<Scene>(lua_tostring(_lua, 2)))
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
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    std::shared_ptr<Form> form = self->getStore(_lua)->createObject<Form>(lua_tostring(_lua, 2), self->renderer);
    pushFormApi(_lua, self->workspace, form);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetForm(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    if(std::shared_ptr<Form> form = self->getStore(_lua)->getObject<Form>(lua_tostring(_lua, 2)))
        pushFormApi(_lua, self->workspace, form);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateSprite(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    std::shared_ptr<Sprite> sprite = self->getStore(_lua)->createObject<Sprite>(lua_tostring(_lua, 2), self->renderer);
    pushSpriteApi(_lua, self->workspace, sprite);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSprite(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    if(std::shared_ptr<Sprite> sprite = self->getStore(_lua)->getObject<Sprite>(lua_tostring(_lua, 2)))
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
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    std::shared_ptr<SpriteSheet> sprite_sheet =
        self->getStore(_lua)->createObject<SpriteSheet>(lua_tostring(_lua, 2), self->renderer);
    pushSpriteSheetApi(_lua, self->workspace, sprite_sheet);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSpriteSheet(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    if(std::shared_ptr<SpriteSheet> sprite_sheet = self->getStore(_lua)->getObject<SpriteSheet>(lua_tostring(_lua, 2)))
        pushSpriteSheetApi(_lua, self->workspace, sprite_sheet);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_CreateGraphicsPack(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    std::shared_ptr<GraphicsPack> graphics_pack = self->getStore(_lua)->createObject<GraphicsPack>(
        lua_tostring(_lua, 2),
        self->renderer);
    pushGraphicsPackApi(_lua, graphics_pack);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetGraphicsPack(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    if(std::shared_ptr<GraphicsPack> graphics_pack =
        self->getStore(_lua)->getObject<GraphicsPack>(lua_tostring(_lua, 2)))
    {
        pushGraphicsPackApi(_lua, graphics_pack);
    }
    else
    {
        lua_pushnil(_lua);
    }
    return 1;
}

// 1 self
// 2 key
// 3 body definition
int luaApi_CreateBodyPrototype(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    std::unique_ptr<BodyDefinition> definition = tryGetBodyDefinition(_lua, 3);
    luaL_argcheck(_lua, definition, 3, "a body definition expected");
    std::shared_ptr<BodyDefinition> shared_definition(definition.release());
    self->getStore(_lua)->createObject<BodyDefinition>(lua_tostring(_lua, 2), shared_definition);
    pushBodyPrototype(_lua, shared_definition);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetBodyPrototype(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    std::shared_ptr<BodyDefinition> definition = self->getStore(_lua)->getObject<BodyDefinition>(lua_tostring(_lua, 2));
    if(definition)
        pushBodyPrototype(_lua, definition);
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
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    luaL_argcheck(_lua, lua_isstring(_lua, 3), 3, "a font file path expected");
    const char * path = lua_tostring(_lua, 3);
    luaL_argcheck(_lua, lua_isinteger(_lua, 4), 4, "a font size expected");
    std::shared_ptr<TTF_Font> font = self->getStore(_lua)->createObject<TTF_Font>(
        lua_tostring(_lua, 2),
        self->workspace.getResourceFullPath(std::filesystem::path(path)),
        static_cast<uint16_t>(lua_tointeger(_lua, 4)));
    if(font)
        pushFontApi(_lua, font);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetFont(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    if(std::shared_ptr<TTF_Font> font = self->getStore(_lua)->getObject<TTF_Font>(lua_tostring(_lua, 2)))
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
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    luaL_argcheck(_lua, lua_isstring(_lua, 3), 3, "a sound effect file path expected");
    const char * path = lua_tostring(_lua, 3);
    std::shared_ptr<Mix_Chunk> chunk = self->getStore(_lua)->createObject<Mix_Chunk>(
        lua_tostring(_lua, 2),
        self->workspace.getResourceFullPath(std::filesystem::path(path)));
    if(chunk)
        pushSoundEffectApi(_lua, chunk);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetSoundEffect(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    if(std::shared_ptr<Mix_Chunk> chunk = self->getStore(_lua)->getObject<Mix_Chunk>(lua_tostring(_lua, 2)))
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
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    luaL_argcheck(_lua, lua_isstring(_lua, 3), 3, "a music file path expected");
    const char * path = lua_tostring(_lua, 3);
    std::shared_ptr<Mix_Music> chunk = self->getStore(_lua)->createObject<Mix_Music>(
        lua_tostring(_lua, 2),
        self->workspace.getResourceFullPath(std::filesystem::path(path)));
    if(chunk)
        pushMusicApi(_lua, chunk);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetMusic(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    if(std::shared_ptr<Mix_Music> chunk = self->getStore(_lua)->getObject<Mix_Music>(lua_tostring(_lua, 2)))
        pushMusicApi(_lua, chunk);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 key
template<typename T>
int luaApi_FreeObject(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, gc_message_key_required);
    bool result = self->getStore(_lua)->freeObject<T>(lua_tostring(_lua, 2));
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
            { "createView", luaApi_CreateView },
            { "getView", luaApi_GetView },
            { "freeView", luaApi_FreeObject<View> },
            { "createScene", luaApi_CreateScene },
            { "getScene", luaApi_GetScene },
            { "freeScene", luaApi_FreeObject<Scene> },
            { "createForm", luaApi_CreateForm },
            { "getForm", luaApi_GetForm },
            { "freeForm", luaApi_FreeObject<Form> },
            { "createSprite", luaApi_CreateSprite },
            { "getSprite", luaApi_GetSprite },
            { "freeSprite", luaApi_FreeObject<Sprite> },
            { "createSpriteSheet", luaApi_CreateSpriteSheet },
            { "getSpriteSheet", luaApi_GetSpriteSheet },
            { "freeSpriteSheet", luaApi_FreeObject<SpriteSheet> },
            { "createGraphicsPack", luaApi_CreateGraphicsPack },
            { "getGraphicsPack", luaApi_GetGraphicsPack },
            { "freeGraphicsPack", luaApi_FreeObject<GraphicsPack> },
            { "createBodyPrototype", luaApi_CreateBodyPrototype },
            { "getBodyPrototype", luaApi_GetBodyPrototype },
            { "freeeBodyDefinition", luaApi_FreeObject<BodyDefinition> },
            { "createSoundEffect", luaApi_CreateSoundEffect },
            { "getSoundEffect", luaApi_GetSoundEffect },
            { "freeSoundEffect", luaApi_FreeObject<Mix_Chunk> },
            { "createMusic", luaApi_CreateMusic },
            { "getMusic", luaApi_GetMusic },
            { "freeMusic", luaApi_FreeObject<Mix_Music> },
            { "createFont", luaApi_CreateFont },
            { "getFont", luaApi_GetFont },
            { "freeFont", luaApi_FreeObject<TTF_Font> },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
