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

#include <lua.hpp>
#include <Sol2D/Def.h>

namespace Sol2D::Lua::Aux {

class LuaWeakRegistryStorage final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(LuaWeakRegistryStorage)

    explicit LuaWeakRegistryStorage(lua_State * _lua) :
        mp_lua(_lua)
    {
    }

    void save(void * _key, int _idx);
    bool tryGet(void * _key, int _expected_type);

private:
    lua_State * mp_lua;
    static const char sc_storage_key;
};

} // namespace Sol2D::Lua::Aux
