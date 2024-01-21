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

#include <Sol2D/Lua/LuaLibrary.h>
#include <Sol2D/Lua/LuaKeyboardApi.h>
#include <Sol2D/Lua/LuaColorApi.h>
#include <Sol2D/Lua/LuaWorldApi.h>
#include <Sol2D/Lua/LuaHeartbeatApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaRectApi.h>
#include <functional>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

const char * gc_lib_mtable = "sol.Library";

void luaAddSublibrary(lua_State * _lua, const char * _sublib_name, std::function<void()> _push_sublib_onto_stack)
{
    int self = lua_gettop(_lua);
    lua_pushstring(_lua, _sublib_name);
    _push_sublib_onto_stack();
    lua_settable(_lua, self);
}

// If _set is true, object at the top of the stack will be set as object and popped from the stack,
// otherwise nil will be set.
void luaSetCallObject(lua_State * _lua, const std::string & _key, bool _set)
{
    luaL_getmetatable(_lua, gc_lib_mtable);
    lua_pushstring(_lua, _key.c_str());
    if(_set)
        lua_pushvalue(_lua, -3);
    else
        lua_pushnil(_lua);
    lua_settable(_lua, -3);
    lua_pop(_lua, _set ? 2 : 1);
}

} // namespace


LuaCallObject::LuaCallObject(lua_State * _lua, const std::string & _key) :
    mp_lua(_lua),
    m_key(_key)
{
    luaSetCallObject(_lua, m_key, true);
}

LuaCallObject::~LuaCallObject()
{
    luaSetCallObject(mp_lua, m_key, false);
}

void Sol2D::Lua::luaRegisterLibrary(lua_State * _lua, const Workspace & _workspace, World & _world)
{
    lua_newuserdata(_lua, 1);

    luaL_newmetatable(_lua, gc_lib_mtable);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);

    luaAddSublibrary(_lua, "world",
                     std::bind(luaPushWorldApiOntoStack, _lua, std::cref(_workspace), std::ref(_world)));
    luaAddSublibrary(_lua, "keyboard", std::bind(luaPushKeyboardApiOntoStack, _lua));
    luaAddSublibrary(_lua, "color", std::bind(luaPushColorApiOntoStack, _lua));
    luaAddSublibrary(_lua, "heartbeat", std::bind(luaPushHeartbeatApiOntoStack, _lua));
    luaAddSublibrary(_lua, "point", std::bind(luaPushPointApiOntoStack, _lua));
    luaAddSublibrary(_lua, "rect", std::bind(luaPushRectApiOntoStack, _lua));
    lua_setmetatable(_lua, -2);
    lua_setglobal(_lua, "sol");
}

std::unique_ptr<LuaCallObject> Sol2D::Lua::luaUseCallObject(lua_State * _lua, const std::string & _key)
{
    return std::make_unique<LuaCallObject>(_lua, _key);
}
