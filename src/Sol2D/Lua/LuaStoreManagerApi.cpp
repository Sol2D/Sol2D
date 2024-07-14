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

#include <Sol2D/Lua/LuaStoreManagerApi.h>
#include <Sol2D/Lua/LuaStoreApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_message_store_key_expected[] = "a store key expected";

struct Self : LuaSelfBase
{
    explicit Self(StoreManager & _manager, const Workspace & _workspace, SDL_Renderer & _renderer) :
        manager(_manager),
        workspace(_workspace),
        renderer(_renderer)
    {
    }

    StoreManager & manager;
    const Workspace & workspace;
    SDL_Renderer & renderer;
};

using UserData = LuaUserData<Self, LuaTypeName::store_manager>;

// 1 self
// 2 key
int luaApi_CreateStore(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_store_key_expected);
    std::shared_ptr<Store> store = self->manager.createStore(key);
    pushStoreApi(_lua, self->workspace, self->renderer, store);
    return 1;
}

// 1 self
// 2 key
int luaApi_GetStore(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_store_key_expected);
    std::shared_ptr<Store> store = self->manager.getStore(key);
    pushStoreApi(_lua, self->workspace, self->renderer, store);
    return 1;
}

// 1 self
// 2 key
int luaApi_DeleteStore(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_store_key_expected);
    lua_pushboolean(_lua, self->manager.deleteStore(key));
    return 1;
}

} // namespace

void Sol2D::Lua::pushStoreManagerApi(
    lua_State * _lua,
    const Workspace & _workspace,
    SDL_Renderer & _renderer,
    StoreManager & _store_manager)
{
    UserData::pushUserData(_lua, _store_manager, _workspace, _renderer);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "createStore", luaApi_CreateStore },
            { "getStore", luaApi_GetStore },
            { "deleteStore", luaApi_DeleteStore },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
