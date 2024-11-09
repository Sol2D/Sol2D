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

#include <Sol2D/Lua/LuaKeyboardApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <SDL3/SDL_keyboard.h>

using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    int kb_state_length;
    const bool * kb_state;
};

using UserData = LuaUserData<Self, LuaTypeName::keyboard>;

// 1 self
// 2 scancode
// ...
// n scancode
int luaApi_GetState(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    int top = lua_gettop(_lua);
    for(int idx = 2; idx <= top; ++idx)
    {
        luaL_argexpected(_lua, lua_isinteger(_lua, idx), idx, LuaTypeName::integer);
        lua_Integer scancode = lua_tointeger(_lua, idx);
        lua_pushboolean(_lua, scancode >= 0 && scancode < self->kb_state_length && self->kb_state[scancode]);
    }
    return top - 1;
}

} // namespace name

void Sol2D::Lua::pushKeyboardApiOntoStack(lua_State * _lua)
{
    Self * self = UserData::pushUserData(_lua);
    self->kb_state = SDL_GetKeyboardState(&self->kb_state_length);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "getState", luaApi_GetState },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
