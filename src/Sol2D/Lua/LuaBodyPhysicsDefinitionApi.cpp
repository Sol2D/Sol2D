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

#include <Sol2D/Lua/LuaBodyPhysicsDefinitionApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua::Aux;

bool Sol2D::Lua::tryGetBodyPhysicsDefinition(lua_State * _lua, int _idx, BodyPhysicsDefinition & _physics)
{
    if(!lua_istable(_lua, _idx))
        return false;
    LuaTable table(_lua, _idx);
    {
        lua_Number value;
        if(table.tryGetNumber("linearDamping", &value))
            _physics.linear_damping = static_cast<float>(value);
        if(table.tryGetNumber("angularDamping", &value))
            _physics.angular_damping = static_cast<float>(value);
    }
    table.tryGetBoolean("fixedRotation", &_physics.fixed_rotation);
    return true;
}
