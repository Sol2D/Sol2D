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

#include <Sol2D/Rendering/Impl/RectRenderingQueue.h>
#include <Sol2D/Rendering/Impl/Shader.h>
#include <Sol2D/Rendering/SDLException.h>
#include <glm/gtc/type_ptr.hpp>

using namespace Sol2D;
using namespace Sol2D::Rendering;
using namespace Sol2D::Rendering::Impl;

namespace {

constexpr int g_vertex_count = 4;
constexpr int g_index_count = 6;

struct RectVertex
{
    glm::vec3 position;
    glm::vec2 tex_coords;
};

glm::mat4x4 calculateProjectionMatrix(const glm::uvec2 & _texture_size, const RectRenderingData & _data)
{
    const float ratio = static_cast<float>(_texture_size.x) / _texture_size.y;
    const float scale_factor = 2.0f / _texture_size.y;

    glm::mat4x4 m = glm::translate(
        glm::mat4x4(1.0f),
        {
            scale_factor * (_data.rect.x - (_texture_size.x - _data.rect.w) / 2),
            scale_factor * ((_texture_size.y - _data.rect.h) / 2 - _data.rect.y),
            0.0f
        });

    if(_data.rotation.has_value() && !_data.rotation->isZero())
    {
        glm::mat4x4 r(
            {_data.rotation->cosine, -_data.rotation->sine, .0f, .0f},
            {_data.rotation->sine, _data.rotation->cosine, .0f, .0f},
            {.0f, .0f, 1.0f, .0f},
            {.0f, .0f, .0f, 1.0f});
        m = m * r;
    }

    glm::mat4x4 p = glm::ortho(-ratio, ratio, -1.0f, 1.0f);

    m = p * m;

    return glm::scale(m, {scale_factor * _data.rect.w, scale_factor * _data.rect.h, 1.0f});
}

} // namespace

RectRenderingQueue::RectRenderingQueue(
    SDL_Window * _window,
    SDL_GPUDevice * _device,
    const ResourceManager & _resource_manager
) :
    mp_window(_window),
    mp_device(_device),
    mr_resource_manager(_resource_manager),
    mp_vertex_buffer(nullptr),
    mp_index_buffer(nullptr)
{
    mp_pipelines[static_cast<size_t>(PrimitiveKind::Rect)] = createRectPipeline();
    mp_pipelines[static_cast<size_t>(PrimitiveKind::Texture)] = createTexturePipeline();

    {
        SDL_GPUBufferCreateInfo vertex_buffer_create_info = {};
        vertex_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        vertex_buffer_create_info.size = sizeof(RectVertex) * g_vertex_count;
        mp_vertex_buffer = SDL_CreateGPUBuffer(_device, &vertex_buffer_create_info);
    }

    {
        SDL_GPUBufferCreateInfo index_buffer_create_info = {};
        index_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
        index_buffer_create_info.size = sizeof(float) * g_index_count;
        mp_index_buffer = SDL_CreateGPUBuffer(_device, &index_buffer_create_info);
    }

    SDL_GPUCommandBuffer * upload_cmd_buffer = SDL_AcquireGPUCommandBuffer(_device);
    SDL_GPUCopyPass * copy_pass = SDL_BeginGPUCopyPass(upload_cmd_buffer);

    SDL_GPUTransferBuffer * transfer_buffer;
    {
        SDL_GPUTransferBufferCreateInfo transfer_buffer_create_info = {};
        transfer_buffer_create_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transfer_buffer_create_info.size = sizeof(RectVertex) * g_vertex_count + sizeof(float) * g_index_count;
        transfer_buffer = SDL_CreateGPUTransferBuffer(_device, &transfer_buffer_create_info);
    }

    {
        RectVertex * transfer_data = static_cast<RectVertex *>(
            SDL_MapGPUTransferBuffer(_device, transfer_buffer, false));

        transfer_data[0] = { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } };
        transfer_data[1] = { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } };
        transfer_data[2] = { {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } };
        transfer_data[3] = { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } };

        uint16_t * index_data = reinterpret_cast<uint16_t *>(&transfer_data[4]);
        index_data[0] = 0;
        index_data[1] = 1;
        index_data[2] = 2;
        index_data[3] = 0;
        index_data[4] = 2;
        index_data[5] = 3;

        SDL_UnmapGPUTransferBuffer(_device, transfer_buffer);
    }

    {
        SDL_GPUTransferBufferLocation transfer_vertex_buffer_location
        {
            .transfer_buffer = transfer_buffer,
            .offset = 0
        };
        SDL_GPUBufferRegion transfer_destination
        {
            .buffer = mp_vertex_buffer,
            .offset = 0,
            .size = sizeof(RectVertex) * g_vertex_count
        };
        SDL_UploadToGPUBuffer(
            copy_pass,
            &transfer_vertex_buffer_location,
            &transfer_destination,
            false);
    }

    {
        SDL_GPUTransferBufferLocation transfer_index_buffer_location
        {
            .transfer_buffer = transfer_buffer,
            .offset = sizeof(RectVertex) * g_vertex_count
        };
        SDL_GPUBufferRegion transfer_index_buffer_destination
        {
            .buffer = mp_index_buffer,
            .offset = 0,
            .size = sizeof(uint16_t) * g_index_count
        };
        SDL_UploadToGPUBuffer(
            copy_pass,
            &transfer_index_buffer_location,
            &transfer_index_buffer_destination,
            false);
    }

    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(upload_cmd_buffer);
    SDL_ReleaseGPUTransferBuffer(_device, transfer_buffer);
}

RectRenderingQueue::~RectRenderingQueue()
{
    for(size_t i = 0; i < static_cast<size_t>(PrimitiveKind::_Count); ++i)
    {
        if(mp_pipelines[i])
        {
            SDL_ReleaseGPUGraphicsPipeline(mp_device, mp_pipelines[i]);
            mp_pipelines[i] = nullptr;
        }
    }
    if(mp_index_buffer)
    {
        SDL_ReleaseGPUBuffer(mp_device, mp_index_buffer);
        mp_index_buffer = nullptr;
    }
    if(mp_vertex_buffer)
    {
        SDL_ReleaseGPUBuffer(mp_device, mp_vertex_buffer);
        mp_vertex_buffer = nullptr;
    }
}

SDL_GPUGraphicsPipeline * RectRenderingQueue::createRectPipeline() const
{
    ShaderLoader loader(mp_device, mr_resource_manager);
    ShaderPtr vert_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_VERTEX,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Rectangle.vert",
        {
             .num_samplers = 0,
             .num_uniform_buffers = 1
        });
    ShaderPtr frag_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_VERTEX,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Rectangle.frag",
        {
            .num_samplers = 0,
            .num_uniform_buffers = 1
        });
    return createPipeline(vert_shader.get(), frag_shader.get());
}

SDL_GPUGraphicsPipeline * RectRenderingQueue::createTexturePipeline() const
{
    ShaderLoader loader(mp_device, mr_resource_manager);
    ShaderPtr vert_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_VERTEX,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Texture.vert",
        {
             .num_samplers = 0,
             .num_uniform_buffers = 1
        });
    ShaderPtr frag_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_VERTEX,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Texture.frag",
        {
            .num_samplers = 1,
            .num_uniform_buffers = 1
        });
    return createPipeline(vert_shader.get(), frag_shader.get());
}

SDL_GPUGraphicsPipeline * RectRenderingQueue::createPipeline(
    SDL_GPUShader * _vert_shader,
    SDL_GPUShader * _frag_shader) const
{
    SDL_GPUVertexBufferDescription vertex_buffer_description
    {
        .slot = 0,
        .pitch = sizeof(RectVertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0
    };
    SDL_GPUVertexAttribute positoin_vertex_attrs[]
    {
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = 0
        },
        {
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = offsetof(RectVertex, tex_coords)
        }
    };
    SDL_GPUVertexInputState vertex_input_state
    {
        .vertex_buffer_descriptions = &vertex_buffer_description,
        .num_vertex_buffers = 1,
        .vertex_attributes = positoin_vertex_attrs,
        .num_vertex_attributes = 2
    };

    SDL_GPUColorTargetDescription color_target_description = {};
    color_target_description.format = SDL_GetGPUSwapchainTextureFormat(mp_device, mp_window);
    color_target_description.blend_state = {};
    color_target_description.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    color_target_description.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    color_target_description.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    color_target_description.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    color_target_description.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    color_target_description.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    color_target_description.blend_state.enable_blend = true;

    SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.vertex_shader = _vert_shader;
    pipeline_create_info.fragment_shader = _frag_shader;
    pipeline_create_info.vertex_input_state = vertex_input_state;
    pipeline_create_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeline_create_info.rasterizer_state = {};
    pipeline_create_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipeline_create_info.target_info = {};
    pipeline_create_info.target_info.color_target_descriptions = &color_target_description;
    pipeline_create_info.target_info.num_color_targets = 1;

    SDL_GPUGraphicsPipeline * pipeline = SDL_CreateGPUGraphicsPipeline(mp_device, &pipeline_create_info);
    if(!pipeline)
        throw SDLException("Unable to create GPU graphics pipeline");
    return pipeline;
}

void RectRenderingQueue::render(const RenderingContext & _context)
{
    SDL_GPURenderPass * render_pass;
    {
        SDL_GPUColorTargetInfo color_target_info = {};
        color_target_info.texture = _context.texture;
        color_target_info.clear_color = { .2f, .2f, .2f, 1.0f };
        color_target_info.load_op = SDL_GPU_LOADOP_LOAD;
        color_target_info.store_op = SDL_GPU_STOREOP_STORE;
        render_pass = SDL_BeginGPURenderPass(_context.command_buffer, &color_target_info, 1, nullptr);
    }
    while(!m_queue.empty())
    {
        renderTask(_context, render_pass, m_queue.front());
        m_queue.pop();
    }
    SDL_DrawGPUIndexedPrimitives(render_pass, g_index_count, 1, 0, 0, 0);
    SDL_EndGPURenderPass(render_pass);
}

void RectRenderingQueue::renderTask(
    const RenderingContext & _context,
    SDL_GPURenderPass * _render_pass,
    const RectRendererTask & _task)
{
    // TODO: color to Uniforms
    SDL_BindGPUGraphicsPipeline(_render_pass, _task.pipeline);
    {
        SDL_GPUBufferBinding vertex_buffer_binding
        {
            .buffer = mp_vertex_buffer,
            .offset = 0
        };
        SDL_BindGPUVertexBuffers(_render_pass, 0, &vertex_buffer_binding, 1);
    }
    {
        SDL_GPUBufferBinding index_buffer_binding
        {
            .buffer = mp_index_buffer,
            .offset = 0
        };
        SDL_BindGPUIndexBuffer(_render_pass, &index_buffer_binding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
    }
    glm::mat4x4 projection_matrix = calculateProjectionMatrix(_context.texture_size, _task.data);
    SDL_PushGPUVertexUniformData(
        _context.command_buffer,
        0,
        glm::value_ptr(projection_matrix),
        sizeof(glm::mat4x4));
    if(_task.sampler_binding) {
        SDL_BindGPUFragmentSamplers(_render_pass, 0, _task.sampler_binding, 1);
    }
    SDL_DrawGPUIndexedPrimitives(_render_pass, g_index_count, 1, 0, 0, 0);
}

void RectRenderingQueue::enqueueRect(const RectRenderingData & _data)
{
    m_queue.emplace(RectRendererTask {
        .data = _data,
        .pipeline = mp_pipelines[static_cast<size_t>(PrimitiveKind::Rect)],
        .sampler_binding = nullptr
    });
}

void RectRenderingQueue::enqueueTexture()
{

}
