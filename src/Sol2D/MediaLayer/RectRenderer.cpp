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

#include <Sol2D/MediaLayer/RectRenderer.h>
#include <Sol2D/MediaLayer/Shader.h>
#include <Sol2D/MediaLayer/SDLException.h>

using namespace Sol2D;

namespace {

constexpr int g_vertex_count = 4;
constexpr int g_index_count = 6;

struct RectMVP
{
    Bool32 use_translate_to_center;
    Bool32 use_rotate;
    uint32_t padding0;
    uint32_t padding1;
    Matrix4x4 mat_translate_to_center;
    Matrix4x4 mat_rotate;
    Matrix4x4 mat_translate_to_final_position;
    Matrix4x4 mat_scale;
    Matrix4x4 mat_projection;
};

struct CircleMVP
{
    Matrix4x4 mat_translate_to_final_position;
    Matrix4x4 mat_scale;
    Matrix4x4 mat_projection;
};

struct RectVertex
{
    FPoint3 position;
    SDL_FPoint tex_coords;
};

struct RectVertexUniform
{
    RectMVP mvp;
};

using CapsuleVertexUniform = RectVertexUniform;

struct RectFragmentUniform
{
    SDL_FColor color;
    SDL_FColor border_color;
    FSize resolution;
    float border_width;
};

struct TextureVertexUniform
{
    SDL_FRect texture_region;
    RectMVP mvp;
};

struct TextureFragmentUniform
{
    Bool32 flip_h;
    Bool32 flip_v;
};

struct CircleVertexUniform
{
    CircleMVP mvp;
};

struct CircleFragmentUniform
{
    SDL_FColor color;
    SDL_FColor border_color;
    float border_width;
    float radius;
};

struct CapsuleFragmentUniform
{
    SDL_FColor color;
    SDL_FColor border_color;
    FSize resolution;
    float border_width;
    float radius;
};

RectMVP getModelViewProjection(
    const FSize & _viewport_size, const SDL_FRect & _rect, const std::optional<Rotation> & _rotation
)
{
    const float scale_factor = 2.0f / _viewport_size.h;
    RectMVP mvp = {};
    if(_rotation.has_value() && !_rotation->isZero())
    {
        mvp.use_rotate = true;
        mvp.mat_rotate = Transform::createRotation(_rotation.value());
    }
    mvp.mat_translate_to_final_position = Transform::createTranslation(
        scale_factor * (_rect.x - (_viewport_size.w - _rect.w) / 2),
        scale_factor * ((_viewport_size.h - _rect.h) / 2 - _rect.y)
    );
    mvp.mat_scale = Transform::createScale(scale_factor * _rect.w, scale_factor * _rect.h);
    mvp.mat_projection = Transform::createOrtho(static_cast<float>(_viewport_size.w) / _viewport_size.h);
    return mvp;
}

CircleMVP getModelViewProjection(const FSize & _viewport_size, const CircleRenderingDataBase & _data)
{
    const float scale_factor = 2.0f / _viewport_size.h;
    CircleMVP mvp = {};
    mvp.mat_translate_to_final_position = Transform::createTranslation(
        scale_factor * (_data.center.x - _viewport_size.w / 2), scale_factor * (_viewport_size.h / 2 - _data.center.y)
    );
    mvp.mat_scale = Transform::createScale(scale_factor * _data.radius * 2);
    mvp.mat_projection = Transform::createOrtho(static_cast<float>(_viewport_size.w) / _viewport_size.h);
    return mvp;
}

SDL_FRect calculateNormalTextureFragmentRect(const FSize & _full_texture_size, const SDL_FRect & _clip_rect)
{
    float ratio_x = 1.0f / _full_texture_size.w;
    float ratio_y = 1.0f / _full_texture_size.h;
    return SDL_FRect {
        .x = _clip_rect.x * ratio_x,
        .y = _clip_rect.y * ratio_y,
        .w = _clip_rect.w * ratio_x,
        .h = _clip_rect.h * ratio_y
    };
}

} // namespace

// TODO: check SDL errors
// FIXME: exceptions in the constructor!
RectRenderer::RectRenderer(const ResourceManager & _resource_manager, SDL_Window * _window, SDL_GPUDevice * _device) :
    m_device(_device),
    m_resource_manager(_resource_manager),
    m_rect_pipeline(createRectPipeline(_window)),
    m_texture_pipeline(createTexturePipeline(_window)),
    m_circle_pipeline(createCirclePipeline(_window)),
    m_capsule_pipeline(createCapsulePipeline(_window)),
    m_vertex_buffer(nullptr),
    m_index_buffer(nullptr),
    m_texture_sampler(nullptr)
{
    {
        SDL_GPUBufferCreateInfo vertex_buffer_create_info = {};
        vertex_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        vertex_buffer_create_info.size = sizeof(RectVertex) * g_vertex_count;
        m_vertex_buffer = SDL_CreateGPUBuffer(_device, &vertex_buffer_create_info);
    }

    {
        SDL_GPUBufferCreateInfo index_buffer_create_info = {};
        index_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
        index_buffer_create_info.size = sizeof(float) * g_index_count;
        m_index_buffer = SDL_CreateGPUBuffer(_device, &index_buffer_create_info);
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
        RectVertex * transfer_data =
            static_cast<RectVertex *>(SDL_MapGPUTransferBuffer(_device, transfer_buffer, false));

        transfer_data[0] = {
            {-0.5f, 0.5f, 0.0f},
            {0.0f, 0.0f}
        };
        transfer_data[1] = {
            {0.5f, 0.5f, 0.0f},
            {1.0f, 0.0f}
        };
        transfer_data[2] = {
            {0.5f, -0.5f, 0.0f},
            {1.0f, 1.0f}
        };
        transfer_data[3] = {
            {-0.5f, -0.5f, 0.0f},
            {0.0f, 1.0f}
        };

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
        SDL_GPUTransferBufferLocation transfer_vertex_buffer_location {.transfer_buffer = transfer_buffer, .offset = 0};
        SDL_GPUBufferRegion transfer_destination {
            .buffer = m_vertex_buffer, .offset = 0, .size = sizeof(RectVertex) * g_vertex_count
        };
        SDL_UploadToGPUBuffer(copy_pass, &transfer_vertex_buffer_location, &transfer_destination, false);
    }

    {
        SDL_GPUTransferBufferLocation transfer_index_buffer_location {
            .transfer_buffer = transfer_buffer, .offset = sizeof(RectVertex) * g_vertex_count
        };
        SDL_GPUBufferRegion transfer_index_buffer_destination {
            .buffer = m_index_buffer, .offset = 0, .size = sizeof(uint16_t) * g_index_count
        };
        SDL_UploadToGPUBuffer(copy_pass, &transfer_index_buffer_location, &transfer_index_buffer_destination, false);
    }

    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(upload_cmd_buffer);
    SDL_ReleaseGPUTransferBuffer(_device, transfer_buffer);

    {
        SDL_GPUSamplerCreateInfo sampler_create_info;
        sampler_create_info.min_filter = SDL_GPU_FILTER_NEAREST;
        sampler_create_info.mag_filter = SDL_GPU_FILTER_NEAREST;
        sampler_create_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
        sampler_create_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_create_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_create_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        m_texture_sampler = SDL_CreateGPUSampler(_device, &sampler_create_info);
    }
}

RectRenderer::~RectRenderer()
{
    if(m_rect_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_rect_pipeline);
    if(m_texture_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_texture_pipeline);
    if(m_circle_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_circle_pipeline);
    if(m_capsule_pipeline)
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_capsule_pipeline);
    if(m_index_buffer)
        SDL_ReleaseGPUBuffer(m_device, m_index_buffer);
    if(m_vertex_buffer)
        SDL_ReleaseGPUBuffer(m_device, m_vertex_buffer);
    if(m_texture_sampler)
        SDL_ReleaseGPUSampler(m_device, m_texture_sampler);
}

SDL_GPUGraphicsPipeline * RectRenderer::createRectPipeline(SDL_Window * _window) const
{
    ShaderLoader loader(m_device, m_resource_manager);
    ShaderPtr vert_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_VERTEX,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Rectangle.vert",
        {.num_samplers = 0, .num_uniform_buffers = 1}
    );
    ShaderPtr frag_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_FRAGMENT,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Rectangle.frag",
        {.num_samplers = 0, .num_uniform_buffers = 1}
    );
    return createPipeline(_window, vert_shader.get(), frag_shader.get());
}

SDL_GPUGraphicsPipeline * RectRenderer::createTexturePipeline(SDL_Window * _window) const
{
    ShaderLoader loader(m_device, m_resource_manager);
    ShaderPtr vert_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_VERTEX,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Texture.vert",
        {.num_samplers = 0, .num_uniform_buffers = 1}
    );
    ShaderPtr frag_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_FRAGMENT,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Texture.frag",
        {.num_samplers = 1, .num_uniform_buffers = 1}
    );
    return createPipeline(_window, vert_shader.get(), frag_shader.get());
}

SDL_GPUGraphicsPipeline * RectRenderer::createCirclePipeline(SDL_Window * _window) const
{
    ShaderLoader loader(m_device, m_resource_manager);
    ShaderPtr vert_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_VERTEX,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Circle.vert",
        {.num_samplers = 0, .num_uniform_buffers = 1}
    );
    ShaderPtr frag_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_FRAGMENT,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Circle.frag",
        {.num_samplers = 0, .num_uniform_buffers = 1}
    );
    return createPipeline(_window, vert_shader.get(), frag_shader.get());
}

SDL_GPUGraphicsPipeline * RectRenderer::createCapsulePipeline(SDL_Window * _window) const
{
    ShaderLoader loader(m_device, m_resource_manager);
    ShaderPtr vert_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_VERTEX,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Rectangle.vert",
        {.num_samplers = 0, .num_uniform_buffers = 1}
    );
    ShaderPtr frag_shader = loader.loadStandard(
        SDL_GPU_SHADERSTAGE_FRAGMENT,
        SDL_GPU_SHADERFORMAT_SPIRV,
        "Capsule.frag",
        {.num_samplers = 0, .num_uniform_buffers = 1}
    );
    return createPipeline(_window, vert_shader.get(), frag_shader.get());
}

SDL_GPUGraphicsPipeline * RectRenderer::createPipeline(
    SDL_Window * _window, SDL_GPUShader * _vert_shader, SDL_GPUShader * _frag_shader
) const
{
    SDL_GPUVertexBufferDescription vertex_buffer_description {
        .slot = 0, .pitch = sizeof(RectVertex), .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX, .instance_step_rate = 0
    };
    SDL_GPUVertexAttribute vertex_attrs[] {
        {.location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, .offset = 0},
        {.location = 1,
         .buffer_slot = 0,
         .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
         .offset = offsetof(RectVertex, tex_coords)}
    };
    SDL_GPUVertexInputState vertex_input_state {
        .vertex_buffer_descriptions = &vertex_buffer_description,
        .num_vertex_buffers = 1,
        .vertex_attributes = vertex_attrs,
        .num_vertex_attributes = 2
    };

    SDL_GPUColorTargetDescription color_target_description = {};
    color_target_description.format = SDL_GetGPUSwapchainTextureFormat(m_device, _window);
    color_target_description.blend_state = {}; // TODO: use common blending in all renderings
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

    SDL_GPUGraphicsPipeline * pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipeline_create_info);
    if(!pipeline)
        throw SDLException("Unable to create GPU graphics pipeline.");
    return pipeline;
}

void RectRenderer::renderRect(const RenderingContext & _ctx, const SolidRectRenderingData & _data) const
{
    RectFragmentUniform frag_uniform {
        .color = _data.color,
        .border_color = _data.color,
        .resolution = FSize(_data.rect.w, _data.rect.h),
        .border_width = .0f
    };
    renderRect(_ctx, _data, &frag_uniform);
}

void RectRenderer::renderRect(const RenderingContext & _ctx, const RectRenderingData & _data) const
{
    RectFragmentUniform frag_uniform {
        .color = _data.color,
        .border_color = _data.border_color,
        .resolution = FSize(_data.rect.w, _data.rect.h),
        .border_width = _data.border_width / _data.rect.w
    };
    renderRect(_ctx, _data, &frag_uniform);
}

void RectRenderer::renderRect(
    const RenderingContext & _ctx, const RectRenderingDataBase & _data, const void * _frag_uniform
) const
{
    SDL_BindGPUGraphicsPipeline(_ctx.render_pass, m_rect_pipeline);
    bindBuffers(_ctx);
    RectVertexUniform vert_uniform {.mvp = getModelViewProjection(_ctx.texture_size, _data.rect, _data.rotation)};
    SDL_PushGPUVertexUniformData(_ctx.command_buffer, 0, &vert_uniform, sizeof(RectVertexUniform));
    SDL_PushGPUFragmentUniformData(_ctx.command_buffer, 0, _frag_uniform, sizeof(RectFragmentUniform));
    SDL_DrawGPUIndexedPrimitives(_ctx.render_pass, g_index_count, 1, 0, 0, 0);
}

void RectRenderer::bindBuffers(const RenderingContext & _ctx) const
{
    SDL_GPUBufferBinding binding = {};
    binding.buffer = m_vertex_buffer;
    SDL_BindGPUVertexBuffers(_ctx.render_pass, 0, &binding, 1);
    binding.buffer = m_index_buffer;
    SDL_BindGPUIndexBuffer(_ctx.render_pass, &binding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
}

void RectRenderer::renderTexture(const RenderingContext & _ctx, const TextureRenderingData & _data) const
{
    SDL_BindGPUGraphicsPipeline(_ctx.render_pass, m_texture_pipeline);
    bindBuffers(_ctx);
    {
        TextureVertexUniform vert_uniform {
            .texture_region = _data.texture_rect.has_value()
                ? calculateNormalTextureFragmentRect(_data.texture.getSize(), _data.texture_rect.value())
                : SDL_FRect {.x = .0f, .y = .0f, .w = 1.0f, .h = 1.0f},
            .mvp = getModelViewProjection(_ctx.texture_size, _data.rect, _data.rotation)
        };
        SDL_PushGPUVertexUniformData(_ctx.command_buffer, 0, &vert_uniform, sizeof(TextureVertexUniform));
    }
    {
        SDL_GPUTextureSamplerBinding sampler_binding {.texture = _data.texture, .sampler = m_texture_sampler};
        SDL_BindGPUFragmentSamplers(_ctx.render_pass, 0, &sampler_binding, 1);
    }
    {
        TextureFragmentUniform frag_uniform = {};
        frag_uniform.flip_h = (_data.flip_mode & SDL_FLIP_HORIZONTAL) == SDL_FLIP_HORIZONTAL;
        frag_uniform.flip_v = (_data.flip_mode & SDL_FLIP_VERTICAL) == SDL_FLIP_VERTICAL;
        SDL_PushGPUFragmentUniformData(_ctx.command_buffer, 0, &frag_uniform, sizeof(TextureFragmentUniform));
    }
    SDL_DrawGPUIndexedPrimitives(_ctx.render_pass, g_index_count, 1, 0, 0, 0);
}

void RectRenderer::renderCircle(const RenderingContext & _ctx, const SolidCircleRenderingData & _data) const
{
    CircleFragmentUniform frag_uniform = {};
    frag_uniform.color = _data.color;
    frag_uniform.radius = _data.radius;
    renderCircle(_ctx, _data, &frag_uniform);
}

void RectRenderer::renderCircle(const RenderingContext & _ctx, const CircleRenderingData & _data) const
{
    CircleFragmentUniform frag_uniform {
        .color = _data.color,
        .border_color = _data.border_color,
        .border_width = _data.border_width / (_data.radius * 2),
        .radius = _data.radius
    };
    renderCircle(_ctx, _data, &frag_uniform);
}

void RectRenderer::renderCircle(
    const RenderingContext & _ctx, const CircleRenderingDataBase & _data, const void * _frag_uniform
) const
{
    SDL_BindGPUGraphicsPipeline(_ctx.render_pass, m_circle_pipeline);
    bindBuffers(_ctx);
    CircleVertexUniform vert_uniform {.mvp = getModelViewProjection(_ctx.texture_size, _data)};
    SDL_PushGPUVertexUniformData(_ctx.command_buffer, 0, &vert_uniform, sizeof(CircleVertexUniform));
    SDL_PushGPUFragmentUniformData(_ctx.command_buffer, 0, _frag_uniform, sizeof(CircleFragmentUniform));
    SDL_DrawGPUIndexedPrimitives(_ctx.render_pass, g_index_count, 1, 0, 0, 0);
}

void RectRenderer::renderCapsule(const RenderingContext & _ctx, const SolidCapsuleRenderingData & _data) const
{
    CapsuleFragmentUniform frag_uniform {
        .color = _data.color,
        .border_color = {},
        .resolution = FSize(_data.capsule.getRect().w, _data.capsule.getRect().h),
        .border_width = .0f,
        .radius = _data.capsule.getRadius()
    };
    renderCapsule(_ctx, _data, &frag_uniform);
}

void RectRenderer::renderCapsule(const RenderingContext & _ctx, const CapsuleRenderingData & _data) const
{
    CapsuleFragmentUniform frag_uniform {
        .color = _data.color,
        .border_color = _data.border_color,
        .resolution = FSize(_data.capsule.getRect().w, _data.capsule.getRect().h),
        .border_width = _data.border_width / _data.capsule.getRect().w,
        .radius = _data.capsule.getRadius()
    };
    renderCapsule(_ctx, _data, &frag_uniform);
}

void RectRenderer::renderCapsule(
    const RenderingContext & _ctx, const CapsuleRenderingDataBase & _data, const void * _frag_uniform
) const
{
    SDL_BindGPUGraphicsPipeline(_ctx.render_pass, m_capsule_pipeline);
    bindBuffers(_ctx);
    CapsuleVertexUniform vert_uniform {
        .mvp = getModelViewProjection(_ctx.texture_size, _data.capsule.getRect(), _data.capsule.getRotation())
    };
    SDL_PushGPUVertexUniformData(_ctx.command_buffer, 0, &vert_uniform, sizeof(CapsuleVertexUniform));
    SDL_PushGPUFragmentUniformData(_ctx.command_buffer, 0, _frag_uniform, sizeof(CapsuleFragmentUniform));
    SDL_DrawGPUIndexedPrimitives(_ctx.render_pass, g_index_count, 1, 0, 0, 0);
}
