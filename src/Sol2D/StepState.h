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
#include <chrono>

namespace Sol2D {

enum class MouseClickState
{
    None = 0,
    Started = 1,
    Finished = 2
};

struct MouseClick
{
    MouseClickState state;
    SDL_FPoint start;
    SDL_FPoint finish;
};

struct MouseState
{
    SDL_FPoint position;
    uint32_t buttons;
    MouseClick lb_click;
    MouseClick rb_click;
    MouseClick mb_click;

    bool isLeftButtonDown() const
    {
        return buttons & SDL_BUTTON_LMASK;
    }

    bool isRightButtonDown() const
    {
        return buttons & SDL_BUTTON_RIGHT;
    }

    bool isMiddleButtonDown() const
    {
        return buttons & SDL_BUTTON_MMASK;
    }
};

struct StepState
{
    std::chrono::milliseconds delta_time;
    MouseState mouse_state;
};

} // namespace Sol2D
