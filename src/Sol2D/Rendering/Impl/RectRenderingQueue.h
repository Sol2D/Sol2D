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

#include <Sol2D/Rendering/Impl/RenderingContext.h>
#include <Sol2D/Rendering/RectRenderingData.h>
#include <Sol2D/ResourceManager.h>
#include <queue>

namespace Sol2D::Rendering::Impl {

class RectRenderingQueue final
{
    struct RectRendererTask
    {
        RectRenderingData data;
        SDL_GPUGraphicsPipeline * pipeline;
        SDL_GPUTextureSamplerBinding * sampler_binding;
    };

    enum class PrimitiveKind
    {
        Rect,
        Texture,
        _Count
    };

    S2_DISABLE_COPY_AND_MOVE(RectRenderingQueue)

public:
    RectRenderingQueue(SDL_Window * _window, SDL_GPUDevice * _device, const ResourceManager & _resource_manager);
    ~RectRenderingQueue();
    void enqueueRect(const RectRenderingData & _data);
    void enqueueTexture();

    void render(const RenderingContext & _context);

private:
    SDL_GPUGraphicsPipeline * createRectPipeline() const;
    SDL_GPUGraphicsPipeline * createTexturePipeline() const;
    SDL_GPUGraphicsPipeline * createPipeline(SDL_GPUShader * _vert_shader, SDL_GPUShader * _frag_shader) const;
    void renderTask(
        const RenderingContext & _context,
        SDL_GPURenderPass * _render_pass,
        const RectRendererTask & _task);

private:
    SDL_Window * mp_window;
    SDL_GPUDevice * mp_device;
    const ResourceManager & mr_resource_manager;
    SDL_GPUBuffer * mp_vertex_buffer;
    SDL_GPUBuffer * mp_index_buffer;
    SDL_GPUGraphicsPipeline * mp_pipelines[static_cast<size_t>(PrimitiveKind::_Count)];
    std::queue<RectRendererTask> m_queue;
};

} // namespace Sol2D::Rendering::Impl
