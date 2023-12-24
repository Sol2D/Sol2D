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

#include <Sol2D/Lua/LuaLevelApi.h>
#include <Sol2D/Lua/LuaBodyPrototypeApi.h>
#include <Sol2D/Lua/LuaLibrary.h>
#include <Sol2D/Lua/LuaBodyApi.h>
#include <Sol2D/Lua/LuaLevelObjectApi.h>
#include <Sol2D/Def.h>

using namespace Sol2D;
using namespace Sol2D::Tiles;
using namespace Sol2D::Lua;

namespace {

const char * gc_mtable_level = "sol.Level";
const char * gc_key_func_on_body_contacted = "onBodyContacted";
const char * gc_key_func_on_body_discontacted = "onBodyDicontacted";

void luaPushLevelApiMetatableOntoStack(lua_State * _lua);
int luaApi_Destroy(lua_State * _lua);
int luaApi_Move(lua_State * _lua);
int luaApi_CreateBody(lua_State * _lua);
int luaApi_FollowBody(lua_State * _lua);
int luaApi_GetObjectById(lua_State * _lua);
int luaApi_GetObjectByName(lua_State * _lua);
int luaApi_CreateBox2dObstaclesFromObjects(lua_State * _lua); // TODO: direct call Box2D API
int luaApi_CreateBox2dSensorsFromObjects(lua_State * _lua); // TODO: direct call Box2D API
int luaApi_onBodyContacted(lua_State * _lua);
int luaApi_onBodyDiscontacted(lua_State * _lua);

class LuaContactObserver : public ContactObserver
{
    DISABLE_COPY_AND_MOVE(LuaContactObserver)

public:
    LuaContactObserver(const Workspace & _workspace, lua_State * _lua, int _registry_key);
    void onContacted(Body & _body, TileMapObject & _object) override;
    void onContacted(Body & _body1, Body & _body2) override;
    void onDiscontacted(Body & _body, TileMapObject & _object) override;
    void onDiscontacted(Body & _body1, Body & _body2) override;

private:
    bool pushCallback(const char * _key);
    void callCallback();

private:
    const Workspace & mr_workspace;
    lua_State * mp_lua;
    int m_registry_key;
};

void registerCallback(lua_State * _lua, const char * _key);

struct Self
{
    const Workspace * workspace;
    Level * level;
    std::unique_ptr<LuaContactObserver> contact_observer;
    int registry_key;
};

void luaPushLevelApiMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_mtable_level) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "destroy", luaApi_Destroy },
        { "move", luaApi_Move },
        { "createBody", luaApi_CreateBody },
        { "followBody", luaApi_FollowBody },
        { "getObjectById", luaApi_GetObjectById },
        { "getObjectByName", luaApi_GetObjectByName },
        { "createBox2dObstaclesFromObjects", luaApi_CreateBox2dObstaclesFromObjects },
        { "createBox2dSensorsFromObjects", luaApi_CreateBox2dSensorsFromObjects },
        { "onBodyContacted", luaApi_onBodyContacted },
        { "onBodyDiscontacted", luaApi_onBodyDiscontacted },
        { nullptr, nullptr }
    };

    luaL_newmetatable(_lua, gc_mtable_level);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
    luaL_setfuncs(_lua, funcs, 0);
}

// 1 self
int luaApi_Destroy(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_level));
    self->level->getBox2D().removeContactObserver(*self->contact_observer);
    luaL_unref(_lua, LUA_REGISTRYINDEX,  self->registry_key);
    delete self;
    return 0;
}

// 1 self
// 2 delta X
// 3 delta Y
int luaApi_Move(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_level));
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "delta X expected");
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, "delta Y expected");
    int dx = static_cast<int>(lua_tointeger(_lua, 2));
    int dy = static_cast<int>(lua_tointeger(_lua, 3));
    self->level->move(dx, dy);
    return 0;
}

// 1 self
// 2 prototype
int luaApi_CreateBody(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_level));
    LuaBodyPrototype * lproto;
    luaL_argcheck(_lua, luaTryGetBodyPrototype(_lua, 2, &lproto), 2, "body prototype expected");
    Body & body = self->level->createBody(lproto->proto);
    luaPushBodyApiOntoStack(_lua, body);

    if(lproto->script_path.has_value())
    {
        lua_pushvalue(_lua, -1);
        std::unique_ptr<LuaCallObject> call_object_body = luaUseCallObject(_lua, "body");
        lua_pushvalue(_lua, 1);
        std::unique_ptr<LuaCallObject> call_object_level = luaUseCallObject(_lua, "level");
        luaL_loadfile(_lua, lproto->script_path.value().c_str());
        if(lua_pcall(_lua, 0, LUA_MULTRET, 0) != LUA_OK)
            self->workspace->getMainLogger().error(lua_tostring(_lua, -1));
        call_object_body.release();
        call_object_level.release();
    }

    return 1;
}

// 1 self
// 2 body or nil
int luaApi_FollowBody(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_level));
    Body * body = nullptr;
    if(!lua_isnil(_lua, 2) && !luaTryGetBody(_lua, 2, &body))
        luaL_argerror(_lua, 2, "a body or nil expected");
    self->level->setFollowedBody(body);
    return 0;
}

// 1 self
// 2 id
int luaApi_GetObjectById(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_level));
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "object ID expected");
    int id = static_cast<int>(lua_tointeger(_lua, 2));
    const TileMapObject * object = self->level->getObjectById(id);
    if(object == nullptr)
        lua_pushnil(_lua);
    else
        luaPushLevelObjectOntoStack(_lua, *object);
    return 1;
}

// 1 self
// 2 name
int luaApi_GetObjectByName(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_level));
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, "an object name expected");
    std::string name(lua_tostring(_lua, 2));
    const TileMapObject * object = self->level->getObjectByName(name);
    if(object == nullptr)
        lua_pushnil(_lua);
    else
        luaPushLevelObjectOntoStack(_lua, *object);
    return 1;
}

// 1 self
// 2 class
int luaApi_CreateBox2dObstaclesFromObjects(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_level));
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, "a class name expected");
    std::string cls(lua_tostring(_lua, 2));
    self->level->createBox2dStaticBodies(cls, Box2D::StaticObjectKind::obstacle);
    return 0;
}

// 1 self
// 2 class
int luaApi_CreateBox2dSensorsFromObjects(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_level));
    luaL_argcheck(_lua, lua_isstring(_lua, 2), 2, "a class name expected");
    std::string cls(lua_tostring(_lua, 2));
    self->level->createBox2dStaticBodies(cls, Box2D::StaticObjectKind::sensor);
    return 0;
}

// 1 self
// 2 callback or null
int luaApi_onBodyContacted(lua_State * _lua)
{
    registerCallback(_lua, gc_key_func_on_body_contacted);
    return 0;
}

// 1 self
// 2 callback or null
void registerCallback(lua_State * _lua, const char * _key)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_mtable_level));
    luaL_argcheck(_lua, 2, lua_isfunction(_lua, 2) || lua_isnil(_lua, 2), "callback or nil expected");
    lua_rawgeti(_lua, LUA_REGISTRYINDEX, self->registry_key);
    lua_pushstring(_lua, _key);
    lua_pushvalue(_lua, 2);
    lua_settable(_lua, -3);
}

// 1 self
// 2 callback or null
int luaApi_onBodyDiscontacted(lua_State * _lua)
{
    registerCallback(_lua, gc_key_func_on_body_discontacted);
    return 0;
}

} // namespace

LuaContactObserver::LuaContactObserver(const Workspace & _workspace, lua_State * _lua, int _registry_key) :
    mr_workspace(_workspace),
    mp_lua(_lua),
    m_registry_key(_registry_key)
{
}

void LuaContactObserver::onContacted(Body & _body, TileMapObject & _object)
{
    if(pushCallback(gc_key_func_on_body_contacted))
    {
        luaPushBodyApiOntoStack(mp_lua, _body);
        luaPushLevelObjectOntoStack(mp_lua, _object);
        callCallback();
    }
}

void LuaContactObserver::onContacted(Body & _body1, Body & _body2)
{
    NOT_USED(_body1);
    NOT_USED(_body2);
    if(pushCallback(gc_key_func_on_body_contacted))
    {
        luaPushBodyApiOntoStack(mp_lua, _body1);
        luaPushBodyApiOntoStack(mp_lua, _body2);
        callCallback();
    }
}

void LuaContactObserver::onDiscontacted(Body & _body, TileMapObject & _object)
{
    NOT_USED(_body);
    NOT_USED(_object);
    if(pushCallback(gc_key_func_on_body_discontacted))
    {
        luaPushBodyApiOntoStack(mp_lua, _body);
        luaPushLevelObjectOntoStack(mp_lua, _object);
        callCallback();
    }

}

void LuaContactObserver::onDiscontacted(Body & _body1, Body & _body2)
{
    NOT_USED(_body1);
    NOT_USED(_body2);
    if(pushCallback(gc_key_func_on_body_discontacted))
    {
        luaPushBodyApiOntoStack(mp_lua, _body1);
        luaPushBodyApiOntoStack(mp_lua, _body2);
        callCallback();
    }
}

bool LuaContactObserver::pushCallback(const char * _key)
{
    lua_rawgeti(mp_lua, LUA_REGISTRYINDEX, m_registry_key);
    lua_pushstring(mp_lua, _key);
    if(lua_gettable(mp_lua, -2) == LUA_TFUNCTION)
    {
        lua_remove(mp_lua, -2);
        return true;
    }
    lua_pop(mp_lua, 1);
    return false;
}

void LuaContactObserver::callCallback()
{
    if(lua_pcall(mp_lua, 2, 0, 0) != LUA_OK)
    {
        mr_workspace.getMainLogger().error(lua_tostring(mp_lua, -1));
        lua_pop(mp_lua, 1);
    }
}

void Sol2D::Lua::luaCreateLevelApiAndPushOntoStack(lua_State * _lua, const Workspace & _workspace, Level & _level)
{
    void * self = lua_newuserdata(_lua, sizeof(Self));
    Self * lua_level = new(self) Self();
    lua_newtable(_lua);
    int registry_key = luaL_ref(_lua, LUA_REGISTRYINDEX);
    luaPushLevelApiMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
    lua_level->workspace = &_workspace;
    lua_level->level = &_level;
    lua_level->contact_observer.reset(new LuaContactObserver(_workspace, _lua, registry_key));
    lua_level->registry_key = registry_key;
    _level.getBox2D().addContactObserver(*lua_level->contact_observer);
}
