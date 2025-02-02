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

#include <Sol2D/Lua/LuaSceneApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaGraphicsPackApi.h>
#include <Sol2D/Lua/LuaBodyDefinitionApi.h>
#include <Sol2D/Lua/LuaJointDefinitionApi.h>
#include <Sol2D/Lua/LuaBodyOptionsApi.h>
#include <Sol2D/Lua/LuaBodyApi.h>
#include <Sol2D/Lua/LuaJointApi.h>
#include <Sol2D/Lua/LuaContactApi.h>
#include <Sol2D/Lua/LuaTileMapObjectApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaCallbackStorage.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/Aux/LuaScript.h>
#include <sstream>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Lua;
using namespace Sol2D::Utils;

namespace {

const uint16_t gc_event_begin_contact = 0;
const uint16_t gc_event_end_contact = 1;
const uint16_t gc_event_begin_sensor_contact = 2;
const uint16_t gc_event_end_sensor_contact = 3;
const uint16_t gc_event_pre_solve_contact = 4;

const uint16_t gc_event_step = 0;

class LuaContactObserver : public ContactObserver, public ObjectCompanion
{
public:
    LuaContactObserver(lua_State * _lua, const Workspace & _workspace) :
        mp_lua(_lua),
        mr_workspace(_workspace)
    {
    }

    ~LuaContactObserver() override
    {
        LuaCallbackStorage(mp_lua).destroyCallbacks(this);
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

class LuaStepObserver : public StepObserver, public ObjectCompanion
{
public:
    LuaStepObserver(lua_State * _lua, const Workspace & _workspace) :
        mp_lua(_lua),
        mr_workspace(_workspace)
    {
    }

    ~LuaStepObserver() override
    {
        LuaCallbackStorage(mp_lua).destroyCallbacks(this);
    }

    void onStepComplete(const StepState & _state) override
    {
        lua_pushinteger(mp_lua, _state.time_passed.count());
        LuaCallbackStorage(mp_lua).execute(mr_workspace, this, gc_event_step, 1);
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
        m_scene(_scene),
        m_contact_observer_companion_id(null_companion_id),
        m_step_observer_companion_id(null_companion_id)
    {
    }

    ~Self() override
    {
        if(std::shared_ptr<Scene> scene = m_scene.lock())
        {
            scene->removeCompanion(m_contact_observer_companion_id);
            scene->removeCompanion(m_step_observer_companion_id);
        }
    }

    std::shared_ptr<Scene> getScene(lua_State * _lua) const
    {
        std::shared_ptr<Scene> ptr =  m_scene.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::scene_is_destroyed);
        return ptr;
    }

    uint32_t subscribeOnContact(lua_State * _lua, uint16_t _event_id, int _callback_idx);
    void unsubscribeOnContact(lua_State * _lua, uint16_t _event_id, int _subscription_id);
    uint32_t subscribeOnStep(lua_State * _lua, int _callback_idx);
    void unsubscribeOnStep(lua_State * _lua, int _subscription_id);

private:
    template<typename ObserverType>
    uint32_t subscribe(lua_State * _lua, uint16_t _event_id, uint64_t * _companion_id, int _callback_idx);

    template<typename ObserverType>
    void unsubscribe(lua_State * _lua, uint16_t _event_id, uint64_t _companion_id, uint32_t _subscription_id);

public:
    const Workspace & workspace;

private:
    std::weak_ptr<Scene> m_scene;
    uint64_t m_contact_observer_companion_id;
    uint64_t m_step_observer_companion_id;
};

inline uint32_t Self::subscribeOnContact(lua_State * _lua, uint16_t _event_id, int _callback_idx)
{
    return subscribe<LuaContactObserver>(_lua, _event_id, &m_contact_observer_companion_id, _callback_idx);
}

inline void Self::unsubscribeOnContact(lua_State * _lua, uint16_t _event_id, int _subscription_id)
{
    unsubscribe<LuaContactObserver>(_lua, _event_id, m_contact_observer_companion_id, _subscription_id);
}

uint32_t Self::subscribeOnStep(lua_State * _lua, int _callback_idx)
{
    return subscribe<LuaStepObserver>(_lua, gc_event_step, &m_step_observer_companion_id, _callback_idx);
}

inline void Self::unsubscribeOnStep(lua_State * _lua, int _subscription_id)
{
    unsubscribe<LuaStepObserver>(_lua, gc_event_step, m_step_observer_companion_id, _subscription_id);
}

template<typename ObserverType>
uint32_t Self::subscribe(lua_State * _lua, uint16_t _event_id, uint64_t * _companion_id, int _callback_idx)
{
    std::shared_ptr<Scene> scene = getScene(_lua);
    ObserverType * observer = static_cast<ObserverType *>(scene->getCompanion(*_companion_id));
    if(observer == nullptr)
    {
        observer = new ObserverType(_lua, workspace);
        *_companion_id = scene->addCompanion(std::unique_ptr<ObjectCompanion>(observer));
        scene->addObserver(*observer);
    }
    return LuaCallbackStorage(_lua).addCallback(observer, _event_id, _callback_idx);
}

template<typename ObserverType>
void Self::unsubscribe(lua_State * _lua, uint16_t _event_id, uint64_t _companion_id, uint32_t _subscription_id)
{
    std::shared_ptr<Scene> scene = getScene(_lua);
    ObserverType * observer = static_cast<ObserverType *>(scene->getCompanion(_companion_id));
    if(observer == nullptr)
        return;
    if(LuaCallbackStorage(_lua).removeCallback(observer, _event_id, _subscription_id) == 0)
    {
        scene->removeObserver(*observer);
        scene->removeCompanion(_companion_id);
    }
}

using UserData = LuaUserData<Self, LuaTypeName::scene>;

// 1 self
// 2 gravity
int luaApi_SetGravity(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    SDL_FPoint gravity;
    luaL_argexpected(_lua, tryGetPoint(_lua, 2, gravity), 2, LuaTypeName::point);
    self->getScene(_lua)->setGravity(gravity);
    return 1;
}

// 1 self
// 2 file path
int luaApi_LoadTileMap(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isstring(_lua, 2), 2, LuaTypeName::string);
    const char * path = lua_tostring(_lua, 2);
    bool result = self->getScene(_lua)->loadTileMap(self->workspace.getResourceFullPath(path));
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 object id
int luaApi_GetTileMapObjectById(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    const Tiles::TileMapObject * object = self
        ->getScene(_lua)
        ->getTileMapObjectById(static_cast<uint32_t>(lua_tointeger(_lua, 2)));
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
    luaL_argexpected(_lua, lua_isstring(_lua, 2), 2, LuaTypeName::string);
    const char * name = lua_tostring(_lua, 2);
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
    luaL_argexpected(_lua, lua_isstring(_lua, 2), 2, LuaTypeName::string);
    const char * class_name = lua_tostring(_lua, 2);
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
// 3 body definition
// 4 script path (optional)
// 5 script argument (optional)
int luaApi_CreateBody(lua_State * _lua)
{
    bool has_script_argument = lua_gettop(_lua) >= 5;
    const Self * self = UserData::getUserData(_lua, 1);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    SDL_FPoint position = { .0f, .0f };
    if(!lua_isnil(_lua, 2))
        luaL_argexpected(_lua, tryGetPoint(_lua, 2, position), 2, LuaTypeName::point);
    uint64_t body_id = 0;
    std::optional<std::filesystem::path> script_path;
    std::unique_ptr<BodyDefinition> definition = tryGetBodyDefinition(_lua, 3);
    luaL_argexpected(_lua, definition, 3, LuaTypeName::body_definition);
    body_id = scene->createBody(position, *definition);
    if(lua_isstring(_lua, 4))
    {
        const char * script_path = lua_tostring(_lua, 4);
        LuaTable table = LuaTable::pushNew(_lua);
        pushBodyApi(_lua, scene, body_id);
        table.setValueFromTop("body");
        lua_pushvalue(_lua, 1);
        table.setValueFromTop("scene");
        if(has_script_argument) {
            lua_pushvalue(_lua, 5);
            table.setValueFromTop("arg");
        }
        executeScriptWithContext(_lua, self->workspace, script_path);
    }
    pushBodyApi(_lua, scene, body_id);
    return 1;
}

// 1 self
// 2 body or body id
int luaApi_DestroyBody(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    uint64_t body_id;
    if(lua_isinteger(_lua, 2))
        body_id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    else if(!tryGetBodyId(_lua, 2, &body_id))
        luaL_argexpected(_lua, false, 2, LuaTypeName::joinTypes(LuaTypeName::body, LuaTypeName::integer).c_str());
    lua_pushboolean(_lua, self->getScene(_lua)->destroyBody(body_id));
    return 1;
}

// 1 self
// 2 body id
int luaApi_GetBody(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
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
    luaL_argexpected(_lua, lua_isstring(_lua, 2), 2, LuaTypeName::string);
    const char * class_name = lua_tostring(_lua, 2);    
    BodyOptions options;
    if(lua_gettop(_lua) >= 3)
        luaL_argexpected(_lua, tryGetBodyOptions(_lua, 3, options), 3, LuaTypeName::body_options);
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
        luaL_argexpected(_lua, false, 2, LuaTypeName::joinTypes(LuaTypeName::body, LuaTypeName::integer).c_str());
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
    luaL_argexpected(_lua, lua_isfunction(_lua, 2), 2, LuaTypeName::function);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_begin_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromBeginContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_begin_contact, subscription_id);
    return 0;
}

// 1 self
// 2 callback
int luaApi_SubscribeToEndContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isfunction(_lua, 2), 2, LuaTypeName::function);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_end_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromEndContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_end_contact, subscription_id);
    return 0;
}

// 1 self
// 2 callback
int luaApi_SubscribeToSensorBeginContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isfunction(_lua, 2), 2, LuaTypeName::function);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_begin_sensor_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromSensorBeginContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_begin_sensor_contact, subscription_id);
    return 0;
}

// 1 self
// 2 callback
int luaApi_SubscribeToSensorEndContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isfunction(_lua, 2), 2, LuaTypeName::function);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_end_sensor_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromSesnsorEndContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_end_sensor_contact, subscription_id);
    return 0;
}

// 1 self
// 2 callback
int luaApi_SubscribeToPreSolveContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isfunction(_lua, 2), 2, LuaTypeName::function);
    uint32_t id = self->subscribeOnContact(_lua, gc_event_pre_solve_contact, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromPreSolveContact(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnContact(_lua, gc_event_pre_solve_contact, subscription_id);
    return 0;
}

// 1 self
// 2 callback
int luaApi_SubscribeToStep(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isfunction(_lua, 2), 2, LuaTypeName::function);
    uint32_t id = self->subscribeOnStep(_lua, 2);
    lua_pushinteger(_lua, id);
    return 1;
}

// 1 self
// 2 subscription ID
int luaApi_UnsubscribeFromStep(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint32_t subscription_id = static_cast<uint32_t>(lua_tointeger(_lua, 2));
    self->unsubscribeOnStep(_lua, subscription_id);
    return 0;
}

// 1 self
// 2 joint definition
int luaApi_CreateDistanceJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    DistanceJointDefenition definition;
    luaL_argexpected(
        _lua,
        tryGetDistanceJointDefenition(_lua, 2, definition),
        2,
        LuaTypeName::distance_joint_definition);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    uint64_t id = scene->createJoint(definition);
    pushJointApi(_lua, scene, scene->getDistanceJoint(id).value());
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreateMotorJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    MotorJointDefinition definition;
    luaL_argexpected(_lua, tryGetMotorJointDefinition(_lua, 2, definition), 2, LuaTypeName::motor_joint_definition);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    uint64_t id = scene->createJoint(definition);
    pushJointApi(_lua, scene, scene->getMotorJoint(id).value());
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreateMouseJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    MouseJointDefinition definition;
    luaL_argexpected(_lua, tryGetMouseJointDefinition(_lua, 2, definition), 2, LuaTypeName::mouse_joint_definition);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    uint64_t id = scene->createJoint(definition);
    pushJointApi(_lua, scene, scene->getMouseJoint(id).value());
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreatePrismaticJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    PrismaticJointDefinition definition;
    luaL_argexpected(
        _lua,
        tryGetPrismaticJointDefinition(_lua, 2, definition),
        2,
        LuaTypeName::prismatic_joint_definition);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    uint64_t id = scene->createJoint(definition);
    pushJointApi(_lua, scene, scene->getPrismaticJoint(id).value());
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreateWeldJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    WeldJointDefinition definition;
    luaL_argexpected(_lua, tryGetWeldJointDefinition(_lua, 2, definition), 2, LuaTypeName::weld_joint_definition);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    uint64_t id = scene->createJoint(definition);
    pushJointApi(_lua, scene, scene->getWeldJoint(id).value());
    return 1;
}

// 1 self
// 2 joint definition
int luaApi_CreateWheelJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    WheelJointDefinition definition;
    luaL_argexpected(_lua, tryGetWheelJointDefinition(_lua, 2, definition), 2, LuaTypeName::wheel_joint_definition);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    uint64_t id = scene->createJoint(definition);
    pushJointApi(_lua, scene, scene->getWheelJoint(id).value());
    return 1;
}

// 1 self
// 2 id
int luaApi_GetDistanceJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    std::optional<DistanceJoint> joint = scene->getDistanceJoint(static_cast<uint64_t>(lua_tointeger(_lua, 2)));
    if(joint.has_value())
        pushJointApi(_lua, scene, std::move(joint.value()));
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 id
int luaApi_GetMotorJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    std::optional<MotorJoint> joint = scene->getMotorJoint(static_cast<uint64_t>(lua_tointeger(_lua, 2)));
    if(joint.has_value())
        pushJointApi(_lua, scene, std::move(joint.value()));
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 id
int luaApi_GetMouseJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    std::optional<MouseJoint> joint = scene->getMouseJoint(static_cast<uint64_t>(lua_tointeger(_lua, 2)));
    if(joint.has_value())
        pushJointApi(_lua, scene, std::move(joint.value()));
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 id
int luaApi_GetPrismaticJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    std::optional<PrismaticJoint> joint = scene->getPrismaticJoint(static_cast<uint64_t>(lua_tointeger(_lua, 2)));
    if(joint.has_value())
        pushJointApi(_lua, scene, std::move(joint.value()));
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 id
int luaApi_GetWeldJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    std::optional<WeldJoint> joint = scene->getWeldJoint(static_cast<uint64_t>(lua_tointeger(_lua, 2)));
    if(joint.has_value())
        pushJointApi(_lua, scene, std::move(joint.value()));
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 id
int luaApi_GetWheelJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    std::shared_ptr<Scene> scene = self->getScene(_lua);
    std::optional<WheelJoint> joint = scene->getWheelJoint(static_cast<uint64_t>(lua_tointeger(_lua, 2)));
    if(joint.has_value())
        pushJointApi(_lua, scene, std::move(joint.value()));
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 joint or joint id
int luaApi_DestroyJoint(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    uint64_t id = 0;
    if(lua_isinteger(_lua, 2))
    {
        id = static_cast<uint64_t>(lua_tointeger(_lua, 2));
    }
    else if(Joint * joint = tryGetJoint(_lua, 2))
    {
        id = joint->getGid();
    }
    else
    {
        std::vector<const char *> all_joint_types = getAllJointTypes();
        std::stringstream ss;
        for(size_t i = 0; i < all_joint_types.size(); ++i)
        {
            if(i > 0) ss << ", ";
            ss << all_joint_types[i];
        }
        ss << ", " << LuaTypeName::integer;
        luaL_argexpected(_lua, false, 2, ss.str().c_str());
    }
    lua_pushboolean(_lua, self->getScene(_lua)->destroyJoint(id));
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
        luaL_argexpected(_lua, false, 2, LuaTypeName::joinTypes(LuaTypeName::body, LuaTypeName::integer).c_str());
    SDL_FPoint destination;
    luaL_argexpected(_lua, tryGetPoint(_lua, 3, destination), 3, LuaTypeName::string);
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
            { "destroyBody", luaApi_DestroyBody },
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
            { "unsubscribeFromPreSolveContact", luaApi_UnsubscribeFromPreSolveContact },
            { "subscribeToStep", luaApi_SubscribeToStep },
            { "unsubscribeFromStep", luaApi_UnsubscribeFromStep },
            { "createDistanceJoint", luaApi_CreateDistanceJoint },
            { "createMotorJoint", luaApi_CreateMotorJoint },
            { "createMouseJoint", luaApi_CreateMouseJoint },
            { "createPrismaticJoint", luaApi_CreatePrismaticJoint },
            { "createWeldJoint", luaApi_CreateWeldJoint },
            { "createWheelJoint", luaApi_CreateWheelJoint },
            { "getDistanceJoint", luaApi_GetDistanceJoint },
            { "getMotorJoint", luaApi_GetMotorJoint },
            { "getMouseJoint", luaApi_GetMouseJoint },
            { "getPrismaticJoint", luaApi_GetPrismaticJoint },
            { "getWeldJoint", luaApi_GetWeldJoint },
            { "getWheelJoint", luaApi_GetWheelJoint },
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
