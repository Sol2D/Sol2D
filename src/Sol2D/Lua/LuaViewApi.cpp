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

#include <Sol2D/Lua/LuaViewApi.h>
#include <Sol2D/Lua/LuaLayoutingApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
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
int luaApi_GetLayout(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    pushLayoutNodeApi(_lua, self->getView(_lua)->getLayout());
    return 1;
}

} // namespace

void Sol2D::Lua::pushViewApi(lua_State * _lua, std::shared_ptr<View> _view)
{
    UserData::pushUserData(_lua, _view);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "getLayout", luaApi_GetLayout },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<View> Sol2D::Lua::tryGetView(lua_State * _lua, int _idx)
{
    if(const Self * self = UserData::tryGetUserData(_lua, _idx))
        return self->getView(_lua);
    return nullptr;
}
