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

#include <Sol2D/Rendering/Renderer.h>
#include <Sol2D/Rendering/SDLException.h>

using namespace Sol2D;
using namespace Sol2D::Rendering;
using namespace Sol2D::Rendering::Impl;

Renderer::Renderer(const ResourceManager & _resource_manager, SDL_Window * _window, SDL_GPUDevice * _device) :
    mp_rect_rendering_queue(new RectRenderingQueue(_window, _device, _resource_manager)),
    m_rendering_context
    {
        .window = _window,
        .device = _device,
        .command_buffer = nullptr,
        .texture = nullptr,
        .texture_size = glm::uvec2(0u)
    }
{
}

Renderer::~Renderer()
{
    delete mp_rect_rendering_queue;
}

void Renderer::renderRect(const RectRenderingData _data)
{
    mp_rect_rendering_queue->enqueueRect(_data);
}

void Renderer::submit()
{
    if(m_rendering_context.command_buffer)
    {
        throw std::runtime_error("Cannot begin a new rendering step until the previous one is completed");
    }
    m_rendering_context.command_buffer = SDL_AcquireGPUCommandBuffer(m_rendering_context.device);
    if(!SDL_WaitAndAcquireGPUSwapchainTexture(
            m_rendering_context.command_buffer,
            m_rendering_context.window,
            &m_rendering_context.texture,
            &m_rendering_context.texture_size.x,
            &m_rendering_context.texture_size.y) ||
        !m_rendering_context.texture)
    {
        throw SDLException("Unable to acquire a swapchain texture");
    }

    mp_rect_rendering_queue->render(m_rendering_context);

    m_rendering_context.command_buffer = nullptr;
    m_rendering_context.texture = nullptr;
}
