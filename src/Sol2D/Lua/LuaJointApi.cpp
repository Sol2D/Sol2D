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

#include <Sol2D/Lua/LuaJointApi.h>
#include <Sol2D/Lua/LuaBodyApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Utils/Array.h>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Lua;

namespace {

struct BasicSelf : LuaSelfBase
{
protected:
    explicit BasicSelf(std::shared_ptr<Scene> & _scene) :
        m_scene(_scene)
    {
    }

public:
    virtual Joint & getJoint() = 0;

    std::shared_ptr<Scene> getScene(lua_State * _lua) const
    {
        std::shared_ptr<Scene> ptr = m_scene.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::scene_is_destroyed);
        return ptr;
    }

private:
    std::weak_ptr<Scene> m_scene;
};

struct DistanceJointSelf : BasicSelf
{
public:
    DistanceJointSelf(std::shared_ptr<Scene> & _scene, DistanceJoint && _joint) :
        BasicSelf(_scene),
        m_distance_joint(std::move(_joint))
    {
    }

    Joint & getJoint() override
    {
        return m_distance_joint;
    }

    DistanceJoint & getDistanceJoint()
    {
        return m_distance_joint;
    }

private:
    DistanceJoint m_distance_joint;
};

struct MotorJointSelf : BasicSelf
{
public:
    MotorJointSelf(std::shared_ptr<Scene> & _scene, MotorJoint && _joint) :
        BasicSelf(_scene),
        m_motor_joint(std::move(_joint))
    {
    }

    Joint & getJoint() override
    {
        return m_motor_joint;
    }

    MotorJoint & getMotorJoint()
    {
        return m_motor_joint;
    }

private:
    MotorJoint m_motor_joint;
};

struct MouseJointSelf : BasicSelf
{
public:
    MouseJointSelf(std::shared_ptr<Scene> & _scene, MouseJoint && _joint) :
        BasicSelf(_scene),
        m_mouse_joint(std::move(_joint))
    {
    }

    Joint & getJoint() override
    {
        return m_mouse_joint;
    }

    MouseJoint & getMouseJoint()
    {
        return m_mouse_joint;
    }

private:
    MouseJoint m_mouse_joint;
};

struct PrismaticJointSelf : BasicSelf
{
public:
    PrismaticJointSelf(std::shared_ptr<Scene> & _scene, PrismaticJoint && _joint) :
        BasicSelf(_scene),
        m_prismatic_joint(std::move(_joint))
    {
    }

    Joint & getJoint() override
    {
        return m_prismatic_joint;
    }

    PrismaticJoint & getPrismaticJoint()
    {
        return m_prismatic_joint;
    }

private:
    PrismaticJoint m_prismatic_joint;
};

struct RevoluteJointSelf : BasicSelf
{
public:
    RevoluteJointSelf(std::shared_ptr<Scene> & _scene, RevoluteJoint && _joint) :
        BasicSelf(_scene),
        m_revolute_joint(std::move(_joint))
    {
    }

    Joint & getJoint() override
    {
        return m_revolute_joint;
    }

    RevoluteJoint & getRevoluteJoint()
    {
        return m_revolute_joint;
    }

private:
    RevoluteJoint m_revolute_joint;
};

struct WeldJointSelf : BasicSelf
{
public:
    WeldJointSelf(std::shared_ptr<Scene> & _scene, WeldJoint && _joint) :
        BasicSelf(_scene),
        m_weld_joint(std::move(_joint))
    {
    }

    Joint & getJoint() override
    {
        return m_weld_joint;
    }

    WeldJoint & getWeldJoint()
    {
        return m_weld_joint;
    }

private:
    WeldJoint m_weld_joint;
};

struct WheelJointSelf : BasicSelf
{
public:
    WheelJointSelf(std::shared_ptr<Scene> & _scene, const WheelJoint & _joint) :
        BasicSelf(_scene),
        m_wheel_joint(std::move(_joint))
    {
    }

    Joint & getJoint() override
    {
        return m_wheel_joint;
    }

    WheelJoint & getWheelJoint()
    {
        return m_wheel_joint;
    }

private:
    WheelJoint m_wheel_joint;
};

using DistanceJointUserData = LuaUserData<DistanceJointSelf, LuaTypeName::distance_joint>;
using MotorJointUserData = LuaUserData<MotorJointSelf, LuaTypeName::motor_joint>;
using MouseJointUserData = LuaUserData<MouseJointSelf, LuaTypeName::mouse_joint>;
using PrismaticJointUserData = LuaUserData<PrismaticJointSelf, LuaTypeName::prismatic_joint>;
using RevoluteJointUserData = LuaUserData<RevoluteJointSelf, LuaTypeName::revolute_joint>;
using WeldJointUserData = LuaUserData<WeldJointSelf, LuaTypeName::weld_joint>;
using WheelJointUserData = LuaUserData<WheelJointSelf, LuaTypeName::wheel_joint>;

// 1 self
int luaApi_IsValid(lua_State * _lua)
{
    BasicSelf * self = getLuaUserData<BasicSelf>(_lua, 1, getAllJointTypes());
    lua_pushboolean(_lua, self->getJoint().isValid());
    return 1;
}

// 1 self
int luaApi_GetId(lua_State * _lua)
{
    BasicSelf * self = getLuaUserData<BasicSelf>(_lua, 1, getAllJointTypes());
    lua_pushinteger(_lua, self->getJoint().getGid());
    return 1;
}

// 1 self
int luaApi_GetBodyA(lua_State * _lua)
{
    BasicSelf * self = getLuaUserData<BasicSelf>(_lua, 1, getAllJointTypes());
    pushBodyApi(_lua, self->getScene(_lua), self->getJoint().getBodyA());
    return 1;
}

// 1 self
int luaApi_GetBodyB(lua_State * _lua)
{
    BasicSelf * self = getLuaUserData<BasicSelf>(_lua, 1, getAllJointTypes());
    pushBodyApi(_lua, self->getScene(_lua), self->getJoint().getBodyB());
    return 1;
}

// 1 self
int luaApi_GetLocalAnchorA(lua_State * _lua)
{
    BasicSelf * self = getLuaUserData<BasicSelf>(_lua, 1, getAllJointTypes());
    SDL_FPoint anchor = self->getJoint().getLocalAnchorA();
    pushPoint(_lua, anchor.x, anchor.y);
    return 1;
}

// 1 self
int luaApi_GetLocalAnchorB(lua_State * _lua)
{
    BasicSelf * self = getLuaUserData<BasicSelf>(_lua, 1, getAllJointTypes());
    SDL_FPoint anchor = self->getJoint().getLocalAnchorB();
    pushPoint(_lua, anchor.x, anchor.y);
    return 1;
}

// 1 self
int luaApi_IsCollideConnectedEnabled(lua_State * _lua)
{
    BasicSelf * self = getLuaUserData<BasicSelf>(_lua, 1, getAllJointTypes());
    lua_pushboolean(_lua, self->getJoint().isCollideConnectedEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableCollideConnected(lua_State * _lua)
{
    BasicSelf * self = getLuaUserData<BasicSelf>(_lua, 1, getAllJointTypes());
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getJoint().enableCollideConnected(lua_toboolean(_lua, 2));
    self->getJoint().wakeBodies();
    return 0;
}

namespace DistanceJointApi {

// 1 self
int luaApi_GetLength(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getDistanceJoint().getLength());
    return 1;
}

// 1 self
// 2 length
int luaApi_SetLength(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getDistanceJoint().setLength(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getDistanceJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_IsSpringEnabled(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getDistanceJoint().isSpringEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableSpring(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getDistanceJoint().enableSpring(lua_toboolean(_lua, 2));
    self->getDistanceJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringDampingRatio(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getDistanceJoint().getSpringDampingRatio());
    return 1;
}

// 1 self
// 2 ratio
int luaApi_SetSpringDampingRatio(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getDistanceJoint().setSpringDampingRatio(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getDistanceJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringHertz(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getDistanceJoint().getSpringHertz());
    return 1;
}

// 1 self
// 2 hertz
int luaApi_SetSpringHertz(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getDistanceJoint().setSpringHertz(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getDistanceJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_IsLimtEnabled(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getDistanceJoint().isLimtEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableLimit(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getDistanceJoint().enableLimit(lua_toboolean(_lua, 2));
    self->getDistanceJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMinLength(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getDistanceJoint().getMinLength());
    return 1;
}

// 1 self
int luaApi_GetMaxLength(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getDistanceJoint().getMaxLength());
    return 1;
}

// 1 self
int luaApi_GetCurrentLength(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getDistanceJoint().getCurrentLength());
    return 1;
}

// 1 self
// 2 min length
// 3 max length
int luaApi_SetLengthRange(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    luaL_argexpected(_lua, lua_isnumber(_lua, 3), 3, LuaTypeName::number);
    self->getDistanceJoint().setLengthRange(
        static_cast<float>(lua_tonumber(_lua, 2)), static_cast<float>(lua_tonumber(_lua, 3))
    );
    self->getDistanceJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_IsMotorEnabled(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getDistanceJoint().isMotorEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableMotor(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getDistanceJoint().enableMotor(lua_toboolean(_lua, 2));
    self->getDistanceJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMotorSpeed(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getDistanceJoint().getMotorSpeed());
    return 1;
}

// 1 self
// 2 speed
int luaApi_SetMotorSpeed(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getDistanceJoint().setMotorSpeed(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getDistanceJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMotorForce(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getDistanceJoint().getMotorForce());
    return 1;
}

// 1 self
int luaApi_GetMaxMotorForce(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getDistanceJoint().getMaxMotorForce());
    return 1;
}

// 1 self
// 2 force
int luaApi_SetMaxMotorForce(lua_State * _lua)
{
    DistanceJointSelf * self = DistanceJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getDistanceJoint().setMaxMotorForce(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getDistanceJoint().wakeBodies();
    return 0;
}

} // namespace DistanceJointApi

namespace MotorJointApi {

// 1 self
int luaApi_GetLinearOffset(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    SDL_FPoint offsert = self->getMotorJoint().getLinearOffset();
    pushPoint(_lua, offsert.x, offsert.y);
    return 1;
}

// 1 self
// 2 offset
int luaApi_SetLinearOffset(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    SDL_FPoint offset;
    luaL_argexpected(_lua, tryGetPoint(_lua, 2, offset), 2, LuaTypeName::point);
    self->getMotorJoint().setLinearOffset(offset);
    self->getMotorJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetAngularOffset(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getMotorJoint().getAngularOffset());
    return 1;
}

// 1 self
// 2 offset
int luaApi_SetAngularOffset(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getMotorJoint().setAngularOffset(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getMotorJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMaxForce(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getMotorJoint().getMaxForce());
    return 1;
}

// 1 self
// 2 force
int luaApi_SetMaxForce(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getMotorJoint().setMaxForce(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getMotorJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMaxTorque(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getMotorJoint().getMaxTorque());
    return 1;
}

// 1 self
// 2 torque
int luaApi_SetMaxTorque(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getMotorJoint().setMaxTorque(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getMotorJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetCorrectionFactor(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getMotorJoint().getCorrectionFactor());
    return 1;
}

// 1 self
// 2 correction factor
int luaApi_SetCorrectionFactor(lua_State * _lua)
{
    MotorJointSelf * self = MotorJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getMotorJoint().setCorrectionFactor(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getMotorJoint().wakeBodies();
    return 0;
}

} // namespace MotorJointApi

namespace MouseJointApi {

// 1 self
int luaApi_GetTarget(lua_State * _lua)
{
    MouseJointSelf * self = MouseJointUserData::getUserData(_lua, 1);
    SDL_FPoint target = self->getMouseJoint().getTarget();
    pushPoint(_lua, target.x, target.y);
    return 1;
}

// 1 self
// 2 target
int luaApi_SetTarget(lua_State * _lua)
{
    MouseJointSelf * self = MouseJointUserData::getUserData(_lua, 1);
    SDL_FPoint target;
    luaL_argexpected(_lua, tryGetPoint(_lua, 2, target), 2, LuaTypeName::point);
    self->getMouseJoint().setTarget(target);
    self->getMouseJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringHertz(lua_State * _lua)
{
    MouseJointSelf * self = MouseJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getMouseJoint().getSpringHertz());
    return 1;
}

// 1 self
// 2 hertz
int luaApi_SetSpringHertz(lua_State * _lua)
{
    MouseJointSelf * self = MouseJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getMouseJoint().setSpringHertz(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getMouseJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringDampingRatio(lua_State * _lua)
{
    MouseJointSelf * self = MouseJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getMouseJoint().getSpringDampingRatio());
    return 1;
}

// 1 self
// 2 ratio
int luaApi_SetSpringDampingRatio(lua_State * _lua)
{
    MouseJointSelf * self = MouseJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getMouseJoint().setSpringDampingRatio(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getMouseJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMaxForce(lua_State * _lua)
{
    MouseJointSelf * self = MouseJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getMouseJoint().getMaxForce());
    return 1;
}

// 1 self
// 2 force
int luaApi_SetMaxForce(lua_State * _lua)
{
    MouseJointSelf * self = MouseJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getMouseJoint().setMaxForce(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getMouseJoint().wakeBodies();
    return 0;
}

} // namespace MouseJointApi

namespace PrismaticJointApi {

// 1 self
int luaApi_IsSpringEnabled(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getPrismaticJoint().isSpringEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableSpring(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getPrismaticJoint().enableSpring(lua_toboolean(_lua, 2));
    self->getPrismaticJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringHertz(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getPrismaticJoint().getSpringHertz());
    return 1;
}

// 1 self
// 2 hertz
int luaApi_SetSpringHertz(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getPrismaticJoint().setSpringHertz(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getPrismaticJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringDampingRatio(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getPrismaticJoint().getSpringDampingRatio());
    return 1;
}

// 1 self
// 2 ratio
int luaApi_SetSpringDampingRatio(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getPrismaticJoint().setSpringDampingRatio(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getPrismaticJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_IsLimitEnabled(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getPrismaticJoint().isLimitEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableLimit(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getPrismaticJoint().enableLimit(lua_toboolean(_lua, 2));
    self->getPrismaticJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetLowerLimit(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getPrismaticJoint().getLowerLimit());
    return 1;
}

// 1 self
int luaApi_GetUpperLimit(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getPrismaticJoint().getUpperLimit());
    return 1;
}

// 1 self
// 2 lower limit
// 3 uppoer limit
int luaApi_SetLimits(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    luaL_argexpected(_lua, lua_isnumber(_lua, 3), 3, LuaTypeName::number);
    self->getPrismaticJoint().setLimits(
        static_cast<float>(lua_tonumber(_lua, 2)), static_cast<float>(lua_tonumber(_lua, 3))
    );
    self->getPrismaticJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_IsMotorEnabled(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getPrismaticJoint().isMotorEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableMotor(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getPrismaticJoint().enableMotor(lua_toboolean(_lua, 2));
    self->getPrismaticJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMotorSpeed(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getPrismaticJoint().getMotorSpeed());
    return 1;
}

// 1 self
// 2 speed
int luaApi_SetMotorSpeed(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getPrismaticJoint().setMotorSpeed(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getPrismaticJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMaxMotorForce(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getPrismaticJoint().getMaxMotorForce());
    return 1;
}

// 1 self
int luaApi_GetMotorForce(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getPrismaticJoint().getMotorForce());
    return 1;
}

// 1 self
// 2 force
int luaApi_SetMaxMotorForce(lua_State * _lua)
{
    PrismaticJointSelf * self = PrismaticJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getPrismaticJoint().setMaxMotorForce(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getPrismaticJoint().wakeBodies();
    return 0;
}

} // namespace PrismaticJointApi

namespace RevoluteJointApi {

// 1 self
int luaApi_IsSpringEnabled(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getRevoluteJoint().isSpringEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableSpring(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getRevoluteJoint().enableSpring(lua_toboolean(_lua, 2));
    self->getRevoluteJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringHertz(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getRevoluteJoint().getSpringHertz());
    return 1;
}

// 1 self
// 2 hertz
int luaApi_SetSpringHertz(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getRevoluteJoint().setSpringHertz(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getRevoluteJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringDampingRatio(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getRevoluteJoint().getSpringDampingRatio());
    return 1;
}

// 1 self
// 2 ratio
int luaApi_SetSpringDampingRatio(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getRevoluteJoint().setSpringDampingRatio(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getRevoluteJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetAngle(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getRevoluteJoint().getAngle());
    return 1;
}

// 1 self
int luaApi_IsLimitEnabled(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getRevoluteJoint().isLimitEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableLimit(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getRevoluteJoint().enableLimit(lua_toboolean(_lua, 2));
    self->getRevoluteJoint().wakeBodies();
    return 0;
}

// 1 self
// 2 lower limit
// 3 upper limit
int luaApi_SetLimits(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    luaL_argexpected(_lua, lua_isnumber(_lua, 3), 3, LuaTypeName::number);
    self->getRevoluteJoint().setLimits(
        static_cast<float>(lua_tonumber(_lua, 2)), static_cast<float>(lua_tonumber(_lua, 3))
    );
    self->getRevoluteJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetLowerLimit(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getRevoluteJoint().getLowerLimit());
    return 1;
}

// 1 self
int luaApi_GetUpperLimit(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getRevoluteJoint().getUpperLimit());
    return 1;
}

// 1 self
int luaApi_IsMotorEnabled(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getRevoluteJoint().isMotorEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableMotor(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getRevoluteJoint().enableMotor(lua_toboolean(_lua, 2));
    self->getRevoluteJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMotorSpeed(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getRevoluteJoint().getMotorSpeed());
    return 1;
}

// 1 self
// 2 speed
int luaApi_SetMotorSpeed(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getRevoluteJoint().setMotorSpeed(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getRevoluteJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMotorTorque(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getRevoluteJoint().getMotorTorque());
    return 1;
}

// 1 self
int luaApi_GetMaxMotorTorque(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getRevoluteJoint().getMaxMotorTorque());
    return 1;
}

// 1 self
// 2 torque
int luaApi_SetMaxMotorTorque(lua_State * _lua)
{
    RevoluteJointSelf * self = RevoluteJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getRevoluteJoint().setMaxMotorTorque(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getRevoluteJoint().wakeBodies();
    return 0;
}

} // namespace RevoluteJointApi

namespace WeldJointApi {

// 1 self
int luaApi_GetReferenceAngle(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWeldJoint().getReferenceAngle());
    return 1;
}

// 1 self
// 2 angle
int luaApi_SetReferenceAngle(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getWeldJoint().setReferenceAngle(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getWeldJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetLinearHertz(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWeldJoint().getLinearHertz());
    return 1;
}

// 1 self
// 2 hertz
int luaApi_SetLinearHertz(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getWeldJoint().setLinearHertz(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getWeldJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetLinearDampingRatio(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWeldJoint().getLinearDampingRatio());
    return 1;
}

// 1 self
// 2 dapmping
int luaApi_SetLinearDampingRatio(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getWeldJoint().setLinearDampingRatio(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getWeldJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetAngularHertz(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWeldJoint().getAngularHertz());
    return 1;
}

// 1 self
// 2 hertz
int luaApi_SetAngularHertz(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getWeldJoint().setAngularHertz(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getWeldJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetAngularDampingRatio(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWeldJoint().getAngularDampingRatio());
    return 1;
}

// 1 self
// 2 ratio
int luaApi_SetAngularDampingRatio(lua_State * _lua)
{
    WeldJointSelf * self = WeldJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getWeldJoint().setAngularDampingRatio(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getWeldJoint().wakeBodies();
    return 0;
}

} // namespace WeldJointApi

namespace WheelJointApi {

// 1 self
int luaApi_IsSpringEnabled(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getWheelJoint().isSpringEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableSpring(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getWheelJoint().enableSpring(lua_toboolean(_lua, 2));
    self->getWheelJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringHertz(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWheelJoint().getSpringHertz());
    return 1;
}

// 1 self
// 2 hertz
int luaApi_SetSpringHertz(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getWheelJoint().setSpringHertz(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getWheelJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetSpringDampingRatio(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWheelJoint().getSpringDampingRatio());
    return 1;
}

// 1 self
// 2 ratio
int luaApi_SetSpringDampingRatio(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getWheelJoint().setSpringDampingRatio(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getWheelJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_IsLimitEnabled(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getWheelJoint().isLimitEnabled());
    return 1;
}

// 1 self
int luaApi_GetLowerLimit(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWheelJoint().getLowerLimit());
    return 1;
}

// 1 self
int luaApi_GetUpperLimit(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWheelJoint().getUpperLimit());
    return 1;
}

// 1 self
// 2 lower
// 3 upper
int luaApi_SetLimits(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    luaL_argexpected(_lua, lua_isnumber(_lua, 3), 3, LuaTypeName::number);
    self->getWheelJoint().setLimits(
        static_cast<float>(lua_tonumber(_lua, 2)), static_cast<float>(lua_tonumber(_lua, 3))
    );
    self->getWheelJoint().wakeBodies();
    return 0;
}

// 1 self
// 2 is enabled?
int luaApi_EnableLimit(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getWheelJoint().enableLimit(lua_toboolean(_lua, 2));
    self->getWheelJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_IsMotorEnabled(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getWheelJoint().isMotorEnabled());
    return 1;
}

// 1 self
// 2 is enabled?
int luaApi_EnableMotor(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isboolean(_lua, 2), 2, LuaTypeName::boolean);
    self->getWheelJoint().enableMotor(lua_toboolean(_lua, 2));
    self->getWheelJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMotorSpeed(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWheelJoint().getMotorSpeed());
    return 1;
}

// 1 self
// 2 speed
int luaApi_SetMotorSpeed(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getWheelJoint().setMotorSpeed(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getWheelJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMaxMotorTorque(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWheelJoint().getMaxMotorTorque());
    return 1;
}

// 1 self
// 2 torque
int luaApi_SetMaxMotorTorque(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isnumber(_lua, 2), 2, LuaTypeName::number);
    self->getWheelJoint().setMaxMotorTorque(static_cast<float>(lua_tonumber(_lua, 2)));
    self->getWheelJoint().wakeBodies();
    return 0;
}

// 1 self
int luaApi_GetMotorTorque(lua_State * _lua)
{
    WheelJointSelf * self = WheelJointUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getWheelJoint().getMotorTorque());
    return 1;
}

} // namespace WheelJointApi

template<typename UserDataT>
constexpr std::array<luaL_Reg, 9> gc_joint_funcs = {
    {{"__gc", UserDataT::luaGC},
     {"isValid", luaApi_IsValid},
     {"getId", luaApi_GetId},
     {"getBodyA", luaApi_GetBodyA},
     {"getBodyB", luaApi_GetBodyB},
     {"getLocalAnchorA", luaApi_GetLocalAnchorA},
     {"getLocalAnchorB", luaApi_GetLocalAnchorB},
     {"isCollideConnectedEnabled", luaApi_IsCollideConnectedEnabled},
     {"enableCollideConnected", luaApi_EnableCollideConnected}}
};

constexpr std::array<luaL_Reg, 21> gc_distance_joint_funcs = {
    {{"getLength", DistanceJointApi::luaApi_GetLength},
     {"setLength", DistanceJointApi::luaApi_SetLength},
     {"isSpringEnabled", DistanceJointApi::luaApi_IsSpringEnabled},
     {"enableSpring", DistanceJointApi::luaApi_EnableSpring},
     {"getSpringDampingRatio", DistanceJointApi::luaApi_GetSpringDampingRatio},
     {"setSpringDampingRatio", DistanceJointApi::luaApi_SetSpringDampingRatio},
     {"getSpringHertz", DistanceJointApi::luaApi_GetSpringHertz},
     {"setSpringHertz", DistanceJointApi::luaApi_SetSpringHertz},
     {"isLimtEnabled", DistanceJointApi::luaApi_IsLimtEnabled},
     {"enableLimit", DistanceJointApi::luaApi_EnableLimit},
     {"getMinLength", DistanceJointApi::luaApi_GetMinLength},
     {"getMaxLength", DistanceJointApi::luaApi_GetMaxLength},
     {"getCurrentLength", DistanceJointApi::luaApi_GetCurrentLength},
     {"setLengthRange", DistanceJointApi::luaApi_SetLengthRange},
     {"isMotorEnabled", DistanceJointApi::luaApi_IsMotorEnabled},
     {"enableMotor", DistanceJointApi::luaApi_EnableMotor},
     {"getMotorSpeed", DistanceJointApi::luaApi_GetMotorSpeed},
     {"setMotorSpeed", DistanceJointApi::luaApi_SetMotorSpeed},
     {"getMotorForce", DistanceJointApi::luaApi_GetMotorForce},
     {"getMaxMotorForce", DistanceJointApi::luaApi_GetMaxMotorForce},
     {"setMaxMotorForce", DistanceJointApi::luaApi_SetMaxMotorForce}}
};

constexpr std::array<luaL_Reg, 10> gc_motor_joint_funcs = {
    {{"getLinearOffset", MotorJointApi::luaApi_GetLinearOffset},
     {"setLinearOffset", MotorJointApi::luaApi_SetLinearOffset},
     {"getAngularOffset", MotorJointApi::luaApi_GetAngularOffset},
     {"setAngularOffset", MotorJointApi::luaApi_SetAngularOffset},
     {"getMaxForce", MotorJointApi::luaApi_GetMaxForce},
     {"setMaxForce", MotorJointApi::luaApi_SetMaxForce},
     {"getMaxTorque", MotorJointApi::luaApi_GetMaxTorque},
     {"setMaxTorque", MotorJointApi::luaApi_SetMaxTorque},
     {"getCorrectionFactor", MotorJointApi::luaApi_GetCorrectionFactor},
     {"setCorrectionFactor", MotorJointApi::luaApi_SetCorrectionFactor}}
};

constexpr std::array<luaL_Reg, 8> gc_mouse_joint_funcs = {
    {{"getTarget", MouseJointApi::luaApi_GetTarget},
     {"setTarget", MouseJointApi::luaApi_SetTarget},
     {"getSpringHertz", MouseJointApi::luaApi_GetSpringHertz},
     {"setSpringHertz", MouseJointApi::luaApi_SetSpringHertz},
     {"getSpringDampingRatio", MouseJointApi::luaApi_GetSpringDampingRatio},
     {"setSpringDampingRatio", MouseJointApi::luaApi_SetSpringDampingRatio},
     {"getMaxForce", MouseJointApi::luaApi_GetMaxForce},
     {"setMaxForce", MouseJointApi::luaApi_SetMaxForce}}
};

constexpr std::array<luaL_Reg, 18> gc_prismatic_joint_funcs = {
    {{"isSpringEnabled", PrismaticJointApi::luaApi_IsSpringEnabled},
     {"enableSpring", PrismaticJointApi::luaApi_EnableSpring},
     {"getSpringHertz", PrismaticJointApi::luaApi_GetSpringHertz},
     {"setSpringHertz", PrismaticJointApi::luaApi_SetSpringHertz},
     {"getSpringDampingRatio", PrismaticJointApi::luaApi_GetSpringDampingRatio},
     {"setSpringDampingRatio", PrismaticJointApi::luaApi_SetSpringDampingRatio},
     {"isLimitEnabled", PrismaticJointApi::luaApi_IsLimitEnabled},
     {"enableLimit", PrismaticJointApi::luaApi_EnableLimit},
     {"getLowerLimit", PrismaticJointApi::luaApi_GetLowerLimit},
     {"getUpperLimit", PrismaticJointApi::luaApi_GetUpperLimit},
     {"setLimits", PrismaticJointApi::luaApi_SetLimits},
     {"isMotorEnabled", PrismaticJointApi::luaApi_IsMotorEnabled},
     {"enableMotor", PrismaticJointApi::luaApi_EnableMotor},
     {"getMotorSpeed", PrismaticJointApi::luaApi_GetMotorSpeed},
     {"setMotorSpeed", PrismaticJointApi::luaApi_SetMotorSpeed},
     {"getMaxMotorForce", PrismaticJointApi::luaApi_GetMaxMotorForce},
     {"getMotorForce", PrismaticJointApi::luaApi_GetMotorForce},
     {"setMaxMotorForce", PrismaticJointApi::luaApi_SetMaxMotorForce}}
};

constexpr std::array<luaL_Reg, 19> gc_revolute_joint_funcs = {
    {{"isSpringEnabled", RevoluteJointApi::luaApi_IsSpringEnabled},
     {"enableSpring", RevoluteJointApi::luaApi_EnableSpring},
     {"getSpringHertz", RevoluteJointApi::luaApi_GetSpringHertz},
     {"setSpringHertz", RevoluteJointApi::luaApi_SetSpringHertz},
     {"getSpringDampingRatio", RevoluteJointApi::luaApi_GetSpringDampingRatio},
     {"setSpringDampingRatio", RevoluteJointApi::luaApi_SetSpringDampingRatio},
     {"getAngle", RevoluteJointApi::luaApi_GetAngle},
     {"isLimitEnabled", RevoluteJointApi::luaApi_IsLimitEnabled},
     {"enableLimit", RevoluteJointApi::luaApi_EnableLimit},
     {"setLimits", RevoluteJointApi::luaApi_SetLimits},
     {"getLowerLimit", RevoluteJointApi::luaApi_GetLowerLimit},
     {"getUpperLimit", RevoluteJointApi::luaApi_GetUpperLimit},
     {"isMotorEnabled", RevoluteJointApi::luaApi_IsMotorEnabled},
     {"enableMotor", RevoluteJointApi::luaApi_EnableMotor},
     {"getMotorSpeed", RevoluteJointApi::luaApi_GetMotorSpeed},
     {"setMotorSpeed", RevoluteJointApi::luaApi_SetMotorSpeed},
     {"getMotorTorque", RevoluteJointApi::luaApi_GetMotorTorque},
     {"getMaxMotorTorque", RevoluteJointApi::luaApi_GetMaxMotorTorque},
     {"setMaxMotorTorque", RevoluteJointApi::luaApi_SetMaxMotorTorque}}
};

constexpr std::array<luaL_Reg, 10> gc_weld_joint_funcs = {
    {{"getReferenceAngle", WeldJointApi::luaApi_GetReferenceAngle},
     {"setReferenceAngle", WeldJointApi::luaApi_SetReferenceAngle},
     {"getLinearHertz", WeldJointApi::luaApi_GetLinearHertz},
     {"setLinearHertz", WeldJointApi::luaApi_SetLinearHertz},
     {"getLinearDampingRatio", WeldJointApi::luaApi_GetLinearDampingRatio},
     {"setLinearDampingRatio", WeldJointApi::luaApi_SetLinearDampingRatio},
     {"getAngularHertz", WeldJointApi::luaApi_GetAngularHertz},
     {"setAngularHertz", WeldJointApi::luaApi_SetAngularHertz},
     {"getAngularDampingRatio", WeldJointApi::luaApi_GetAngularDampingRatio},
     {"setAngularDampingRatio", WeldJointApi::luaApi_SetAngularDampingRatio}}
};

constexpr std::array<luaL_Reg, 18> gc_wheel_joint_funcs = {
    {{"isSpringEnabled", WheelJointApi::luaApi_IsSpringEnabled},
     {"enableSpring", WheelJointApi::luaApi_EnableSpring},
     {"getSpringHertz", WheelJointApi::luaApi_GetSpringHertz},
     {"setSpringHertz", WheelJointApi::luaApi_SetSpringHertz},
     {"getSpringDampingRatio", WheelJointApi::luaApi_GetSpringDampingRatio},
     {"setSpringDampingRatio", WheelJointApi::luaApi_SetSpringDampingRatio},
     {"isLimitEnabled", WheelJointApi::luaApi_IsLimitEnabled},
     {"getLowerLimit", WheelJointApi::luaApi_GetLowerLimit},
     {"getUpperLimit", WheelJointApi::luaApi_GetUpperLimit},
     {"setLimits", WheelJointApi::luaApi_SetLimits},
     {"enableLimit", WheelJointApi::luaApi_EnableLimit},
     {"isMotorEnabled", WheelJointApi::luaApi_IsMotorEnabled},
     {"enableMotor", WheelJointApi::luaApi_EnableMotor},
     {"getMotorSpeed", WheelJointApi::luaApi_GetMotorSpeed},
     {"setMotorSpeed", WheelJointApi::luaApi_SetMotorSpeed},
     {"getMaxMotorTorque", WheelJointApi::luaApi_GetMaxMotorTorque},
     {"setMaxMotorTorque", WheelJointApi::luaApi_SetMaxMotorTorque},
     {"getMotorTorque", WheelJointApi::luaApi_GetMotorTorque}}
};

constexpr std::array<luaL_Reg, 1> gc_null_funcs = {{{nullptr, nullptr}}};

} // namespace

void Sol2D::Lua::pushJointApi(lua_State * _lua, std::shared_ptr<Scene> _scene, DistanceJoint && _joint)
{
    DistanceJointUserData::pushUserData(_lua, _scene, std::forward<DistanceJoint>(_joint));
    if(DistanceJointUserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        auto funcs = gc_joint_funcs<DistanceJointUserData> + gc_distance_joint_funcs + gc_null_funcs;
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushJointApi(lua_State * _lua, std::shared_ptr<Scene> _scene, MotorJoint && _joint)
{
    MotorJointUserData::pushUserData(_lua, _scene, std::forward<MotorJoint>(_joint));
    if(MotorJointUserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        auto funcs = gc_joint_funcs<MotorJointUserData> + gc_motor_joint_funcs + gc_null_funcs;
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushJointApi(lua_State * _lua, std::shared_ptr<Scene> _scene, MouseJoint && _joint)
{
    MouseJointUserData::pushUserData(_lua, _scene, std::forward<MouseJoint>(_joint));
    if(MouseJointUserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        auto funcs = gc_joint_funcs<MouseJointUserData> + gc_mouse_joint_funcs + gc_null_funcs;
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushJointApi(lua_State * _lua, std::shared_ptr<Scene> _scene, PrismaticJoint && _joint)
{
    PrismaticJointUserData::pushUserData(_lua, _scene, std::forward<PrismaticJoint>(_joint));
    if(PrismaticJointUserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        auto funcs = gc_joint_funcs<PrismaticJointUserData> + gc_prismatic_joint_funcs + gc_null_funcs;
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}

void pushJointApi(lua_State * _lua, std::shared_ptr<World::Scene> _scene, World::RevoluteJoint && _joint)
{
    RevoluteJointUserData::pushUserData(_lua, _scene, std::forward<RevoluteJoint>(_joint));
    if(RevoluteJointUserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        auto funcs = gc_joint_funcs<RevoluteJointUserData> + gc_revolute_joint_funcs + gc_null_funcs;
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushJointApi(lua_State * _lua, std::shared_ptr<Scene> _scene, WeldJoint && _joint)
{
    WeldJointUserData::pushUserData(_lua, _scene, std::forward<WeldJoint>(_joint));
    if(WeldJointUserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        auto funcs = gc_joint_funcs<WeldJointUserData> + gc_weld_joint_funcs + gc_null_funcs;
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushJointApi(lua_State * _lua, std::shared_ptr<Scene> _scene, WheelJoint && _joint)
{
    WheelJointUserData::pushUserData(_lua, _scene, std::forward<WheelJoint>(_joint));
    if(WheelJointUserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        auto funcs = gc_joint_funcs<WheelJointUserData> + gc_wheel_joint_funcs + gc_null_funcs;
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}

World::Joint * Sol2D::Lua::tryGetJoint(lua_State * _lua, int _idx)
{
    BasicSelf * self = tryGetLuaUserData<BasicSelf>(_lua, _idx, getAllJointTypes());
    return self ? &self->getJoint() : nullptr;
}
