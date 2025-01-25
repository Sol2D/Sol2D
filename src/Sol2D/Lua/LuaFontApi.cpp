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

#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/LuaFontApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    explicit Self(std::shared_ptr<TTF_Font> & _font) :
        font(_font)
    {
    }

    std::weak_ptr<TTF_Font> font;
};

using UserData = LuaUserData<Self, LuaTypeName::font>;

} // namespace name

void Sol2D::Lua::pushFontApi(lua_State * _lua, std::shared_ptr<TTF_Font> _font)
{
    UserData::pushUserData(_lua, _font);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<TTF_Font> Sol2D::Lua::tryGetFont(lua_State * _lua, int _idx)
{
    Self * self = UserData::tryGetUserData(_lua, _idx);
    return self ? self->font.lock() : nullptr;
}
