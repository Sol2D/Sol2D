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

#include <Sol2D/Lua/LuaSceneApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaBodyPrototypeApi.h>
#include <Sol2D/Lua/LuaBodyOptionsApi.h>
#include <Sol2D/Lua/LuaBodyShapeOptionsApi.h>
#include <Sol2D/Lua/LuaContactApi.h>
#include <Sol2D/Lua/LuaTileMapObjectApi.h>
#include <Sol2D/Lua/LuaAux.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

char __g_scene_weak_registry_key[] = "";
constexpr void * gc_scene_weak_registry_key = static_cast<void *>(__g_scene_weak_registry_key);

const char gc_metatable_scene[] = "sol.Scene";
const char gc_message_body_id_expected[] = "a body ID expected";
const char gc_message_shape_key_expected[] = "a shape key expected";
const char gc_message_graphic_key_expected[] = "a graphic key expected";

struct Self : LuaUserData<Self, gc_metatable_scene>
{
    Scene * scene;
    const Workspace * workspace;
    ContactObserver * contact_observer;
    int key_observers_begin_contact;
    int key_observers_end_contact;
};

class LuaContactObserver : public ContactObserver
{
public:
    LuaContactObserver(lua_State * _lua, const Workspace & _workspace, int _key_begin_contact, int _key_end_contact);
    void beginContact(Contact & _contact);
    void endContact(Contact & _contact);

private:
    void callCallbacks(int _registry_key, Contact & _contact);

private:
    lua_State * mp_lua;
    const Workspace & mr_workspace;
    int m_key_begin_contact;
    int m_key_end_contact;
};

LuaContactObserver::LuaContactObserver(
    lua_State * _lua,
    const Workspace & _workspace,
    int _key_begin_contact,
    int _key_end_contact
) :
    mp_lua(_lua),
    mr_workspace(_workspace),
    m_key_begin_contact(_key_begin_contact),
    m_key_end_contact(_key_end_contact)
{
}

void LuaContactObserver::beginContact(Contact & _contact)
{
    callCallbacks(m_key_begin_contact, _contact);
}

void LuaContactObserver::endContact(Contact & _contact)
{
    callCallbacks(m_key_end_contact, _contact);
}

void LuaContactObserver::callCallbacks(int _registry_key, Contact & _contact)
{
    if(lua_rawgeti(mp_lua, LUA_REGISTRYINDEX, _registry_key) != LUA_TTABLE)
        return;
    lua_Integer count = static_cast<lua_Integer>(lua_rawlen(mp_lua, -1));
    if(count == 0)
        return;
    pushContact(mp_lua, _contact);
    int contact_idx = lua_gettop(mp_lua);
    for(lua_Integer i = 1; i <= count; ++i)
    {
        lua_rawgeti(mp_lua, -2, i);
        if(!lua_isfunction(mp_lua, -1))
        {
            lua_pop(mp_lua, 1);
            continue;
        }
        lua_pushvalue(mp_lua, contact_idx);
        if(lua_pcall(mp_lua, 1, 0, 0) != LUA_OK)
        {
            mr_workspace.getMainLogger().error(lua_tostring(mp_lua, -1));
            lua_pop(mp_lua, 1);
        }
    }
    lua_pop(mp_lua, 1);
}

// 1 self
int luaApi_GC(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_unref(_lua, LUA_REGISTRYINDEX, self->key_observers_begin_contact);
    luaL_unref(_lua, LUA_REGISTRYINDEX, self->key_observers_end_contact);
    self->scene->removeContactObserver(*self->contact_observer);
    delete self->contact_observer;
    return 0;
}

// 1 self
// 2 file path
int luaApi_LoadTileMap(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * path = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, path != nullptr, 2, "a file path expected");
    bool result = self->scene->loadTileMap(self->workspace->toAbsolutePath(path));
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 object id
int luaApi_GetTileMapObjectById(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "object id expected");
    const Tiles::TileMapObject * object = self->scene->getTileMapObjectById(static_cast<uint32_t>(lua_tointeger(_lua, 2)));
    if(object)
        pushTileMapObject(_lua, *object);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 object name
int luaApi_GetTileMapObjectByName(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * name = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, name, 2, "object name expected");
    const Tiles::TileMapObject * object = self->scene->getTileMapObjectByName(name);
    if(object)
        pushTileMapObject(_lua, *object);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 position
// 3 body prototype
int luaApi_CreateBody(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    SDL_FPoint position;
    luaL_argcheck(_lua, tryGetPoint(_lua, 2, position), 2, "body position expected");
    LuaBodyPrototype & lua_proto = getBodyPrototype(_lua, 3);
    uint64_t body_id = self->scene->createBody(position, lua_proto.proto);
    lua_pushinteger(_lua, body_id);

    if(lua_proto.script_path.has_value())
    {
        lua_pushinteger(_lua, body_id);
        executeScriptWithContext(_lua, *self->workspace, lua_proto.script_path.value());
    }

    return 1;
}

// 1 self
// 2 class
// 3 body options (optional)
// 4 shape options (optional)
int luaApi_CreateBodiesFromMapObjects(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    const char * class_name = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, class_name != nullptr, 2, "a class name expected");

    int arg_count = lua_gettop(_lua);
    BodyOptions body_options;
    BodyShapeOptions body_shape_options;
    if(arg_count >= 3)
    {
        tryGetBodyOptions(_lua, 3, body_options);
        if(arg_count >= 4)
            tryGetBodyShapeOptions(_lua, 4, body_shape_options);
    }
    self->scene->createBodiesFromMapObjects(class_name, body_options, body_shape_options);
    return 0;
}

// 1 self
// 2 body id
// 3 force vector (point)
int luaApi_ApplyForce(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    SDL_FPoint force;
    luaL_argcheck(_lua, tryGetPoint(_lua, 3, force), 3, "a force vector expected");
    self->scene->applyForce(body_id, force);
    return 0;
}

// 1 self
// 2 body id
// 3 position
int luaApi_SetBodyPosition(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    SDL_FPoint position;
    luaL_argcheck(_lua, tryGetPoint(_lua, 3, position), 3, "a position expected");
    self->scene->setBodyPosition(body_id, position);
    return 0;
}

// 1 self
// 2 body id
int luaApi_SetFolowedBody(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    lua_pushboolean(_lua, self->scene->setFollowedBody(body_id));
    return 1;
}

// 1 self
int luaApi_ResetFolowedBody(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    self->scene->resetFollowedBody();
    return 0;
}

// 1 self
// 2 body id
// 3 layer
int luaApi_SetBodyLayer(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    const char * layer = lua_tostring(_lua, 3);
    luaL_argcheck(_lua, layer != nullptr, 3, "a layer name expected");
    lua_pushboolean(_lua, self->scene->setBodyLayer(body_id, layer));
    return 1;
}

// 1 self
// 2 body id
// 3 shape key
// 4 graphic key
int luaApi_SetBodyShapeCurrentGraphic(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    const char * shape_id = lua_tostring(_lua, 3);
    luaL_argcheck(_lua, shape_id != nullptr, 3, gc_message_shape_key_expected);
    const char * graphic_id = lua_tostring(_lua, 4);
    luaL_argcheck(_lua, graphic_id != nullptr, 4, gc_message_graphic_key_expected);
    lua_pushboolean(_lua, self->scene->setBodyShapeCurrentGraphic(body_id, shape_id, graphic_id));
    return 1;
}

// 1 self
// 2 body id
// 3 shape key
// 4 graphic key
// 5 flip horizontally
// 6 flip vertically
int luaApi_FlipBodyShapeGraphic(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    const char * shape_id = lua_tostring(_lua, 3);
    luaL_argcheck(_lua, shape_id != nullptr, 3, gc_message_shape_key_expected);
    const char * graphic_id = lua_tostring(_lua, 4);
    luaL_argcheck(_lua, graphic_id != nullptr, 4, gc_message_graphic_key_expected);
    luaL_argexpected(_lua, lua_isboolean(_lua, 5), 5, "boolean");
    luaL_argexpected(_lua, lua_isboolean(_lua, 6), 6, "boolean");
    bool result = self->scene->flipBodyShapeGraphic(
        body_id,
        shape_id,
        graphic_id,
        lua_toboolean(_lua, 5),
        lua_toboolean(_lua, 6));
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 callback
int registerCallback(lua_State * _lua, int _storage_key)
{
    luaL_argcheck(_lua, lua_isfunction(_lua, 2), 2, "callback expected");
    if(lua_rawgeti(_lua, LUA_REGISTRYINDEX, _storage_key) == LUA_TTABLE)
    {
        lua_Integer count = static_cast<lua_Integer>(lua_rawlen(_lua, -1));
        lua_pushvalue(_lua, 2);
        lua_rawseti(_lua, -2, count + 1);
    }
    lua_pop(_lua, 1);
    return 0;
}

// 1 self
// 2 callback
int luaApi_SubscribeToBeginContact(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    return registerCallback(_lua, self->key_observers_begin_contact);
}

int luaApi_SubscribeToEndContact(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    return registerCallback(_lua, self->key_observers_end_contact);
}

} // namespace

void Sol2D::Lua::pushSceneApi(lua_State * _lua, const Workspace & _workspace, Scene & _scene)
{
    WeakRegistryStorage weak_registry(_lua, gc_scene_weak_registry_key);
    if(weak_registry.tryGet(&_scene, LUA_TUSERDATA))
        return;

    Self * self = Self::pushUserData(_lua);
    self->scene = &_scene;
    self->workspace = &_workspace;
    lua_newtable(_lua);
    self->key_observers_begin_contact = luaL_ref(_lua, LUA_REGISTRYINDEX);
    lua_newtable(_lua);
    self->key_observers_end_contact = luaL_ref(_lua, LUA_REGISTRYINDEX);
    self->contact_observer = new LuaContactObserver(
        _lua,
        _workspace,
        self->key_observers_begin_contact,
        self->key_observers_end_contact);
    self->scene->addContactObserver(*self->contact_observer);
    if(Self::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "__gc", luaApi_GC, },
            { "loadTileMap", luaApi_LoadTileMap },
            { "getTileMapObjectById", luaApi_GetTileMapObjectById },
            { "getTileMapObjectByName", luaApi_GetTileMapObjectByName },
            { "createBody", luaApi_CreateBody },
            { "createBodiesFromMapObjects", luaApi_CreateBodiesFromMapObjects },
            { "applyForce", luaApi_ApplyForce },
            { "setBodyPosition", luaApi_SetBodyPosition },
            { "setFolowedBody", luaApi_SetFolowedBody },
            { "resetFolowedBody", luaApi_ResetFolowedBody },
            { "setBodyLayer", luaApi_SetBodyLayer },
            { "setBodyShapeCurrentGraphic", luaApi_SetBodyShapeCurrentGraphic },
            { "flipBodyShapeGraphic", luaApi_FlipBodyShapeGraphic },
            { "subscribeToBeginContact", luaApi_SubscribeToBeginContact },
            { "subscribeToEndContact", luaApi_SubscribeToEndContact },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);

    weak_registry.save(&_scene, -1);
}
