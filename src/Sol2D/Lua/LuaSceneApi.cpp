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
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaCallbackStorage.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/Aux/LuaScript.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_message_body_id_expected[] = "a body ID expected";
const char gc_message_shape_key_expected[] = "a shape key expected";
const char gc_message_graphic_key_expected[] = "a graphic key expected";
const char gc_message_subscription_id_expected[] = "a subscriptin ID expected";
const char gc_message_callback_expected[] = "callback expected";

const uint16_t gc_event_begin_contact = 0;
const uint16_t gc_event_end_contact = 1;

const char gc_contact_companion_key[] = "lua_contact_observer";

class LuaContactObserver : public ObjectCompanion, public ContactObserver
{
public:
    LuaContactObserver(lua_State * _lua, const Workspace & _workspace) :
        mp_lua(_lua),
        mr_workspace(_workspace)
    {
    }

    ~LuaContactObserver() override
    {
        LuaCallbackStorage storage(mp_lua);
        storage.destroyCallbacks(this);
    }

    void beginContact(Contact & _contact) override
    {
        pushContact(mp_lua, _contact);
        LuaCallbackStorage(mp_lua).execute(mr_workspace, this, gc_event_begin_contact, 1);
    }

    void endContact(Contact & _contact) override
    {
        pushContact(mp_lua, _contact);
        LuaCallbackStorage(mp_lua).execute(mr_workspace, this, gc_event_end_contact, 1);
    }

private:
    lua_State * mp_lua;
    const Workspace & mr_workspace;
};

struct Self : LuaSelfBase
{
public:
    Self(const Workspace & _workspace, std::shared_ptr<Scene> & _scene) :
        workspace(_workspace),
        m_scene(_scene)
    {
    }

    std::shared_ptr<Scene> getScene(lua_State * _lua) const
    {
        std::shared_ptr<Scene> ptr =  m_scene.lock();
        if(!ptr)
            luaL_error(_lua, "the scene is destroyed");
        return ptr;
    }

    uint32_t subscribeOnContact(lua_State * _lua, uint16_t _event_id, int _callback_idx);
    void unsubscribeOnContact(lua_State * _lua, uint16_t _event_id, int _subscription_id);

public:
    const Workspace & workspace;

private:
    std::weak_ptr<Scene> m_scene;
};

uint32_t Self::subscribeOnContact(lua_State * _lua, uint16_t _event_id, int _callback_idx)
{
    std::shared_ptr<Scene> scene = getScene(_lua);
    ObjectCompanion * owner = scene->getCompanion(gc_contact_companion_key);
    if(owner == nullptr)
    {
        LuaContactObserver * observer = new LuaContactObserver(_lua, workspace);
        scene->addCompanion(gc_contact_companion_key, std::unique_ptr<ObjectCompanion>(observer));
        scene->addObserver(*observer);
        owner = observer;
    }
    return LuaCallbackStorage(_lua).addCallback(owner, _event_id, _callback_idx);
}

void Self::unsubscribeOnContact(lua_State * _lua, uint16_t _event_id, int _subscription_id)
{
    std::shared_ptr<Scene> scene = getScene(_lua);
    ObjectCompanion * owner = scene->getCompanion(gc_contact_companion_key);
    if(owner == nullptr)
        return;
    if(LuaCallbackStorage(_lua).removeCallback(owner, _event_id, _subscription_id) == 0)
    {
        scene->removeObserver(*static_cast<LuaContactObserver *>(owner));
        scene->removeCompanion(gc_contact_companion_key);
    }
}

using UserData = LuaUserData<Self, LuaTypeName::scene>;

// 1 self
// 2 gravity
int luaApi_SetGravity(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    Point gravity;
    luaL_argcheck(_lua, tryGetPoint(_lua, 2, gravity), 2, "gravity vector expected");
    self->getScene(_lua)->setGravity(gravity);
    return 1;
}

// 1 self
// 2 file path
int luaApi_LoadTileMap(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * path = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, path != nullptr, 2, "a file path expected");
    bool result = self->getScene(_lua)->loadTileMap(self->workspace.getResourceFullPath(path));
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 object id
int luaApi_GetTileMapObjectById(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "object id expected");
    const Tiles::TileMapObject * object = self->getScene(_lua)->getTileMapObjectById(static_cast<uint32_t>(lua_tointeger(_lua, 2)));
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
    Self * self = UserData::getUserData(_lua, 1);
    const char * name = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, name, 2, "object name expected");
    const Tiles::TileMapObject * object = self->getScene(_lua)->getTileMapObjectByName(name);
    if(object)
        pushTileMapObject(_lua, *object);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 position or nil
// 3 body prototype
// 4 script argument (optional)
int luaApi_CreateBody(lua_State * _lua)
{
    bool has_script_argument = lua_gettop(_lua) >= 4;
    Self * self = UserData::getUserData(_lua, 1);
    Point position = { .0f, .0f };
    if(!lua_isnil(_lua, 2))
        luaL_argcheck(_lua, tryGetPoint(_lua, 2, position), 2, "body position expected");
    std::optional<LuaBodyPrototype> lua_proto = tryGetBodyPrototype(_lua, 3);
    if(!lua_proto.has_value())
        luaL_argerror(_lua, 3, "invalid body brototype");
    uint64_t body_id = self->getScene(_lua)->createBody(position, *lua_proto->proto);
    lua_pushinteger(_lua, body_id);

    if(lua_proto->script_path.has_value())
    {
        LuaTable table = LuaTable::pushNew(_lua);
        table.setIntegerValue("bodyId", body_id);
        lua_pushvalue(_lua, 1);
        table.setValueFromTop("scene");
        if(has_script_argument) {
            lua_pushvalue(_lua, 4);
            table.setValueFromTop("arg");
        }
        executeScriptWithContext(_lua, self->workspace, lua_proto->script_path.value());
    }

    return 1;
}

// 1 self
// 2 class
// 3 body options (optional)
// 4 shape options (optional)
int luaApi_CreateBodiesFromMapObjects(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
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
    self->getScene(_lua)->createBodiesFromMapObjects(class_name, body_options, body_shape_options);
    return 0;
}

// 1 self
// 2 body id
// 3 force vector (point)
int luaApi_ApplyForce(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    Point force;
    luaL_argcheck(_lua, tryGetPoint(_lua, 3, force), 3, "a force vector expected");
    self->getScene(_lua)->applyForce(body_id, force);
    return 0;
}

// 1 self
// 2 body id
// 3 position
int luaApi_SetBodyPosition(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    Point position;
    luaL_argcheck(_lua, tryGetPoint(_lua, 3, position), 3, "a position expected");
    self->getScene(_lua)->setBodyPosition(body_id, position);
    return 0;
}

// 1 self
// 2 body id
int luaApi_GetBodyPosition(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    std::optional<Point> position = self->getScene(_lua)->getBodyPosition(body_id);
    if(position.has_value())
        pushPoint(_lua, position.value().x, position.value().y);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 body id
int luaApi_SetFollowedBody(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    lua_pushboolean(_lua, self->getScene(_lua)->setFollowedBody(body_id));
    return 1;
}

// 1 self
int luaApi_ResetFollowedBody(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    self->getScene(_lua)->resetFollowedBody();
    return 0;
}

// 1 self
// 2 body id
// 3 layer
int luaApi_SetBodyLayer(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    const char * layer = lua_tostring(_lua, 3);
    luaL_argcheck(_lua, layer != nullptr, 3, "a layer name expected");
    lua_pushboolean(_lua, self->getScene(_lua)->setBodyLayer(body_id, layer));
    return 1;
}

// 1 self
// 2 body id
// 3 shape key
// 4 graphic key
int luaApi_SetBodyShapeCurrentGraphic(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    const char * shape_id = lua_tostring(_lua, 3);
    luaL_argcheck(_lua, shape_id != nullptr, 3, gc_message_shape_key_expected);
    const char * graphic_id = lua_tostring(_lua, 4);
    luaL_argcheck(_lua, graphic_id != nullptr, 4, gc_message_graphic_key_expected);
    lua_pushboolean(_lua, self->getScene(_lua)->setBodyShapeCurrentGraphic(body_id, shape_id, graphic_id));
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
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    const char * shape_id = lua_tostring(_lua, 3);
    luaL_argcheck(_lua, shape_id != nullptr, 3, gc_message_shape_key_expected);
    const char * graphic_id = lua_tostring(_lua, 4);
    luaL_argcheck(_lua, graphic_id != nullptr, 4, gc_message_graphic_key_expected);
    luaL_argexpected(_lua, lua_isboolean(_lua, 5), 5, "boolean");
    luaL_argexpected(_lua, lua_isboolean(_lua, 6), 6, "boolean");
    bool result = self->getScene(_lua)->flipBodyShapeGraphic(
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
int luaApi_SubscribeToBeginContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isfunction(_lua, 2), 2, gc_message_callback_expected);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_begin_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromBeginContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_subscription_id_expected);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_begin_contact, subscription_id);
    return 0;
}

// 1 self
// 2 callback
int luaApi_SubscribeToEndContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isfunction(_lua, 2), 2, gc_message_callback_expected);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_begin_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromEndContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_subscription_id_expected);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_end_contact, subscription_id);
    return 0;
}

// 1 self
// 2 body id
// 3 destination
// TODO: options
int luaApi_FindPath(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    Point destination;
    luaL_argcheck(_lua, tryGetPoint(_lua, 3, destination), 3, "a destination point expected");
    auto result = self->getScene(_lua)->findPath(body_id, destination, false, false);
    if(result.has_value())
    {
        lua_newtable(_lua);
        for(size_t i = 0; i < result.value().size(); ++i)
        {
            pushPoint(_lua, result.value()[i].x, result.value()[i].y);
            lua_rawseti(_lua, -2, i + 1);
        }
    }
    else
    {
        lua_pushnil(_lua);
    }
    return 1;
}

} // namespace

void Sol2D::Lua::pushSceneApi(lua_State * _lua, const Workspace & _workspace, std::shared_ptr<Scene> _scene)
{
    UserData::pushUserData(_lua, _workspace, _scene);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "setGravity", luaApi_SetGravity },
            { "loadTileMap", luaApi_LoadTileMap },
            { "getTileMapObjectById", luaApi_GetTileMapObjectById },
            { "getTileMapObjectByName", luaApi_GetTileMapObjectByName },
            { "createBody", luaApi_CreateBody },
            { "createBodiesFromMapObjects", luaApi_CreateBodiesFromMapObjects },
            { "applyForce", luaApi_ApplyForce },
            { "setBodyPosition", luaApi_SetBodyPosition },
            { "getBodyPosition", luaApi_GetBodyPosition },
            { "setFollowedBody", luaApi_SetFollowedBody },
            { "resetFollowedBody", luaApi_ResetFollowedBody },
            { "setBodyLayer", luaApi_SetBodyLayer },
            { "setBodyShapeCurrentGraphic", luaApi_SetBodyShapeCurrentGraphic },
            { "flipBodyShapeGraphic", luaApi_FlipBodyShapeGraphic },
            { "subscribeToBeginContact", luaApi_SubscribeToBeginContact },
            { "unsubscribeFromBeginContact", luaApi_UnsubscribeFromBeginContact },
            { "subscribeToEndContact", luaApi_SubscribeToEndContact },
            { "unsubscribeFromEndContact", luaApi_UnsubscribeFromEndContact },
            { "findPath", luaApi_FindPath },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<Scene> Sol2D::Lua::tryGetScene(lua_State * _lua, int _idx)
{
    if(Self * self = UserData::tryGetUserData(_lua, _idx))
        return self->getScene(_lua);
    return nullptr;
}
