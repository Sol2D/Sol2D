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

#include <Sol2D/Lua/LuaAux.h>

bool Sol2D::Lua::luaTryGetInteger(lua_State * _lua, int _table_idx, const char * _key, int * _value)
{
    lua_pushstring(_lua, _key);
    lua_gettable(_lua, _table_idx);
    bool result = lua_isinteger(_lua, -1);
    if(result)
        *_value = lua_tointeger(_lua, -1);
    lua_pop(_lua, 1);
    return result;
}

bool Sol2D::Lua::luaTryGetFloat(lua_State * _lua, int _table_idx, const char * _key, float * _value)
{
    lua_pushstring(_lua, _key);
    lua_gettable(_lua, _table_idx);
    bool result = lua_isinteger(_lua, -1);
    if(result)
        *_value = static_cast<float>(lua_tonumber(_lua, -1));
    lua_pop(_lua, 1);
    return result;
}
