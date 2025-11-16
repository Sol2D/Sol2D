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
public:
    struct ConstraintTuning
    {
        float hertz;
        float damping_ratio;
    };

protected:
    S2_DEFAULT_COPY_AND_MOVE(Joint)

    explicit Joint(b2JointId _b2_joint_id) :
        m_b2_joint_id(_b2_joint_id),
        m_gid(s_sequential_id.getNext())
    {
    }

public:
    virtual ~Joint()
    {
    }

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

    b2Transform getLocalFrameA() const
    {
        return b2Joint_GetLocalFrameA(m_b2_joint_id);
    }

    void setLocalFrameA(const b2Transform & _local_frame)
    {
        return b2Joint_SetLocalFrameA(m_b2_joint_id, _local_frame);
    }

    b2Transform getLocalFrameB() const
    {
        return b2Joint_GetLocalFrameB(m_b2_joint_id);
    }

    void setLocalFrameB(const b2Transform & _local_frame)
    {
        return b2Joint_SetLocalFrameB(m_b2_joint_id, _local_frame);
    }

    bool isCollideConnectedEnabled() const
    {
        return b2Joint_GetCollideConnected(m_b2_joint_id);
    }

    void enableCollideConnected(bool _enable)
    {
        b2Joint_SetCollideConnected(m_b2_joint_id, _enable);
    }

    SDL_FPoint getConstraintForce() const
    {
        return asSDL(b2Joint_GetConstraintForce(m_b2_joint_id));
    }

    float getConstraintTorque() const
    {
        return b2Joint_GetConstraintTorque(m_b2_joint_id);
    }

    float getLinearSeparation() const
    {
        return b2Joint_GetLinearSeparation(m_b2_joint_id);
    }

    float getAngularSeparation() const
    {
        return b2Joint_GetAngularSeparation(m_b2_joint_id);
    }

    void setConstraintTuning(const ConstraintTuning & _tuning)
    {
        b2Joint_SetConstraintTuning(m_b2_joint_id, _tuning.hertz, _tuning.damping_ratio);
    }

    ConstraintTuning getConstraintTuning() const
    {
        ConstraintTuning tuning;
        b2Joint_GetConstraintTuning(m_b2_joint_id, &tuning.hertz, &tuning.damping_ratio);
        return tuning;
    }

    float getForceThreshold() const
    {
        return b2Joint_GetForceThreshold(m_b2_joint_id);
    }

    void setForceThreshold(float _threshold)
    {
        b2Joint_SetForceThreshold(m_b2_joint_id, _threshold);
    }

    float getTorqueThreshold() const
    {
        return b2Joint_GetTorqueThreshold(m_b2_joint_id);
    }

    void setTorqueThreshold(float _threshold)
    {
        b2Joint_SetTorqueThreshold(m_b2_joint_id, _threshold);
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

class DistanceJoint : public Joint
{
public:
    struct SpringForceRange
    {
        float lower_force;
        float upper_force;
    };

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

    bool isLimitEnabled() const
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

    SpringForceRange getSpringForceRange() const
    {
        SpringForceRange range;
        b2DistanceJoint_GetSpringForceRange(m_b2_joint_id, &range.lower_force, &range.upper_force);
        return range;
    }

    void setSpringForceRange(const SpringForceRange & _range)
    {
        b2DistanceJoint_SetSpringForceRange(m_b2_joint_id, _range.lower_force, _range.upper_force);
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

    SDL_FPoint getLinearVelocity() const
    {
        return asSDL(b2MotorJoint_GetLinearVelocity(m_b2_joint_id));
    }

    void setLinearVelocity(const SDL_FPoint & _velocity)
    {
        b2MotorJoint_SetLinearVelocity(m_b2_joint_id, asBox2D(_velocity));
    }

    float getAngularVelocity() const
    {
        return b2MotorJoint_GetAngularVelocity(m_b2_joint_id);
    }

    void setAngularVelocity(float _velocity)
    {
        b2MotorJoint_SetAngularVelocity(m_b2_joint_id, _velocity);
    }

    float getMaxVelocityForce() const
    {
        return b2MotorJoint_GetMaxVelocityForce(m_b2_joint_id);
    }

    void setMaxVelocityForce(float _max_force)
    {
        b2MotorJoint_SetMaxVelocityForce(m_b2_joint_id, _max_force);
    }

    float getMaxVelocityTorque() const
    {
        return b2MotorJoint_GetMaxVelocityTorque(m_b2_joint_id);
    }

    void setMaxVelocityTorque(float _max_torque)
    {
        b2MotorJoint_SetMaxVelocityTorque(m_b2_joint_id, _max_torque);
    }

    float getLinearHertz() const
    {
        return b2MotorJoint_GetLinearHertz(m_b2_joint_id);
    }

    void setLinearHertz(float _hertz)
    {
        b2MotorJoint_SetLinearHertz(m_b2_joint_id, _hertz);
    }

    float getLinearDampingRatio() const
    {
        return b2MotorJoint_GetLinearDampingRatio(m_b2_joint_id);
    }

    void setLinearDampingRatio(float _damping)
    {
        b2MotorJoint_SetLinearDampingRatio(m_b2_joint_id, _damping);
    }

    float getAngularHertz() const
    {
        return b2MotorJoint_GetAngularHertz(m_b2_joint_id);
    }

    void setAngularHertz(float _hertz)
    {
        b2MotorJoint_SetAngularHertz(m_b2_joint_id, _hertz);
    }

    float getAngularDampingRatio() const
    {
        return b2MotorJoint_GetAngularDampingRatio(m_b2_joint_id);
    }

    void setAngularDampingRatio(float _damping)
    {
        b2MotorJoint_SetAngularDampingRatio(m_b2_joint_id, _damping);
    }

    float getMaxSpringForce() const
    {
        return b2MotorJoint_GetMaxSpringForce(m_b2_joint_id);
    }

    void setMaxSpringForce(float _max_force)
    {
        b2MotorJoint_SetMaxSpringForce(m_b2_joint_id, _max_force);
    }

    float getMaxSpringTorque() const
    {
        return b2MotorJoint_GetMaxSpringTorque(m_b2_joint_id);
    }

    void setMaxSpringTorque(float _max_torque)
    {
        b2MotorJoint_SetMaxSpringTorque(m_b2_joint_id, _max_torque);
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

    float getSpeed() const
    {
        return b2PrismaticJoint_GetSpeed(m_b2_joint_id);
    }

    float getTargetTranslation() const
    {
        return b2PrismaticJoint_GetTargetTranslation(m_b2_joint_id);
    }

    void setTargetTranslation(float _translation)
    {
        b2PrismaticJoint_SetTargetTranslation(m_b2_joint_id, _translation);
    }

    float getTranslation() const
    {
        return b2PrismaticJoint_GetTranslation(m_b2_joint_id);
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

    float getTargetAngle() const
    {
        return b2RevoluteJoint_GetTargetAngle(m_b2_joint_id);
    }

    void setTargetAngle(float _angle)
    {
        b2RevoluteJoint_SetTargetAngle(m_b2_joint_id, _angle);
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

    void enableLimit(bool _enable)
    {
        b2WheelJoint_EnableLimit(m_b2_joint_id, _enable);
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
