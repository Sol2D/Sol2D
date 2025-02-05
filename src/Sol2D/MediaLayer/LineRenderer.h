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

#include <Sol2D/MediaLayer/RenderingContext.h>
#include <Sol2D/ResourceManager.h>
#include <SDL3/SDL_gpu.h>
#include <vector>

namespace Sol2D {

class LineRenderer final
{
    S2_DISABLE_COPY_AND_MOVE(LineRenderer)

public:
    struct ChunkID
    {
        size_t idx;
        size_t cnt;
    };

public:
    LineRenderer(const ResourceManager & _resource_manager, SDL_Window * _window, SDL_GPUDevice * _device);
    ~LineRenderer();
    void endRendering();
    void beginRendering();
    ChunkID enqueueLine(const SDL_FPoint & _point1, const SDL_FPoint & _point2);
    ChunkID enqueueLines(const std::vector<SDL_FPoint> & _points);
    ChunkID enqueuePolyline(const std::vector<SDL_FPoint> & _points, bool _close = false);
    void render(const RenderingContext & _ctx, ChunkID _id, const SDL_FColor & _color) const;

private:
    void reserveSpace(size_t _n);

private:
    SDL_GPUDevice * mp_device;
    SDL_GPUGraphicsPipeline * mp_pipeline;
    SDL_GPUBuffer * mp_vertex_buffer;
    std::vector<SDL_FPoint> m_vertices;
};

} // namespace Sol2D
