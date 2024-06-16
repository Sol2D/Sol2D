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

#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/LuaFontApi.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;
using namespace Sol2D::SDL;

namespace {

const char gc_metatable_font[] = "sol.Font";

struct Self : LuaUserData<Self, gc_metatable_font>
{
    FontPtr font;
};

// 1 self
int luaApi_GC(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    self->font.reset();
    return 0;
}

} // namespace name

void Sol2D::Lua::pushFontApi(lua_State * _lua, SDL::FontPtr _font)
{
    Self * self = Self::pushUserData(_lua);
    self->font = _font;
    if(Self::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", luaApi_GC },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

FontPtr Sol2D::Lua::tryGetFont(lua_State * _lua, int _idx)
{
    Self * self = Self::getUserData(_lua, _idx);
    return self ? self->font : nullptr;
}
