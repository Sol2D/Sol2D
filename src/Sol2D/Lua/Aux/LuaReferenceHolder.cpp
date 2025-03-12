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

#include <Sol2D/Lua/Aux/LuaReferenceHolder.h>
#include <Sol2D/Lua/Aux/LuaUtils.h>
#include <lua.hpp>
#include <format>
#include <vector>
#include <cstring>

using namespace Sol2D::Lua;

namespace {

const char gc_key[] { '\0' };

std::string makeKeyPrefix(const void * _owner)
{
     return std::format("{:x}:", reinterpret_cast<size_t>(_owner));
}

std::string makeKey(const void * _owner, const char * _object_type, const char * _key)
{
    return std::format("{}:{}:{}", makeKeyPrefix(_owner), _object_type, _key);
}

} // namespace

// Adds a record like this:
// table["<key>:<owner_address>"] = reference
void LuaReferenceHolder::hold(const void * _owner, const char * _object_type, const char * _key) const
{
    getRegistry();
    lua_pushvalue(mp_lua, -2);
    lua_setfield(mp_lua, -2, makeKey(_owner, _object_type, _key).c_str());
    lua_pop(mp_lua, 1);
}

void LuaReferenceHolder::getRegistry() const
{
    if(lua_rawgetp(mp_lua, LUA_REGISTRYINDEX, gc_key) != LUA_TTABLE)
    {
        lua_pop(mp_lua, 1);
        lua_newtable(mp_lua);
        lua_pushvalue(mp_lua, -1);
        lua_rawsetp(mp_lua, LUA_REGISTRYINDEX, gc_key);
    }
}

// Deletes a record by setting its value to nil:
// table["<key>:<owner_address>"] = nil
void LuaReferenceHolder::release(const void * _owner, const char * _object_type, const char * _key) const
{
    getRegistry();
    lua_pushnil(mp_lua);
    lua_setfield(mp_lua, -2, makeKey(_owner, _object_type, _key).c_str());
    lua_pop(mp_lua, 1);
}

void LuaReferenceHolder::releaseAll(const void * _owner) const
{
    const std::string key_prefix = makeKeyPrefix(_owner);
    std::vector<std::string> keys_to_delete;
    getRegistry();
    const int registry_idx = lua_gettop(mp_lua);
    lua_pushnil(mp_lua);
    for(bool exit = false; !exit && lua_next(mp_lua, registry_idx);)
    {
        const char * key = argToString(mp_lua, -2);
        if(key && std::strncmp(key_prefix.c_str(), key, key_prefix.size()) == 0)
            keys_to_delete.push_back(key);
        lua_pop(mp_lua, 1); // value
    }
    lua_pop(mp_lua, 1); // index

    for(const std::string & key : keys_to_delete)
    {
        lua_pushnil(mp_lua);
        lua_setfield(mp_lua, -2, key.c_str());
    }

    lua_pop(mp_lua, 1); // registry
}
