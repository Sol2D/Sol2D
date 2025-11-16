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

#include <Sol2D/MediaLayer/MediaLayer.h>
#include <optional>

namespace Sol2D::World {

struct JointDefinition
{
    uint64_t body_a_id = UINT64_MAX;
    uint64_t body_b_id = UINT64_MAX;
    std::optional<b2Transform> local_frame_a;
    std::optional<b2Transform> local_frame_b;
    std::optional<float> force_threshold;
    std::optional<float> torque_threshold;
    std::optional<float> constraint_hertz;
    std::optional<float> constraint_damping_ratio;
    bool is_collide_connected_enabled = false;
};

struct DistanceJointDefinition : JointDefinition
{
    bool is_spring_enabled = false;
    bool is_motor_enabled = false;
    bool is_limit_enabled = false;
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
    std::optional<SDL_FPoint> linear_velocity;
    std::optional<float> max_velocity_force;
    std::optional<float> angular_velocity;
    std::optional<float> max_velocity_torque;
    std::optional<float> linear_hertz;
    std::optional<float> linear_damping_ratio;
    std::optional<float> max_spring_force;
    std::optional<float> angular_hertz;
    std::optional<float> angular_damping_ratio;
    std::optional<float> max_spring_torque;
};

struct PrismaticJointDefinition : JointDefinition
{
    bool is_spring_enabled = false;
    bool is_motor_enabled = false;
    bool is_limit_enabled = false;
    std::optional<float> hertz;
    std::optional<float> damping_ratio;
    std::optional<float> target_translation;
    std::optional<float> lower_translation;
    std::optional<float> upper_translation;
    std::optional<float> max_motor_force;
    std::optional<float> motor_speed;
};

struct WeldJointDefinition : JointDefinition
{
    std::optional<float> linear_hertz;
    std::optional<float> angular_hertz;
    std::optional<float> linear_damping_ratio;
    std::optional<float> angular_damping_ratio;
};

struct WheelJointDefinition : JointDefinition
{
    bool is_spring_enabled = false;
    bool is_motor_enabled = false;
    bool is_limit_enabled = false;
    std::optional<float> hertz;
    std::optional<float> damping_ratio;
    std::optional<float> lower_translation;
    std::optional<float> upper_translation;
    std::optional<float> max_motor_torque;
    std::optional<float> motor_speed;
};

} // namespace Sol2D::World
