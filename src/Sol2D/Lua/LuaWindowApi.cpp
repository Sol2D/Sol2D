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
#include <Sol2D/Lua/LuaWindowApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    explicit Self(Window & _window) :
        window(_window)
    {
    }

    Window & window;
};

using UserData = LuaUserData<Self, LuaTypeName::window>;

// 1 self
// 2 view (optional)
int luaApi_SetView(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_gettop(_lua) < 2 || lua_isnil(_lua, 2))
    {
        self->window.setView(nullptr);
    }
    else
    {
        std::shared_ptr<View> view = tryGetView(_lua, 2);
        luaL_argexpected(_lua, view, 2, LuaTypeName::view);
        self->window.setView(view);
    }
    return 0;
}

// 1 self
int luaApi_GetView(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(std::shared_ptr<View> view = self->window.getView())
        pushViewApi(_lua, view);
    else
        lua_pushnil(_lua);
    return 1;
}

} // namespace

void Sol2D::Lua::pushWindowApi(lua_State * _lua, Window & _window)
{
    UserData::pushUserData(_lua, _window);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "setView", luaApi_SetView },
            { "getView", luaApi_GetView },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
