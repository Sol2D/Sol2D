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

#include <Sol2D/Def.h>
#include <SDL3/SDL_rect.h>
#include <cmath>
#include <numbers>

namespace Sol2D {

struct Rotation
{
    enum class AngleUnit
    {
        Radian,
        Degree
    };

    S2_DEFAULT_COPY_AND_MOVE(Rotation)

    Rotation() :
        sine(0.0f),
        cosine(1.0f)
    {
    }

    Rotation(float _sine, float _cosine) :
        sine(_sine),
        cosine(_cosine)
    {
    }

    explicit Rotation(float _angle, AngleUnit _unit = AngleUnit::Radian)
    {
        reset(_angle, _unit);
    }

    bool isZero() const
    {
        return sine == 0.0f;
    }

    void reset()
    {
        sine = 0.0f;
        cosine = 1.0f;
    }

    void reset(float _sine, float _cosine)
    {
        sine = _sine;
        cosine = _cosine;
    }

    void reset(float _angle, AngleUnit _unit = AngleUnit::Radian)
    {
        if(_angle == 0.0f)
        {
            sine = 0.0f;
            cosine = 1.0f;
        }
        else if(_unit == AngleUnit::Radian)
        {
            sine = std::sin(_angle);
            cosine = std::cos(_angle);
        }
        else
        {
            const float rad = (_angle * std::numbers::pi_v<float>) / 180.0f;
            sine = std::sin(rad);
            cosine = std::cos(rad);
        }
    }

    SDL_FPoint rotateVectorCCW(const SDL_FPoint & _vector) const
    {
        return
        {
            .x = _vector.x * cosine - _vector.y * sine,
            .y = _vector.x * sine + _vector.y * cosine,
        };
    }

    SDL_FPoint rotateVectorCW(const SDL_FPoint & _vector) const
    {
        return
        {
            .x = _vector.x * cosine + _vector.y * sine,
            .y = _vector.y * cosine - _vector.x * sine,
        };
    }

    float sine;
    float cosine;
};

} // namespace Sol2D
