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

#include <Sol2D/Lua/LuaBodyShapePhysicsDefinitionApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D::World;
using namespace Sol2D::Lua::Aux;

bool Sol2D::Lua::tryGetBodyShapePhysicsDefinition(lua_State * _lua, int _idx, BodyShapePhysicsDefinition & _definition)
{
    LuaTable table(_lua, _idx);
    if(table.isValid())
    {
        table.tryGetNumber("density", _definition.density);
        table.tryGetNumber("restitution", _definition.restitution);
        table.tryGetNumber("friction",  _definition.friction);
        table.tryGetBoolean("isSensor", &_definition.is_sensor);
        table.tryGetBoolean("isPreSolveEnabled", &_definition.is_pre_solve_enabled);
        return true;
    }
    return false;
}
