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

#include <Sol2D/Lua/LuaJointDefinitionApi.h>
#include <Sol2D/Lua/LuaBodyApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

bool tryGetBodyIdFromJointDefinition(const LuaTableApi & _table, const char * _field, uint64_t * _id)
{
    {
        uint64_t id;
        if(_table.tryGetInteger(_field, &id))
        {
            *_id = id;
            return true;
        }
    }
    if(_table.tryGetValue(_field))
    {
        bool result = tryGetBodyId(_table.getLua(), -1, _id);
        lua_pop(_table.getLua(), 1);
        return result;
    }
    return false;
}

bool tryGetJointDefinition(const LuaTableApi & _table, World::JointDefinition & _result)
{
    uint64_t body_a, body_b;
    if(
        !_table.isValid() ||
        !tryGetBodyIdFromJointDefinition(_table, "bodyA", &body_a) ||
        !tryGetBodyIdFromJointDefinition(_table, "bodyB", &body_b))
    {
        return false;
    }
    _table.tryGetNumber("forceThreshold", _result.force_threshold);
    _table.tryGetNumber("torqueThreshold", _result.torque_threshold);
    _table.tryGetNumber("constraintHertz", _result.constraint_hertz);
    _table.tryGetNumber("constraintDampingRatio", _result.constraint_damping_ratio);
    _table.tryGetTranform("localFrameA", _result.local_frame_a);
    _table.tryGetTranform("localFrameB", _result.local_frame_b);
    _table.tryGetBoolean("isCollideConnectedEnabled", &_result.is_collide_connected_enabled);
    _result.body_a_id = body_a;
    _result.body_b_id = body_b;
    return true;
}

} // namespace

bool Sol2D::Lua::tryGetDistanceJointDefinition(lua_State * _lua, int _idx, World::DistanceJointDefinition & _result)
{
    LuaTableApi table(_lua, _idx);
    if(!tryGetJointDefinition(table, _result))
        return false;
    table.tryGetBoolean("isSpringEnabled", &_result.is_spring_enabled);
    table.tryGetBoolean("isMotorEnabled", &_result.is_motor_enabled);
    table.tryGetBoolean("isLimitEnabled", &_result.is_limit_enabled);
    table.tryGetNumber("minLength", _result.min_length);
    table.tryGetNumber("maxLength", _result.max_length);
    table.tryGetNumber("hertz", _result.hertz);
    table.tryGetNumber("dampingRatio", _result.damping_ratio);
    table.tryGetNumber("maxMotorForce", _result.max_motor_force);
    table.tryGetNumber("motorSpeed", _result.motor_speed);
    table.tryGetNumber("length", _result.length);
    return true;
}

bool Sol2D::Lua::tryGetMotorJointDefinition(lua_State * _lua, int _idx, World::MotorJointDefinition & _result)
{
    LuaTableApi table(_lua, _idx);
    if(!tryGetJointDefinition(table, _result))
        return false;
    table.tryGetPoint("linearVelocity", _result.linear_velocity);
    table.tryGetNumber("maxVelocityForce", _result.max_velocity_force);
    table.tryGetNumber("angularVelocity", _result.angular_velocity);
    table.tryGetNumber("maxVelocityTorque", _result.max_velocity_torque);
    table.tryGetNumber("linearHertz", _result.linear_hertz);
    table.tryGetNumber("linearDampingRatio", _result.linear_damping_ratio);
    table.tryGetNumber("maxSpringForce", _result.max_spring_force);
    table.tryGetNumber("angularHertz", _result.angular_hertz);
    table.tryGetNumber("angularDampingRatio", _result.angular_damping_ratio);
    table.tryGetNumber("maxSpringTorque", _result.max_spring_torque);
    return true;
}

bool Sol2D::Lua::tryGetPrismaticJointDefinition(lua_State * _lua, int _idx, World::PrismaticJointDefinition & _result)
{
    LuaTableApi table(_lua, _idx);
    if(!tryGetJointDefinition(table, _result))
        return false;
    table.tryGetBoolean("isSpringEnabled", &_result.is_spring_enabled);
    table.tryGetBoolean("isMotorEnabled", &_result.is_motor_enabled);
    table.tryGetBoolean("isLimitEnabled", &_result.is_limit_enabled);
    table.tryGetNumber("hertz", _result.hertz);
    table.tryGetNumber("dampingRatio", _result.damping_ratio);
    table.tryGetNumber("maxMotorForce", _result.max_motor_force);
    table.tryGetNumber("motorSpeed", _result.motor_speed);
    table.tryGetNumber("targetTranslation", _result.target_translation);
    table.tryGetNumber("lowerTranslation", _result.lower_translation);
    table.tryGetNumber("upperTranslation", _result.upper_translation);
    return true;
}

bool Sol2D::Lua::tryGetWeldJointDefinition(lua_State * _lua, int _idx, World::WeldJointDefinition & _result)
{
    LuaTableApi table(_lua, _idx);
    if(!tryGetJointDefinition(table, _result))
        return false;
    table.tryGetNumber("linearHertz", _result.linear_hertz);
    table.tryGetNumber("angularHertz", _result.angular_hertz);
    table.tryGetNumber("linearDampingRatio", _result.linear_damping_ratio);
    table.tryGetNumber("angularDampingRatio", _result.angular_damping_ratio);
    return true;
}

bool Sol2D::Lua::tryGetWheelJointDefinition(lua_State * _lua, int _idx, World::WheelJointDefinition & _result)
{
    LuaTableApi table(_lua, _idx);
    if(!tryGetJointDefinition(table, _result))
        return false;
    table.tryGetBoolean("isSpringEnabled", &_result.is_spring_enabled);
    table.tryGetBoolean("isMotorEnabled", &_result.is_motor_enabled);
    table.tryGetBoolean("isLimitEnabled", &_result.is_limit_enabled);
    table.tryGetNumber("hertz", _result.hertz);
    table.tryGetNumber("dampingRatio", _result.damping_ratio);
    table.tryGetNumber("maxMotorTorque", _result.max_motor_torque);
    table.tryGetNumber("motorSpeed", _result.motor_speed);
    table.tryGetNumber("lowerTranslation", _result.lower_translation);
    table.tryGetNumber("upperTranslation", _result.upper_translation);
    return true;
}
