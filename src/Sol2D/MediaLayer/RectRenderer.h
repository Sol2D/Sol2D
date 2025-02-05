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

#include <Sol2D/MediaLayer/RenderingData.h>
#include <Sol2D/MediaLayer/RenderingContext.h>
#include <Sol2D/ResourceManager.h>

namespace Sol2D {

class RectRenderer final
{
    S2_DISABLE_COPY_AND_MOVE(RectRenderer)

public:
    RectRenderer(const ResourceManager & _resource_manager, SDL_Window * _window, SDL_GPUDevice * _device);
    ~RectRenderer();
    void renderRect(const RenderingContext & _ctx, const SolidRectRenderingData & _data) const;
    void renderRect(const RenderingContext & _ctx, const RectRenderingData & _data) const;
    void renderTexture(const RenderingContext & _ctx, const TextureRenderingData & _data) const;
    void renderCircle(const RenderingContext & _ctx, const SolidCircleRenderingData & _data) const;
    void renderCircle(const RenderingContext & _ctx, const CircleRenderingData & _data) const;

private:
    SDL_GPUGraphicsPipeline * createRectPipeline(SDL_Window * _window) const;
    SDL_GPUGraphicsPipeline * createCirclePipeline(SDL_Window * _window) const;
    SDL_GPUGraphicsPipeline * createTexturePipeline(SDL_Window * _window) const;
    SDL_GPUGraphicsPipeline * createPipeline(
        SDL_Window * _window,
        SDL_GPUShader * _vert_shader,
        SDL_GPUShader * _frag_shader) const;
    void renderRect(
        const RenderingContext & _ctx,
        const RectRenderingDataBase & _data,
        const void * _frag_uniform) const;
    void renderCircle(
        const RenderingContext & _ctx,
        const CircleRenderingDataBase & _data,
        const void * _frag_uniform) const;
    void bindBuffers(const RenderingContext & _ctx) const;

private:
    SDL_GPUDevice * mp_device;
    const ResourceManager & mr_resource_manager;
    SDL_GPUGraphicsPipeline * mp_rect_pipeline;
    SDL_GPUGraphicsPipeline * mp_texture_pipeline;
    SDL_GPUGraphicsPipeline * mp_circle_pipeline;
    SDL_GPUBuffer * mp_vertex_buffer;
    SDL_GPUBuffer * mp_index_buffer;
    SDL_GPUSampler * mp_texture_sampler;
};

} // namespace Sol2D
