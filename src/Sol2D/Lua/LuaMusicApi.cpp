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

#include <Sol2D/Lua/LuaMusicApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;
using namespace Sol2D::SDL;

namespace {

struct Self : LuaUserData<Self, LuaTypeName::music>
{
    MusicPtr music;
};

// 1 self
int luaApi_GC(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    self->music.reset();
    return 0;
}

// 1 self
int luaApi_Play(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    bool result = self->music && Mix_PlayMusic(self->music.get(), 0) == 0;
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 iteration count
int luaApi_Loop(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "iteration count required");
    bool result = self->music && Mix_PlayMusic(self->music.get(), lua_tointeger(_lua, 2)) == 0;
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushMusicApi(lua_State * _lua, MusicPtr _music)
{
    Self * self = Self::pushUserData(_lua);
    self->music = _music;
    if(Self::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
            {
                { "__gc", luaApi_GC },
                { "play", luaApi_Play },
                { "loop", luaApi_Loop },
                { nullptr, nullptr }
            };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

