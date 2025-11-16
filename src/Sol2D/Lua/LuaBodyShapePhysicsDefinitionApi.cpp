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

#include <Sol2D/Lua/LuaBodyShapePhysicsDefinitionApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D::World;

bool Sol2D::Lua::tryGetBodyShapePhysicsDefinition(lua_State * _lua, int _idx, BodyShapePhysicsDefinition & _definition)
{
    LuaTableApi table(_lua, _idx);
    if(table.isValid())
    {
        if(table.tryGetTable("material"))
        {
            LuaTableApi material_table(_lua, -1);
            b2SurfaceMaterial material = b2DefaultSurfaceMaterial();
            material_table.tryGetNumber("friction", &material.friction);
            material_table.tryGetNumber("restitution", &material.restitution);
            material_table.tryGetNumber("rollingResistance", &material.rollingResistance);
            material_table.tryGetNumber("tangentSpeed", &material.tangentSpeed);
            _definition.material = material;
            lua_pop(_lua, 1);
        }
        table.tryGetNumber("density", _definition.density);
        table.tryGetBoolean("isSensor", &_definition.is_sensor);
        table.tryGetBoolean("arePreSolveEventsEnabled", &_definition.are_pre_solve_events_enabled);
        table.tryGetBoolean("areSensorEventsEnabled", &_definition.are_sensor_events_enabled);
        table.tryGetBoolean("areContactEventsEnabled", &_definition.are_contact_events_enabled);
        table.tryGetBoolean("areHitEventsEnabled", &_definition.are_hit_events_enabled);
        return true;
    }
    return false;
}
