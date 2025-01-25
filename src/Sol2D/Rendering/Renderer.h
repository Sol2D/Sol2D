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

#include <Sol2D/ResourceManager.h>
#include <Sol2D/Rendering/Impl/RectRenderingQueue.h>
#include <Sol2D/Rect.h>
#include <Sol2D/Def.h>
#include <SDL3/SDL_gpu.h>

namespace Sol2D::Rendering {

class Renderer final
{
    S2_DISABLE_COPY_AND_MOVE(Renderer)

public:
    Renderer(const ResourceManager & _resource_manager, SDL_Window * _window, SDL_GPUDevice * _device);
    ~Renderer();
    void renderRect(const RectRenderingData _data);
    void submit();

private:
    Impl::RectRenderingQueue * mp_rect_rendering_queue;
    Impl::RenderingContext m_rendering_context;
};

} // namespace Sol2D::Rendering
