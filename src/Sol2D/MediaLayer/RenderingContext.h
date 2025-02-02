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

#include <Sol2D/MediaLayer.h>
#include <glm/glm.hpp>

namespace Sol2D {

struct RenderingContext
{
    SDL_Window * window;
    SDL_GPUDevice * device;
    SDL_GPUCommandBuffer * command_buffer;
    SDL_GPURenderPass * render_pass;
    SDL_GPUTexture * texture;
    USize window_size;
    FSize texture_size;
};

} // namespace Sol2D
