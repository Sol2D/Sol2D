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

#include <Sol2D/Lua/LuaAudioApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    Self(MIX_Mixer & _mixer, std::shared_ptr<MIX_Audio> & _audio) :
        mixer(&_mixer),
        audio(_audio)
    {
    }

    std::shared_ptr<MIX_Audio> getAudio(lua_State * _lua) const
    {
        std::shared_ptr<MIX_Audio> ptr = audio.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::music_is_destroyed);
        return ptr;
    }

    MIX_Mixer * mixer;
    std::weak_ptr<MIX_Audio> audio;
};

using UserData = LuaUserData<Self, LuaTypeName::audio>;

// 1 self
int luaApi_Play(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    bool result = MIX_PlayAudio(self->mixer, self->getAudio(_lua).get());
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushAudioApi(lua_State * _lua, MIX_Mixer & _mixer, std::shared_ptr<MIX_Audio> _audio)
{
    UserData::pushUserData(_lua, std::ref(_mixer), _audio);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "play", luaApi_Play },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<MIX_Audio> Sol2D::Lua::tryGetAudio(lua_State * _lua, int _idx)
{
    Self * self = UserData::getUserData(_lua, _idx);
    return self ? self->getAudio(_lua) : nullptr;
}
