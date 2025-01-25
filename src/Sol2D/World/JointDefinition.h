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

#include <Sol2D/Rect.h>
#include <optional>
#include <cstdint>

namespace Sol2D::World {

struct JointDefinition
{
    JointDefinition() :
        body_a_id(UINT64_MAX),
        body_b_id(UINT64_MAX),
        is_collide_connected_enabled(false)
    {
    }

    uint64_t body_a_id;
    uint64_t body_b_id;
    bool is_collide_connected_enabled;
};

struct DistanceJointDefenition : JointDefinition
{
    DistanceJointDefenition() :
        is_spring_enabled(false),
        is_motor_enabled(false),
        is_limit_enabled(false)
    {
    }

    bool is_spring_enabled;
    bool is_motor_enabled;
    bool is_limit_enabled;
    std::optional<Point> local_anchor_a;
    std::optional<Point> local_anchor_b;
    std::optional<float> min_length;
    std::optional<float> max_length;
    std::optional<float> hertz;
    std::optional<float> damping_ratio;
    std::optional<float> max_motor_force;
    std::optional<float> motor_speed;
    std::optional<float> length;
};

struct MotorJointDefinition : JointDefinition
{
    std::optional<Point> linear_offset;
    std::optional<float> angular_offset;
    std::optional<float> max_force;
    std::optional<float> max_torque;
    std::optional<float> correction_factor;
};

struct MouseJointDefinition : JointDefinition
{
    std::optional<Point> target;
    std::optional<float> hertz;
    std::optional<float> damping_ratio;
    std::optional<float> max_force;
};

struct PrismaticJointDefinition : JointDefinition
{
    PrismaticJointDefinition() :
        is_spring_enabled(false),
        is_motor_enabled(false),
        is_limit_enabled(false)
    {
    }

    bool is_spring_enabled;
    bool is_motor_enabled;
    bool is_limit_enabled;
    std::optional<Point> local_anchor_a;
    std::optional<Point> local_anchor_b;
    std::optional<Point> local_axis_a;
    std::optional<float> reference_angle;
    std::optional<float> hertz;
    std::optional<float> damping_ratio;
    std::optional<float> lower_translation;
    std::optional<float> upper_translation;
    std::optional<float> max_motor_force;
    std::optional<float> motor_speed;
};

struct WeldJointDefinition : JointDefinition
{
    std::optional<Point> local_anchor_a;
    std::optional<Point> local_anchor_b;
    std::optional<float> reference_angle;
    std::optional<float> linear_hertz;
    std::optional<float> angular_hertz;
    std::optional<float> linear_damping_ratio;
    std::optional<float> angular_damping_ratio;
};

struct WheelJointDefinition : JointDefinition
{
    WheelJointDefinition() :
        is_spring_enabled(false),
        is_motor_enabled(false),
        is_limit_enabled(false)
    {
    }

    bool is_spring_enabled;
    bool is_motor_enabled;
    bool is_limit_enabled;
    std::optional<Point> local_anchor_a;
    std::optional<Point> local_anchor_b;
    std::optional<Point> local_axis_a;
    std::optional<float> hertz;
    std::optional<float> damping_ratio;
    std::optional<float> lower_translation;
    std::optional<float> upper_translation;
    std::optional<float> max_motor_torque;
    std::optional<float> motor_speed;
};

} // namespace Sol2D::World
