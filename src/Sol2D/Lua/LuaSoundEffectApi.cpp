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

#include <Sol2D/Lua/LuaSoundEffectApi.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::SDL;
using namespace Lua::Aux;

namespace {

const char gc_metatable_sound_effect[] = "sol.SoundEffect";

struct Self : LuaUserData<Self, gc_metatable_sound_effect>
{
    SoundChunkPtr chunk;
};

// 1 self
int luaApi_GC(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    self->chunk.reset();
    return 0;
}

// 1 self
// 2 channel (optional)
int luaApi_Play(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    bool result = false;
    if(self->chunk)
    {
        int channel = lua_isinteger(_lua, 2) ? lua_tointeger(_lua, 2) : -1;
        result = Mix_PlayChannel(channel, self->chunk.get(), 0) >= 0;
    }
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 iteration count
// 3 channel (optional)
int luaApi_Loop(lua_State * _lua)
{
    Self * self = Self::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "iteration count required");
    int iteration_count = lua_tointeger(_lua, 2);
    bool result = false;
    if(self->chunk)
    {
        int channel = lua_isinteger(_lua, 3) ? lua_tointeger(_lua, 3) : -1;
        result = Mix_PlayChannel(channel, self->chunk.get(), iteration_count) >= 0;
    }
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushSoundEffectApi(lua_State * _lua, SoundChunkPtr _chunk)
{
    Self * self = Self::pushUserData(_lua);
    self->chunk = _chunk;
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
