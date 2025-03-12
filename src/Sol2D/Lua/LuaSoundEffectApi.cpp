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

#include <Sol2D/Lua/LuaSoundEffectApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Lua;

namespace {

struct Self : LuaSelfBase
{
    explicit Self(std::shared_ptr<Mix_Chunk> & _chunk) :
        chunk(_chunk)
    {
    }

    std::shared_ptr<Mix_Chunk> getChunk(lua_State * _lua) const
    {
        std::shared_ptr<Mix_Chunk> ptr = chunk.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::sound_effect_is_destroyed);
        return ptr;
    }

    std::weak_ptr<Mix_Chunk> chunk;
};

using UserData = LuaUserData<Self, LuaTypeName::sound_effect>;

// 1 self
// 2 channel (optional)
int luaApi_Play(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    int channel = lua_isinteger(_lua, 2) ? lua_tointeger(_lua, 2) : -1;
    bool result = Mix_PlayChannel(channel, self->getChunk(_lua).get(), 0) >= 0;
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 iteration count
// 3 channel (optional)
int luaApi_Loop(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    int iteration_count = lua_tointeger(_lua, 2);
    int channel = lua_isinteger(_lua, 3) ? lua_tointeger(_lua, 3) : -1;
    bool result = Mix_PlayChannel(channel, self->getChunk(_lua).get(), iteration_count) >= 0;
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushSoundEffectApi(lua_State * _lua, std::shared_ptr<Mix_Chunk> _chunk)
{
    UserData::pushUserData(_lua, _chunk);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            {"__gc",  UserData::luaGC},
            {"play",  luaApi_Play    },
            {"loop",  luaApi_Loop    },
            {nullptr, nullptr        }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
