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
    if(!_table.isValid())
        return false;
    uint64_t body_a, body_b;
    if(!tryGetBodyIdFromJointDefinition(_table, "bodyA", &body_a) ||
       !tryGetBodyIdFromJointDefinition(_table, "bodyB", &body_b))
    {
        return false;
    }
    _table.tryGetBoolean("isCollideConnectedEnabled", &_result.is_collide_connected_enabled);
    _result.body_a_id = body_a;
    _result.body_b_id = body_b;
    return true;
}

} // namespace

bool Sol2D::Lua::tryGetDistanceJointDefenition(lua_State * _lua, int _idx, World::DistanceJointDefenition & _result)
{
    LuaTableApi table(_lua, _idx);
    if(!tryGetJointDefinition(table, _result))
        return false;
    table.tryGetBoolean("isSpringEnbaled", &_result.is_spring_enabled);
    table.tryGetBoolean("isMotorEnbaled", &_result.is_motor_enabled);
    table.tryGetBoolean("isLimitEnbaled", &_result.is_limit_enabled);
    table.tryGetPoint("localAnchorA", _result.local_anchor_a);
    table.tryGetPoint("localAnchorB", _result.local_anchor_b);
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
    table.tryGetPoint("linearOffset", _result.linear_offset);
    table.tryGetNumber("angularOffset", _result.angular_offset);
    table.tryGetNumber("maxForce", _result.max_force);
    table.tryGetNumber("maxTorque", _result.max_torque);
    table.tryGetNumber("correctionFactor", _result.correction_factor);
    return true;
}

bool Sol2D::Lua::tryGetMouseJointDefinition(lua_State * _lua, int _idx, World::MouseJointDefinition & _result)
{
    LuaTableApi table(_lua, _idx);
    if(!tryGetJointDefinition(table, _result))
        return false;
    table.tryGetPoint("target", _result.target);
    table.tryGetNumber("hertz", _result.hertz);
    table.tryGetNumber("dampingRatio", _result.damping_ratio);
    table.tryGetNumber("maxForce", _result.max_force);
    return true;
}

bool Sol2D::Lua::tryGetPrismaticJointDefinition(lua_State * _lua, int _idx, World::PrismaticJointDefinition & _result)
{
    LuaTableApi table(_lua, _idx);
    if(!tryGetJointDefinition(table, _result))
        return false;
    table.tryGetBoolean("isSpringEnbaled", &_result.is_spring_enabled);
    table.tryGetBoolean("isMotorEnbaled", &_result.is_motor_enabled);
    table.tryGetBoolean("isLimitEnbaled", &_result.is_limit_enabled);
    table.tryGetPoint("localAnchorA", _result.local_anchor_a);
    table.tryGetPoint("localAnchorB", _result.local_anchor_b);
    table.tryGetPoint("localAxisA", _result.local_axis_a);
    table.tryGetNumber("hertz", _result.hertz);
    table.tryGetNumber("dampingRatio", _result.damping_ratio);
    table.tryGetNumber("maxMotorForce", _result.max_motor_force);
    table.tryGetNumber("motorSpeed", _result.motor_speed);
    table.tryGetNumber("reference_angle", _result.reference_angle);
    table.tryGetNumber("lowerTranslation", _result.lower_translation);
    table.tryGetNumber("upperTranslation", _result.upper_translation);
    return true;
}

bool Sol2D::Lua::tryGetWeldJointDefinition(lua_State * _lua, int _idx, World::WeldJointDefinition & _result)
{
    LuaTableApi table(_lua, _idx);
    if(!tryGetJointDefinition(table, _result))
        return false;
    table.tryGetPoint("localAnchorA", _result.local_anchor_a);
    table.tryGetPoint("localAnchorB", _result.local_anchor_b);
    table.tryGetNumber("reference_angle", _result.reference_angle);
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
    table.tryGetBoolean("isSpringEnbaled", &_result.is_spring_enabled);
    table.tryGetBoolean("isMotorEnbaled", &_result.is_motor_enabled);
    table.tryGetBoolean("isLimitEnbaled", &_result.is_limit_enabled);
    table.tryGetPoint("localAnchorA", _result.local_anchor_a);
    table.tryGetPoint("localAnchorB", _result.local_anchor_b);
    table.tryGetPoint("localAxisA", _result.local_axis_a);
    table.tryGetNumber("hertz", _result.hertz);
    table.tryGetNumber("dampingRatio", _result.damping_ratio);
    table.tryGetNumber("maxMotorTorque", _result.max_motor_torque);
    table.tryGetNumber("motorSpeed", _result.motor_speed);
    table.tryGetNumber("lowerTranslation", _result.lower_translation);
    table.tryGetNumber("upperTranslation", _result.upper_translation);
    return true;
}
