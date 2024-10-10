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
#include <Sol2D/Lua/LuaGraphicsPackApi.h>
#include <Sol2D/Lua/LuaBodyPrototypeApi.h>
#include <Sol2D/Lua/LuaBodyDefinitionApi.h>
#include <Sol2D/Lua/LuaJointDefinitionApi.h>
#include <Sol2D/Lua/LuaBodyOptionsApi.h>
#include <Sol2D/Lua/LuaBodyApi.h>
#include <Sol2D/Lua/LuaContactApi.h>
#include <Sol2D/Lua/LuaTileMapObjectApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaCallbackStorage.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/Aux/LuaScript.h>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;
using namespace Sol2D::Utils;

namespace {

const char gc_message_body_id_expected[] = "body ID expected";
const char gc_message_body_or_body_id_expected[] = "body or body ID expected";
const char gc_message_joint_id_expected[] = "joint id expected";
const char gc_message_subscription_id_expected[] = "subscriptin ID expected";
const char gc_message_callback_expected[] = "callback expected";

const uint16_t gc_event_begin_contact = 0;
const uint16_t gc_event_end_contact = 1;
const uint16_t gc_event_begin_sensor_contact = 2;
const uint16_t gc_event_end_sensor_contact = 3;
const uint16_t gc_event_pre_solve_contact = 4;

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

    void beginContact(const Contact & _contact) override
    {
        pushContact(mp_lua, _contact);
        LuaCallbackStorage(mp_lua).execute(mr_workspace, this, gc_event_begin_contact, 1);
    }

    void endContact(const Contact & _contact) override
    {
        pushContact(mp_lua, _contact);
        LuaCallbackStorage(mp_lua).execute(mr_workspace, this, gc_event_end_contact, 1);
    }

    void beginSensorContact(const SensorContact & _contact) override
    {
        pushContact(mp_lua, _contact);
        LuaCallbackStorage(mp_lua).execute(mr_workspace, this, gc_event_begin_sensor_contact, 1);
    }

    void endSensorContact(const SensorContact & _contact) override
    {
        pushContact(mp_lua, _contact);
        LuaCallbackStorage(mp_lua).execute(mr_workspace, this, gc_event_end_sensor_contact, 1);
    }

    bool preSolveContact(const PreSolveContact & _contact) override
    {
        bool result = true;
        pushContact(mp_lua, _contact);
        LuaCallbackStorage(mp_lua).execute(mr_workspace, this, gc_event_pre_solve_contact, 1, 1, [this, &result]() {
            if(lua_isboolean(mp_lua, -1))
                result = lua_toboolean(mp_lua, -1) != 0;
            return result;
        });
        return result;
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
    const Self * self = UserData::getUserData(_lua, 1);
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
    const Self * self = UserData::getUserData(_lua, 1);
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
    const Self * self = UserData::getUserData(_lua, 1);
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
// 2 class
int luaApi_GetTileMapObjectsByClass(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    const char * class_name = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, class_name, 2, "class expected");
    auto objects = self->getScene(_lua)->getTileMapObjectsByClass(class_name);
    lua_newtable(_lua);
    int i = 0;
    for(const auto * object : objects)
    {
        pushTileMapObject(_lua, *object);
        lua_rawseti(_lua, -2, ++i);
    }
    return 1;
}

// 1 self
// 2 position or nil
// 3 body prototype OR body definition
// 4 script argument (optional)
int luaApi_CreateBody(lua_State * _lua)
{
    bool has_script_argument = lua_gettop(_lua) >= 4;
    const Self * self = UserData::getUserData(_lua, 1);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    Point position = { .0f, .0f };
    if(!lua_isnil(_lua, 2))
        luaL_argcheck(_lua, tryGetPoint(_lua, 2, position), 2, "body position expected");

    uint64_t body_id = 0;
    LuaBodyPrototype proto;
    std::optional<std::filesystem::path> script_path;
    if(tryGetBodyPrototype(_lua, 3, proto))
    {
        body_id = scene->createBody(position, *proto.definition);
        script_path = proto.definition->script;
    }
    else if(std::unique_ptr<BodyDefinition> definition = tryGetBodyDefinition(_lua, 3))
    {
        body_id = scene->createBody(position, *definition);
        script_path = definition->script;
    }
    else
    {
        luaL_argerror(_lua, 3, "body prototype or defenition expected");
    }

    if(script_path.has_value())
    {
        LuaTable table = LuaTable::pushNew(_lua);
        pushBodyApi(_lua, scene, body_id);
        table.setValueFromTop("body");
        lua_pushvalue(_lua, 1);
        table.setValueFromTop("scene");
        if(has_script_argument) {
            lua_pushvalue(_lua, 4);
            table.setValueFromTop("arg");
        }
        executeScriptWithContext(_lua, self->workspace, script_path.value());
    }

    pushBodyApi(_lua, scene, body_id);
    return 1;
}

// 1 self
// 2 body id
int luaApi_GetBody(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_body_id_expected);
    uint64_t body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    if(scene->doesBodyExist(body_id))
        pushBodyApi(_lua, scene, body_id);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 class
// 3 options (optional)
int luaApi_CreateBodiesFromMapObjects(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    const char * class_name = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, class_name != nullptr, 2, "a class name expected");
    BodyOptions options;
    if(lua_gettop(_lua) >= 3)
        luaL_argcheck(_lua, tryGetBodyOptions(_lua, 3, options), 3, "body options expected");
    self->getScene(_lua)->createBodiesFromMapObjects(class_name, options);
    return 0;
}

// 1 self
// 2 body id or body
int luaApi_SetFollowedBody(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    uint64_t body_id;
    if(lua_isinteger(_lua, 2))
        body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    else if(!tryGetBodyId(_lua, 2, &body_id))
        luaL_argerror(_lua, 2, gc_message_body_or_body_id_expected);
    lua_pushboolean(_lua, self->getScene(_lua)->setFollowedBody(body_id));
    return 1;
}

// 1 self
int luaApi_ResetFollowedBody(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    self->getScene(_lua)->resetFollowedBody();
    return 0;
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
    uint32_t id = self->subscribeOnContact(_lua, gc_event_end_contact, 2);
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
// 2 callback
int luaApi_SubscribeToSensorBeginContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isfunction(_lua, 2), 2, gc_message_callback_expected);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_begin_sensor_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromSensorBeginContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_subscription_id_expected);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_begin_sensor_contact, subscription_id);
    return 0;
}

// 1 self
// 2 callback
int luaApi_SubscribeToSensorEndContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isfunction(_lua, 2), 2, gc_message_callback_expected);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_end_sensor_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromSesnsorEndContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_subscription_id_expected);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_end_sensor_contact, subscription_id);
    return 0;
}

// 1 self
// 2 callback
int luaApi_SubscribeToPreSolveContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isfunction(_lua, 2), 2, gc_message_callback_expected);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_pre_solve_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribePreSolveContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_subscription_id_expected);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_pre_solve_contact, subscription_id);
    return 0;
}

// 1 self
// 2 joint definition
int luaApi_CreateDistanceJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    DistanceJointDefenition definition;
    luaL_argcheck(_lua, tryGetDistanceJointDefenition(_lua, 2, definition), 2, "distance joint definition expected");
    lua_pushinteger(_lua, self->getScene(_lua)->createJoint(definition));
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreateMotorJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    MotorJointDefinition definition;
    luaL_argcheck(_lua, tryGetMotorJointDefinition(_lua, 2, definition), 2, "motor joint definition expected");
    lua_pushinteger(_lua, self->getScene(_lua)->createJoint(definition));
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreateMouseJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    MouseJointDefinition definition;
    luaL_argcheck(_lua, tryGetMouseJointDefinition(_lua, 2, definition), 2, "mouse joint definition expected");
    lua_pushinteger(_lua, self->getScene(_lua)->createJoint(definition));
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreatePrismaticJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    PrismaticJointDefinition definition;
    luaL_argcheck(_lua, tryGetPrismaticJointDefinition(_lua, 2, definition), 2, "prismatic joint definition expected");
    lua_pushinteger(_lua, self->getScene(_lua)->createJoint(definition));
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreateWeldJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    WeldJointDefinition definition;
    luaL_argcheck(_lua, tryGetWeldJointDefinition(_lua, 2, definition), 2, "weld joint definition expected");
    lua_pushinteger(_lua, self->getScene(_lua)->createJoint(definition));
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreateWheelJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    WheelJointDefinition definition;
    luaL_argcheck(_lua, tryGetWheelJointDefinition(_lua, 2, definition), 2, "wheel joint definition expected");
    lua_pushinteger(_lua, self->getScene(_lua)->createJoint(definition));
    return 1;
}

// 1 self
// 2 joint id
int luaApi_DestroyJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_joint_id_expected);
    uint64_t id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    lua_pushboolean(_lua, self->getScene(_lua)->destroyJoing(id));
    return 1;
}

// 1 self
// 2 body id | body
// 3 destination
// TODO: options
int luaApi_FindPath(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    uint64_t body_id;
    if(lua_isinteger(_lua, 2))
        body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    else if(!tryGetBodyId(_lua, 2, &body_id))
        luaL_argerror(_lua, 2, gc_message_body_or_body_id_expected);
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
            { "getTileMapObjectsByClass", luaApi_GetTileMapObjectsByClass },
            { "createBody", luaApi_CreateBody },
            { "getBody", luaApi_GetBody },
            { "createBodiesFromMapObjects", luaApi_CreateBodiesFromMapObjects },
            { "setFollowedBody", luaApi_SetFollowedBody },
            { "resetFollowedBody", luaApi_ResetFollowedBody },
            { "subscribeToBeginContact", luaApi_SubscribeToBeginContact },
            { "unsubscribeFromBeginContact", luaApi_UnsubscribeFromBeginContact },
            { "subscribeToEndContact", luaApi_SubscribeToEndContact },
            { "unsubscribeFromEndContact", luaApi_UnsubscribeFromEndContact },
            { "subscribeToSensorBeginContact", luaApi_SubscribeToSensorBeginContact },
            { "unsubscribeFromSensorBeginContact", luaApi_UnsubscribeFromSensorBeginContact },
            { "subscribeToSensorEndContact", luaApi_SubscribeToSensorEndContact },
            { "unsubscribeFromSesnsorEndContact", luaApi_UnsubscribeFromSesnsorEndContact },
            { "subscribeToPreSolveContact", luaApi_SubscribeToPreSolveContact },
            { "unsubscribePreSolveContact", luaApi_UnsubscribePreSolveContact },
            { "createDistanceJoint", luaApi_CreateDistanceJoint },
            { "createMotorJoint", luaApi_CreateMotorJoint },
            { "createMouseJoint", luaApi_CreateMouseJoint },
            { "createPrismaticJoint", luaApi_CreatePrismaticJoint },
            { "createWeldJoint", luaApi_CreateWeldJoint },
            { "createWheelJoint", luaApi_CreateWheelJoint },
            { "destroyJoint", luaApi_DestroyJoint },
            { "findPath", luaApi_FindPath },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<Scene> Sol2D::Lua::tryGetScene(lua_State * _lua, int _idx)
{
    if(const Self * self = UserData::tryGetUserData(_lua, _idx))
        return self->getScene(_lua);
    return nullptr;
}
