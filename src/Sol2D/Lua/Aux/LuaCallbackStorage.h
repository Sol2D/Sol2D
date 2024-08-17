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

#pragma once

#include <Sol2D/Workspace.h>
#include <lua.hpp>
#include <cstdint>

namespace Sol2D::Lua::Aux {

class LuaCallbackStorage final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(LuaCallbackStorage)

    explicit LuaCallbackStorage(lua_State * _lua) :
        mp_lua(_lua)
    {
    }

    uint32_t addCallback(const void * _owner, uint16_t _event_id, int _callback_idx);
    size_t removeCallback(const void * _owner, uint16_t _event_id, uint32_t _subscription_id);
    void execute(const Workspace & _workspace, const void * _owner, uint16_t _event_id, uint16_t _args_count);
    void destroyCallbacks(const void * _owner);

private:
    static int luaGC(lua_State *);
    void getCallbackRegisty();
    void createCallbackRegisty();
    bool tryGetEventsTable(const void * _owner, uint16_t _event_id);
    void ensureEventsTable(const void * _owner, uint16_t _event_id);

private:
    static const char sc_callback_registry_key;
    static bool s_is_disposed;
    static uint32_t s_next_subscription_id;
    lua_State * mp_lua;
};

} // namespace Sol2D::Lua::Aux
