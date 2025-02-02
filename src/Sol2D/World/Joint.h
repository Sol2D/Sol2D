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

#pragma once

#include <Sol2D/World/UserData.h>
#include <Sol2D/World/Body.h>
#include <Sol2D/Utils/SequentialId.h>

namespace Sol2D::World {

class Joint
{
protected:
    S2_DEFAULT_COPY_AND_MOVE(Joint)

    explicit Joint(b2JointId _b2_joint_id) :
        m_b2_joint_id(_b2_joint_id),
        m_gid(s_sequential_id.getNext())
    {
    }

public:
    virtual ~Joint() { }

    uint64_t getGid() const
    {
        return m_gid;
    }

    bool isValid() const
    {
        return b2Joint_IsValid(m_b2_joint_id);
    }

    uint64_t getBodyA() const
    {
        b2BodyId m_b2_body_id = b2Joint_GetBodyA(m_b2_joint_id);
        return B2_IS_NULL(m_b2_body_id) ? 0 : getUserData(m_b2_body_id)->getGid();
    }

    uint64_t getBodyB() const
    {
        b2BodyId m_b2_body_id = b2Joint_GetBodyB(m_b2_joint_id);
        return B2_IS_NULL(m_b2_body_id) ? 0 : getUserData(m_b2_body_id)->getGid();
    }

    SDL_FPoint getLocalAnchorA() const
    {
        return toSDL(b2Joint_GetLocalAnchorA(m_b2_joint_id));
    }

    SDL_FPoint getLocalAnchorB() const
    {
        return toSDL(b2Joint_GetLocalAnchorB(m_b2_joint_id));
    }

    bool isCollideConnectedEnabled() const
    {
        return b2Joint_GetCollideConnected(m_b2_joint_id);
    }

    void enableCollideConnected(bool _enable)
    {
        b2Joint_SetCollideConnected(m_b2_joint_id, _enable);
    }

    void wakeBodies()
    {
        b2Joint_WakeBodies(m_b2_joint_id);
    }

protected:
    b2JointId m_b2_joint_id;

private:
    static Utils::SequentialId<uint64_t> s_sequential_id;
    uint64_t m_gid;
};

class DistanceJoint : public Joint {
public:
    S2_DEFAULT_COPY_AND_MOVE(DistanceJoint)

    explicit DistanceJoint(b2JointId _b2_joint_id) :
        Joint(_b2_joint_id)
    {
    }

    float getLength() const
    {
        return b2DistanceJoint_GetLength(m_b2_joint_id);
    }

    void setLength(float _length)
    {
        b2DistanceJoint_SetLength(m_b2_joint_id, _length);
    }

    bool isSpringEnabled() const
    {
        return b2DistanceJoint_IsSpringEnabled(m_b2_joint_id);
    }

    void enableSpring(bool _enable)
    {
        b2DistanceJoint_EnableSpring(m_b2_joint_id, _enable);
    }

    float getSpringDampingRatio() const
    {
        return b2DistanceJoint_GetSpringDampingRatio(m_b2_joint_id);
    }

    void setSpringDampingRatio(float _ratio)
    {
        b2DistanceJoint_SetSpringDampingRatio(m_b2_joint_id, _ratio);
    }

    float getSpringHertz() const
    {
        return b2DistanceJoint_GetSpringHertz(m_b2_joint_id);
    }

    void setSpringHertz(float _hertz) const
    {
        b2DistanceJoint_SetSpringHertz(m_b2_joint_id, _hertz);
    }

    bool isLimtEnabled() const
    {
        return b2DistanceJoint_IsLimitEnabled(m_b2_joint_id);
    }

    void enableLimit(bool _enable)
    {
        b2DistanceJoint_EnableLimit(m_b2_joint_id, _enable);
    }

    float getMinLength() const
    {
        return b2DistanceJoint_GetMinLength(m_b2_joint_id);
    }

    float getMaxLength() const
    {
        return b2DistanceJoint_GetMaxLength(m_b2_joint_id);
    }

    float getCurrentLength() const
    {
        return b2DistanceJoint_GetCurrentLength(m_b2_joint_id);
    }

    void setLengthRange(float _min_length, float _max_length)
    {
        b2DistanceJoint_SetLengthRange(m_b2_joint_id, _min_length, _max_length);
    }

    bool isMotorEnabled() const
    {
        return b2DistanceJoint_IsMotorEnabled(m_b2_joint_id);
    }

    void enableMotor(bool _enable)
    {
        b2DistanceJoint_EnableMotor(m_b2_joint_id, _enable);
    }

    float getMotorSpeed() const
    {
        return b2DistanceJoint_GetMotorSpeed(m_b2_joint_id);
    }

    void setMotorSpeed(float _speed)
    {
        b2DistanceJoint_SetMotorSpeed(m_b2_joint_id, _speed);
    }

    float getMotorForce()
    {
        return b2DistanceJoint_GetMotorForce(m_b2_joint_id);
    }

    float getMaxMotorForce() const
    {
        return b2DistanceJoint_GetMaxMotorForce(m_b2_joint_id);
    }

    void setMaxMotorForce(float _force)
    {
        b2DistanceJoint_SetMaxMotorForce(m_b2_joint_id, _force);
    }
};

class MotorJoint : public Joint
{
public:
    S2_DEFAULT_COPY_AND_MOVE(MotorJoint)

    explicit MotorJoint(b2JointId _b2_joint_id) :
        Joint(_b2_joint_id)
    {
    }

    SDL_FPoint getLinearOffset() const
    {
        return toSDL(b2MotorJoint_GetLinearOffset(m_b2_joint_id));
    }

    void setLinearOffset(SDL_FPoint _offset)
    {
        b2MotorJoint_SetLinearOffset(m_b2_joint_id, toBox2D(_offset));
    }

    float getAngularOffset() const
    {
        return b2MotorJoint_GetAngularOffset(m_b2_joint_id);
    }

    void setAngularOffset(float _offset)
    {
        b2MotorJoint_SetAngularOffset(m_b2_joint_id, _offset);
    }

    float getMaxForce() const
    {
        return b2MotorJoint_GetMaxForce(m_b2_joint_id);
    }

    void setMaxForce(float _force)
    {
        b2MotorJoint_SetMaxForce(m_b2_joint_id, _force);
    }

    float getMaxTorque() const
    {
        return b2MotorJoint_GetMaxForce(m_b2_joint_id);
    }

    void setMaxTorque(float _torque)
    {
        b2MotorJoint_SetMaxTorque(m_b2_joint_id, _torque);
    }

    float getCorrectionFactor() const
    {
        return b2MotorJoint_GetCorrectionFactor(m_b2_joint_id);
    }

    void setCorrectionFactor(float _correction_factor)
    {
        b2MotorJoint_SetCorrectionFactor(m_b2_joint_id, _correction_factor);
    }
};

class MouseJoint : public Joint
{
public:
    S2_DEFAULT_COPY_AND_MOVE(MouseJoint)

    explicit MouseJoint(b2JointId _b2_joint_id) :
        Joint(_b2_joint_id)
    {
    }

    SDL_FPoint getTarget() const
    {
        return toSDL(b2MouseJoint_GetTarget(m_b2_joint_id));
    }

    void setTarget(SDL_FPoint _target)
    {
        b2MouseJoint_SetTarget(m_b2_joint_id, toBox2D(_target));
    }

    float getSpringHertz() const
    {
        return b2MouseJoint_GetSpringHertz(m_b2_joint_id);
    }

    void setSpringHertz(float _hertz)
    {
        b2MouseJoint_SetSpringHertz(m_b2_joint_id, _hertz);
    }

    float getSpringDampingRatio() const
    {
        return b2MouseJoint_GetSpringDampingRatio(m_b2_joint_id);
    }

    void setSpringDampingRatio(float _ratio)
    {
        b2MouseJoint_SetSpringDampingRatio(m_b2_joint_id, _ratio);
    }

    float getMaxForce() const
    {
        return b2MouseJoint_GetMaxForce(m_b2_joint_id);
    }

    void setMaxForce(float _max_force)
    {
        b2MouseJoint_SetMaxForce(m_b2_joint_id, _max_force);
    }

};

class PrismaticJoint : public Joint
{
public:
    S2_DEFAULT_COPY_AND_MOVE(PrismaticJoint)

    explicit PrismaticJoint(b2JointId _b2_joint_id) :
        Joint(_b2_joint_id)
    {
    }

    bool isSpringEnabled() const
    {
        return b2PrismaticJoint_IsSpringEnabled(m_b2_joint_id);
    }

    void enableSpring(bool _enable)
    {
        b2PrismaticJoint_EnableSpring(m_b2_joint_id, _enable);
    }

    float getSpringHertz() const
    {
        return b2PrismaticJoint_GetSpringHertz(m_b2_joint_id);
    }

    void setSpringHertz(float _hertz)
    {
        b2PrismaticJoint_SetSpringHertz(m_b2_joint_id, _hertz);
    }

    float getSpringDampingRatio() const
    {
        return b2PrismaticJoint_GetSpringDampingRatio(m_b2_joint_id);
    }

    void setSpringDampingRatio(float _ratio)
    {
        b2PrismaticJoint_SetSpringDampingRatio(m_b2_joint_id, _ratio);
    }

    bool isLimitEnabled() const
    {
        return b2PrismaticJoint_IsLimitEnabled(m_b2_joint_id);
    }

    void enableLimit(bool _enable)
    {
        b2PrismaticJoint_EnableLimit(m_b2_joint_id, _enable);
    }

    float getLowerLimit() const
    {
        return b2PrismaticJoint_GetLowerLimit(m_b2_joint_id);
    }

    float getUpperLimit() const
    {
        return b2PrismaticJoint_GetUpperLimit(m_b2_joint_id);
    }

    void setLimits(float _lower, float _upper)
    {
        b2PrismaticJoint_SetLimits(m_b2_joint_id, _lower, _upper);
    }

    bool isMotorEnabled() const
    {
        return b2PrismaticJoint_IsMotorEnabled(m_b2_joint_id);
    }

    void enableMotor(bool _enable)
    {
        b2PrismaticJoint_EnableMotor(m_b2_joint_id, _enable);
    }

    float getMotorSpeed() const
    {
        return b2PrismaticJoint_GetMotorSpeed(m_b2_joint_id);
    }

    void setMotorSpeed(float _speed)
    {
        b2PrismaticJoint_SetMotorSpeed(m_b2_joint_id, _speed);
    }

    float getMaxMotorForce() const
    {
        return b2PrismaticJoint_GetMaxMotorForce(m_b2_joint_id);
    }

    float getMotorForce() const
    {
        return b2PrismaticJoint_GetMotorForce(m_b2_joint_id);
    }

    void setMaxMotorForce(float _force)
    {
        b2PrismaticJoint_SetMaxMotorForce(m_b2_joint_id, _force);
    }
};

class RevoluteJoint : public Joint
{
public:
    S2_DEFAULT_COPY_AND_MOVE(RevoluteJoint)

    explicit RevoluteJoint(b2JointId _b2_joint_id) :
        Joint(_b2_joint_id)
    {
    }

    bool isSpringEnabled() const
    {
        return b2RevoluteJoint_IsSpringEnabled(m_b2_joint_id);
    }

    void enableSpring(bool _enable)
    {
        b2RevoluteJoint_EnableSpring(m_b2_joint_id, _enable);
    }

    float getSpringHertz() const
    {
        return b2RevoluteJoint_GetSpringHertz(m_b2_joint_id);
    }

    void setSpringHertz(float _hertz)
    {
        b2RevoluteJoint_SetSpringHertz(m_b2_joint_id, _hertz);
    }

    float getSpringDampingRatio() const
    {
        return b2RevoluteJoint_GetSpringDampingRatio(m_b2_joint_id);
    }

    void setSpringDampingRatio(float _ratio)
    {
        b2RevoluteJoint_SetSpringDampingRatio(m_b2_joint_id, _ratio);
    }

    float getAngle() const
    {
        return b2RevoluteJoint_GetAngle(m_b2_joint_id);
    }

    bool isLimitEnabled() const
    {
        return b2RevoluteJoint_IsLimitEnabled(m_b2_joint_id);
    }

    void enableLimit(bool _enable)
    {
        b2RevoluteJoint_EnableLimit(m_b2_joint_id, _enable);
    }

    void setLimits(float _lower, float _upper)
    {
        b2RevoluteJoint_SetLimits(m_b2_joint_id, _lower, _upper);
    }

    float getLowerLimit() const
    {
        return b2RevoluteJoint_GetLowerLimit(m_b2_joint_id);
    }

    float getUpperLimit() const
    {
        return b2RevoluteJoint_GetUpperLimit(m_b2_joint_id);
    }

    bool isMotorEnabled() const
    {
        return b2RevoluteJoint_IsMotorEnabled(m_b2_joint_id);
    }

    void enableMotor(bool _enable)
    {
        b2RevoluteJoint_EnableMotor(m_b2_joint_id, _enable);
    }

    float getMotorSpeed() const
    {
        return b2RevoluteJoint_GetMotorSpeed(m_b2_joint_id);
    }

    void setMotorSpeed(float _speed)
    {
        b2RevoluteJoint_SetMotorSpeed(m_b2_joint_id, _speed);
    }

    float getMotorTorque() const
    {
        return b2RevoluteJoint_GetMotorTorque(m_b2_joint_id);
    }

    float getMaxMotorTorque() const
    {
        return b2RevoluteJoint_GetMaxMotorTorque(m_b2_joint_id);
    }

    void setMaxMotorTorque(float _torque)
    {
        b2RevoluteJoint_SetMaxMotorTorque(m_b2_joint_id, _torque);
    }
};

class WeldJoint : public Joint
{
public:
    S2_DEFAULT_COPY_AND_MOVE(WeldJoint)

    explicit WeldJoint(b2JointId _b2_joint_id) :
        Joint(_b2_joint_id)
    {
    }

    float getReferenceAngle() const
    {
        return b2WeldJoint_GetReferenceAngle(m_b2_joint_id);
    }

    void setReferenceAngle(float _angle_in_radians)
    {
        b2WeldJoint_SetReferenceAngle(m_b2_joint_id, _angle_in_radians);
    }

    float getLinearHertz() const
    {
        return b2WeldJoint_GetLinearHertz(m_b2_joint_id);
    }

    void setLinearHertz(float _hertz)
    {
        b2WeldJoint_SetLinearHertz(m_b2_joint_id, _hertz);
    }

    float getLinearDampingRatio() const
    {
        return b2WeldJoint_GetLinearDampingRatio(m_b2_joint_id);
    }

    void setLinearDampingRatio(float _ratio)
    {
        b2WeldJoint_SetLinearDampingRatio(m_b2_joint_id, _ratio);
    }

    float getAngularHertz() const
    {
        return b2WeldJoint_GetAngularHertz(m_b2_joint_id);
    }

    void setAngularHertz(float _hertz)
    {
        b2WeldJoint_SetAngularHertz(m_b2_joint_id, _hertz);
    }

    float getAngularDampingRatio() const
    {
        return b2WeldJoint_GetAngularDampingRatio(m_b2_joint_id);
    }

    void setAngularDampingRatio(float _ratio)
    {
        b2WeldJoint_SetAngularDampingRatio(m_b2_joint_id, _ratio);
    }
};

class WheelJoint : public Joint
{
public:
    S2_DEFAULT_COPY_AND_MOVE(WheelJoint)

    explicit WheelJoint(b2JointId _b2_joint_id) :
        Joint(_b2_joint_id)
    {
    }

    bool isSpringEnabled() const
    {
        return b2WheelJoint_IsSpringEnabled(m_b2_joint_id);
    }

    void enableSpring(bool _enable)
    {
        b2WheelJoint_EnableSpring(m_b2_joint_id, _enable);
    }

    float getSpringHertz() const
    {
        return b2WheelJoint_GetSpringHertz(m_b2_joint_id);
    }

    void setSpringHertz(float _hertz)
    {
        b2WheelJoint_SetSpringHertz(m_b2_joint_id, _hertz);
    }

    float getSpringDampingRatio() const
    {
        return b2WheelJoint_GetSpringDampingRatio(m_b2_joint_id);
    }

    void setSpringDampingRatio(float _ratio)
    {
        b2WheelJoint_SetSpringDampingRatio(m_b2_joint_id, _ratio);
    }

    bool isLimitEnabled() const
    {
        return b2WheelJoint_IsLimitEnabled(m_b2_joint_id);
    }

    float getLowerLimit() const
    {
        return b2WheelJoint_GetLowerLimit(m_b2_joint_id);
    }

    float getUpperLimit() const
    {
        return b2WheelJoint_GetUpperLimit(m_b2_joint_id);
    }

    void setLimits(float _lower, float _upper)
    {
        b2WheelJoint_SetLimits(m_b2_joint_id, _lower, _upper);
    }

    void enableLimit(bool _enable)
    {
        b2WheelJoint_EnableLimit(m_b2_joint_id, _enable);
    }

    bool isMotorEnabled() const
    {
        return b2WheelJoint_IsMotorEnabled(m_b2_joint_id);
    }

    void enableMotor(bool _enable)
    {
        b2WheelJoint_EnableMotor(m_b2_joint_id, _enable);
    }

    float getMotorSpeed() const
    {
        return b2WheelJoint_GetMotorSpeed(m_b2_joint_id);
    }

    void setMotorSpeed(float _speed)
    {
        b2WheelJoint_SetMotorSpeed(m_b2_joint_id, _speed);
    }

    float getMaxMotorTorque() const
    {
        return b2WheelJoint_GetMaxMotorTorque(m_b2_joint_id);
    }

    void setMaxMotorTorque(float _torque)
    {
        b2WheelJoint_SetMaxMotorTorque(m_b2_joint_id, _torque);
    }

    float getMotorTorque() const
    {
        return b2WheelJoint_GetMotorTorque(m_b2_joint_id);
    }
};

} // namespace Sol2D::World
