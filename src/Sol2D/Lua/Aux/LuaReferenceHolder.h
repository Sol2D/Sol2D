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

#include <Sol2D/Lua/Aux/LuaForward.h>
#include <Sol2D/Def.h>

#pragma once

namespace Sol2D::Lua {

class LuaReferenceHolder
{
public:
    LuaReferenceHolder(lua_State * _lua) :
        m_lua(_lua)
    {
    }

    void hold(const void * _owner, const char * _object_type, const char * _key) const;
    void release(const void * _owner, const char * _object_type, const char * _key) const;
    void releaseAll(const void * _owner) const;

private:
    void getRegistry() const;

private:
    lua_State * m_lua;
};

} // namespace Sol2D::Lua
