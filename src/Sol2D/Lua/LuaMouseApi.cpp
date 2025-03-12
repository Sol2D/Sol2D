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

#include <Sol2D/Lua/LuaMouseApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <SDL3/SDL_mouse.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
};

using UserData = LuaUserData<Self, LuaTypeName::mouse>;

// 1 self
int luaApi_GetState(lua_State * _lua)
{
    UserData::validateUserData(_lua, 1);
    SDL_FPoint point;
    SDL_MouseButtonFlags flags = SDL_GetMouseState(&point.x, &point.y);
    LuaTable table = LuaTable::pushNew(_lua);
    table.setBooleanValue("left", SDL_BUTTON_LMASK & flags);
    table.setBooleanValue("right", SDL_BUTTON_RMASK & flags);
    table.setBooleanValue("middle", SDL_BUTTON_MMASK & flags);
    table.setBooleanValue("x1", SDL_BUTTON_X1MASK & flags);
    table.setBooleanValue("x2", SDL_BUTTON_X2MASK & flags);
    table.setPointValue("point", point);
    return 1;
}

} // namespace

void Sol2D::Lua::pushMouseApi(lua_State * _lua)
{
    UserData::pushUserData(_lua);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            {"__gc",     UserData::luaGC},
            {"getState", luaApi_GetState},
            {nullptr,    nullptr        }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
