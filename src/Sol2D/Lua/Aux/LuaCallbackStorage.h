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

    static uint32_t generateUniqueSetId()
    {
        return s_next_set_id++;
    }

    uint32_t addCallback(uint32_t _unique_set_id, int _callback_idx);
    void removeCallback(uint32_t _unique_set_id, uint32_t _callback_id);
    void callSet(const Workspace & _workspace, uint32_t _unique_set_id, uint16_t _args_count);
    void destroyCallbackSet(int _set_unique_id);

private:
    bool tryGetCallbackTableFromIndexTable(uint32_t _unique_set_id);
    void getIndexTable();

private:
    static const char sc_registry_key;
    static uint32_t s_next_callback_id;
    static uint32_t s_next_set_id;
    lua_State * mp_lua;
};

} // namespace Sol2D::Lua::Aux
