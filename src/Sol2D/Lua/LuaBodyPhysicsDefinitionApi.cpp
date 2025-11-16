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

#include <Sol2D/Lua/LuaBodyPhysicsDefinitionApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D::World;

bool Sol2D::Lua::tryGetBodyPhysicsDefinition(lua_State * _lua, int _idx, BodyPhysicsDefinition & _physics)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return false;
    {
        lua_Integer lua_int;
        if(table.tryGetInteger("type", &lua_int))
        {
            std::optional<BodyType> body_type = castToBodyType(lua_int);
            if(body_type.has_value())
                _physics.type = body_type.value();
        }
    }
    table.tryGetPoint("position", _physics.position);
    table.tryGetPoint("linearVelocity", _physics.linear_velocity);
    table.tryGetNumber("linearDamping", _physics.linear_damping);
    table.tryGetNumber("angularDamping", _physics.angular_damping);
    table.tryGetNumber("angularVelocity", _physics.angular_velocity);
    table.tryGetNumber("gravityScale", _physics.gravity_scale);
    table.tryGetNumber("sleepThreshold", _physics.sleep_threshold);
    table.tryGetRotation("rotation", _physics.rotation);
    table.tryGetBoolean("isRotationAllowed", &_physics.is_rotation_allowed);
    table.tryGetBoolean("isMotionXAllowed", &_physics.is_motion_x_allowed);
    table.tryGetBoolean("isMotionYAllowed", &_physics.is_motion_y_allowed);
    table.tryGetBoolean("isSleepEnabled", &_physics.is_sleep_enabled);
    table.tryGetBoolean("isAwake", &_physics.is_awake);
    table.tryGetBoolean("isBullet", &_physics.is_bullet);
    table.tryGetBoolean("isEnabled", &_physics.is_enabled);
    table.tryGetBoolean("isFastRotationAllowed", &_physics.is_fast_rotation_allowed);
    return true;
}
