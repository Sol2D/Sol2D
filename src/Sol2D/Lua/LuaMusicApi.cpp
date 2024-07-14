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

namespace {

struct Self : LuaSelfBase
{
    Self(std::shared_ptr<Mix_Music> & _music) :
        music(_music)
    {
    }

    std::shared_ptr<Mix_Music> getMusic(lua_State * _lua) const
    {
        std::shared_ptr<Mix_Music> ptr = music.lock();
        if(!ptr)
            luaL_error(_lua, "the music is destroyed");
        return ptr;
    }

    std::weak_ptr<Mix_Music> music;
};

using UserData = LuaUserData<Self, LuaTypeName::music>;

// 1 self
int luaApi_Play(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    bool result = Mix_PlayMusic(self->getMusic(_lua).get(), 0) == 0;
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 iteration count
int luaApi_Loop(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "iteration count required");
    bool result = Mix_PlayMusic(self->getMusic(_lua).get(), lua_tointeger(_lua, 2)) == 0;
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushMusicApi(lua_State * _lua, std::shared_ptr<Mix_Music> _music)
{
    UserData::pushUserData(_lua, _music);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "play", luaApi_Play },
            { "loop", luaApi_Loop },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

