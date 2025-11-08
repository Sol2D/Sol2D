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

#include <Sol2D/Lua/LuaAudioTrackApi.h>
#include <Sol2D/Lua/LuaAudioApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Lua;

namespace {

struct Self : LuaSelfBase
{
    explicit Self(std::shared_ptr<MIX_Track> & _track) :
        track(_track),
        track_props(SDL_CreateProperties())
    {
    }

    void beforeDelete(lua_State *) override
    {
        SDL_DestroyProperties(track_props);
    }

    std::shared_ptr<MIX_Track> getTrack(lua_State * _lua) const
    {
        std::shared_ptr<MIX_Track> ptr = track.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::sound_effect_is_destroyed);
        return ptr;
    }

    std::weak_ptr<MIX_Track> track;
    SDL_PropertiesID track_props;
};

using UserData = LuaUserData<Self, LuaTypeName::audio_track>;

// 1 self
int luaApi_Play(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    bool result = MIX_PlayTrack(self->getTrack(_lua).get(), self->track_props);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 audio
int luaApi_SetAudio(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::shared_ptr<MIX_Audio> audio = tryGetAudio(_lua, 2);
    bool result = MIX_SetTrackAudio(self->getTrack(_lua).get(), audio ? audio.get() : nullptr);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 iteration count
int luaApi_SetLoop(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    int iteration_count = lua_tointeger(_lua, 2);
    bool result = SDL_SetNumberProperty(self->track_props, MIX_PROP_PLAY_LOOPS_NUMBER, iteration_count);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushAudioTrackApi(lua_State * _lua, std::shared_ptr<MIX_Track> _track)
{
    UserData::pushUserData(_lua, _track);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "__gc",  UserData::luaGC },
            { "setAudio", luaApi_SetAudio },
            { "setLoop", luaApi_SetLoop },
            { "play",  luaApi_Play },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
