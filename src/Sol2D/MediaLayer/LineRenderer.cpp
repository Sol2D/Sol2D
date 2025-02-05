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

#include <Sol2D/MediaLayer/LineRenderer.h>
#include <Sol2D/MediaLayer/Shader.h>
#include <Sol2D/MediaLayer/SDLException.h>
#include <Sol2D/Exception.h>

using namespace Sol2D;

LineRenderer::LineRenderer(const ResourceManager & _resource_manager, SDL_Window * _window, SDL_GPUDevice * _device) :
    mp_device(_device),
    mp_pipeline(nullptr),
    mp_vertex_buffer(nullptr)
{
    ShaderLoader loader(mp_device, _resource_manager);
    ShaderPtr vert_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_VERTEX,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Polyline.vert",
        {
            .num_samplers = 0,
            .num_uniform_buffers = 1
        });
    ShaderPtr frag_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_FRAGMENT,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Polyline.frag",
        {
            .num_samplers = 0,
            .num_uniform_buffers = 1
        });

    SDL_GPUColorTargetDescription color_target_description = {};
    color_target_description.format = SDL_GetGPUSwapchainTextureFormat(mp_device, _window);
    color_target_description.blend_state = {}; // TODO: use common blending in all renderings
    color_target_description.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    color_target_description.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    color_target_description.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    color_target_description.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    color_target_description.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    color_target_description.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    color_target_description.blend_state.enable_blend = true;

    SDL_GPUVertexAttribute vertex_attributes[]
    {
        SDL_GPUVertexAttribute
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = 0
        }
    };
    SDL_GPUVertexBufferDescription vertex_buffer_description
    {
        .slot = 0,
        .pitch = sizeof(SDL_FPoint),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0
    };
    SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.vertex_shader = vert_shader.get();
    pipeline_create_info.fragment_shader = frag_shader.get();
    pipeline_create_info.primitive_type = SDL_GPU_PRIMITIVETYPE_LINELIST;
    pipeline_create_info.vertex_input_state = {};
    pipeline_create_info.vertex_input_state.vertex_attributes = vertex_attributes;
    pipeline_create_info.vertex_input_state.num_vertex_attributes = 1;
    pipeline_create_info.vertex_input_state.num_vertex_buffers = 1;
    pipeline_create_info.vertex_input_state.vertex_buffer_descriptions = &vertex_buffer_description;
    pipeline_create_info.rasterizer_state = {};
    pipeline_create_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_LINE;
    pipeline_create_info.target_info = {};
    pipeline_create_info.target_info.color_target_descriptions = &color_target_description;
    pipeline_create_info.target_info.num_color_targets = 1;

    mp_pipeline = SDL_CreateGPUGraphicsPipeline(mp_device, &pipeline_create_info);
    if(!mp_pipeline)
        throw SDLException("Unable to create GPU graphics pipeline");
}

LineRenderer::~LineRenderer()
{
    if(mp_vertex_buffer)
        SDL_ReleaseGPUBuffer(mp_device, mp_vertex_buffer);
    if(mp_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(mp_device, mp_pipeline);
}

void LineRenderer::reserveSpace(size_t _n)
{
    static const size_t increment_size = 256;
    const size_t desired_capacity = m_vertices.size() + std::max(increment_size, _n);
    if(m_vertices.capacity() < desired_capacity)
        m_vertices.reserve(desired_capacity);
}

void LineRenderer::beginRendering()
{
    if(mp_vertex_buffer)
        throw InvalidOperationException("There is already an active rendering");

    if(m_vertices.empty())
        return;

    SDL_GPUBufferCreateInfo vertex_buffer_create_info = {};
    vertex_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertex_buffer_create_info.size = sizeof(SDL_FPoint) * m_vertices.size();
    mp_vertex_buffer = SDL_CreateGPUBuffer(mp_device, &vertex_buffer_create_info);

    SDL_GPUCommandBuffer * upload_cmd_buffer = SDL_AcquireGPUCommandBuffer(mp_device);
    SDL_GPUCopyPass * copy_pass;
    SDL_GPUTransferBuffer * transfer_buffer;

    {
        copy_pass = SDL_BeginGPUCopyPass(upload_cmd_buffer);
        if(!copy_pass)
            throw SDLException("Unable to create a copy pass for line rendering");
        SDL_GPUTransferBufferCreateInfo transfer_buffer_create_info = {};
        transfer_buffer_create_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transfer_buffer_create_info.size = vertex_buffer_create_info.size;
        transfer_buffer = SDL_CreateGPUTransferBuffer(mp_device, &transfer_buffer_create_info);
        if(!transfer_buffer)
            throw SDLException("Unable to create a transfer buffer for line rendering");
        void * data = SDL_MapGPUTransferBuffer(mp_device, transfer_buffer, false);
        if(!data)
            throw SDLException("Unable to map a vertex buffer for line rendering");
        memcpy(data, m_vertices.data(), vertex_buffer_create_info.size);
        SDL_UnmapGPUTransferBuffer(mp_device, transfer_buffer);
    }

    {
        SDL_GPUTransferBufferLocation transfer_buffer_location
        {
            .transfer_buffer = transfer_buffer,
            .offset = 0
        };
        SDL_GPUBufferRegion transfer_destination
        {
            .buffer = mp_vertex_buffer,
            .offset = 0,
            .size = vertex_buffer_create_info.size
        };
        SDL_UploadToGPUBuffer(
            copy_pass,
            &transfer_buffer_location,
            &transfer_destination,
            false);
    }

    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(upload_cmd_buffer);
    SDL_ReleaseGPUTransferBuffer(mp_device, transfer_buffer);
}

void LineRenderer::endRendering()
{
    if(mp_vertex_buffer)
    {
        SDL_ReleaseGPUBuffer(mp_device, mp_vertex_buffer);
        mp_vertex_buffer = nullptr;
    }
    m_vertices.clear();
}

LineRenderer::ChunkID LineRenderer::enqueueLine(const SDL_FPoint & _point1, const SDL_FPoint & _point2)
{
    ChunkID id
    {
        .idx = m_vertices.size(),
        .cnt = 2
    };
    reserveSpace(id.cnt);
    m_vertices.push_back(_point1);
    m_vertices.push_back(_point2);
    return id;
}

LineRenderer::ChunkID LineRenderer::enqueueLines(const std::vector<SDL_FPoint> & _points)
{
    if(_points.size() < 2)
        throw InvalidOperationException("A line must contain at least 2 points");

    ChunkID id
    {
        .idx = m_vertices.size(),
        .cnt = _points.size()
    };
    reserveSpace(id.cnt);
    m_vertices.insert(m_vertices.end(), _points.begin(), _points.end());
    return id;
}

LineRenderer::ChunkID LineRenderer::enqueuePolyline(const std::vector<SDL_FPoint> & _points, bool _close)
{
    ChunkID id
    {
        .idx = m_vertices.size(),
        .cnt = _points.size() * 2
    };

    if(_close)
    {
        if(_points.size() < 3)
            throw InvalidOperationException("A closed polyline must contain at least 3 points");
    }
    else
    {
        if(_points.size() < 2)
            throw InvalidOperationException("A polyline must contain at least 2 points");
        id.cnt -= 2;
    }

    reserveSpace(id.cnt);

    for(size_t i = 1; i < _points.size(); ++i)
    {
        m_vertices.push_back(_points[i - 1]);
        m_vertices.push_back(_points[i]);
    }
    if(_close)
    {
        m_vertices.push_back(_points.front());
        m_vertices.push_back(_points.back());
    }
    return id;
}

void LineRenderer::render(const RenderingContext & _ctx, ChunkID _id, const SDL_FColor & _color) const
{
    if(!mp_vertex_buffer)
        throw InvalidOperationException("There is no active rendering");

    SDL_BindGPUGraphicsPipeline(_ctx.render_pass, mp_pipeline);
    SDL_PushGPUVertexUniformData(_ctx.command_buffer, 0, &_ctx.texture_size, sizeof(FSize));
    SDL_PushGPUFragmentUniformData(_ctx.command_buffer, 0, &_color, sizeof(SDL_FColor));
    SDL_GPUBufferBinding binding
    {
        .buffer = mp_vertex_buffer,
        .offset = 0
    };
    SDL_BindGPUVertexBuffers(_ctx.render_pass, 0, &binding, 1);
    SDL_DrawGPUPrimitives(_ctx.render_pass, _id.cnt, 1, _id.idx, 0);
}
