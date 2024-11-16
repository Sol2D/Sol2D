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

#include <Sol2D/Lua/LuaViewApi.h>
#include <Sol2D/Lua/LuaFragmentApi.h>
#include <Sol2D/Lua/LuaFormApi.h>
#include <Sol2D/Lua/LuaSceneApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    explicit Self(std::shared_ptr<View> & _view) :
        view(_view)
    {
    }

    std::shared_ptr<View> getView(lua_State * _lua) const
    {
        std::shared_ptr<View> ptr = view.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::view_is_destroyed);
        return ptr;
    }

    std::weak_ptr<View> view;
};

using UserData = LuaUserData<Self, LuaTypeName::view>;

// 1 self
// 2 fragment
int luaApi_CreateFragment(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    Fragment fragment;
    luaL_argexpected(_lua, tryGetFragment(_lua, 2, fragment), 2, LuaTypeName::fragment);
    lua_pushinteger(_lua, self->getView(_lua)->createFragment(fragment));
    return 1;
}

// 1 self
// 2 fragment ID
// 3 fragment
int luaApi_UpdateFragment(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint16_t fragment_id = static_cast<uint16_t>(lua_tointeger(_lua, 2));
    Fragment fragment;
    luaL_argexpected(_lua, tryGetFragment(_lua, 3, fragment), 3, LuaTypeName::fragment);
    lua_pushboolean(_lua, self->getView(_lua)->updateFragment(fragment_id, fragment));
    return 1;
}

// 1 self
// 2 fragment ID
int luaApi_GetFragment(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint16_t fragment_id = static_cast<uint16_t>(lua_tointeger(_lua, 2));
    if(const Fragment * fragment = self->getView(_lua)->getFragment(fragment_id))
        pushFragment(_lua, *fragment);
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 fragment ID
int luaApi_DeleteFragment(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint16_t fragment_id = static_cast<uint16_t>(lua_tointeger(_lua, 2));
    lua_pushboolean(_lua, self->getView(_lua)->deleteFragment(fragment_id));
    return 1;
}

// 1 self
// 2 fragment ID
// 3 scene | form | nil (optioanl)
int luaApi_BindFragment(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    uint16_t fragment_id = static_cast<uint16_t>(lua_tointeger(_lua, 2));
    bool result = false;
    if(lua_gettop(_lua) < 3 || lua_isnil(_lua, 3))
    {
        result = self->getView(_lua)->bindFragment(fragment_id, nullptr);
    }
    else if(std::shared_ptr<Scene> scene = tryGetScene(_lua, 3))
    {
        result = self->getView(_lua)->bindFragment(fragment_id, std::static_pointer_cast<Canvas>(scene));
    }
    else if(std::shared_ptr<Forms::Form> form = tryGetForm(_lua, 3))
    {
        result = self->getView(_lua)->bindFragment(fragment_id, std::static_pointer_cast<Canvas>(form));
    }
    else
    {
        luaL_argexpected(
            _lua,
            false,
            3,
            LuaTypeName::joinTypes(LuaTypeName::scene, LuaTypeName::form, LuaTypeName::nil).c_str());
    }
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

std::shared_ptr<View> Sol2D::Lua::tryGetView(lua_State * _lua, int _idx)
{
    if(const Self * self = UserData::tryGetUserData(_lua, _idx))
        return self->getView(_lua);
    return nullptr;
}

void Sol2D::Lua::pushViewApi(lua_State * _lua, std::shared_ptr<View> _view)
{
    UserData::pushUserData(_lua, _view);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "createFragment", luaApi_CreateFragment },
            { "updateFragment", luaApi_UpdateFragment },
            { "getFragment", luaApi_GetFragment },
            { "deleteFragment", luaApi_DeleteFragment },
            { "bindFragment", luaApi_BindFragment },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
