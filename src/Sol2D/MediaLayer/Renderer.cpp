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

#include <Sol2D/MediaLayer/Renderer.h>
#include <Sol2D/MediaLayer/Primitive.h>
#include <Sol2D/MediaLayer/SDLException.h>

using namespace Sol2D;

Renderer::Renderer(const ResourceManager & _resource_manager, SDL_Window * _window, SDL_GPUDevice * _device) :
    mr_resource_manager(_resource_manager),
    m_rendering_context
    {
        .window = _window,
        .device = _device,
        .command_buffer = nullptr,
        .render_pass = nullptr,
        .texture = nullptr,
        .window_size = USize(),
        .texture_size = FSize()
    },
    mp_swapchain_texture(nullptr),
    m_rect_renderer(_resource_manager, _window, _device),
    m_line_renderer(_resource_manager, _window, _device)
{
}

Renderer::~Renderer()
{
    while(!m_queue.empty())
    {
        delete m_queue.front();
        m_queue.pop();
    }
}

const FSize Renderer::getOutputSize() const
{
    int w, h;
    SDL_GetWindowSize(m_rendering_context.window, &w, &h);
    return FSize(w, h);
}

Texture Renderer::createTexture(SDL_Surface & _surface, const char * _name) const
{
    SDL_Surface * surface = &_surface;
    if(_surface.format != SDL_PIXELFORMAT_RGBA32)
        surface = SDL_ConvertSurface(&_surface, SDL_PIXELFORMAT_RGBA32);

    SDL_GPUTexture * texture;
    {
        SDL_GPUTextureCreateInfo tex_create_info = {};
        tex_create_info.type = SDL_GPU_TEXTURETYPE_2D;
        tex_create_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        tex_create_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        tex_create_info.width = static_cast<uint32_t>(surface->w);
        tex_create_info.height = static_cast<uint32_t>(surface->h);
        tex_create_info.layer_count_or_depth = 1;
        tex_create_info.num_levels = 1;
        texture = SDL_CreateGPUTexture(m_rendering_context.device, &tex_create_info);
    }

    if(!texture)
    {
        if(surface != &_surface) SDL_DestroySurface(surface);
        throw SDLException("Unable to create texture for transform from surface");
    }

    if(_name)
        SDL_SetGPUTextureName(m_rendering_context.device, texture, _name);

    SDL_GPUTransferBuffer * tex_transfer_buffer;
    {
        const SDL_PixelFormatDetails * surface_format = SDL_GetPixelFormatDetails(surface->format);
        SDL_GPUTransferBufferCreateInfo tex_transfer_buffer_create_info = {};
        tex_transfer_buffer_create_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        tex_transfer_buffer_create_info.size = static_cast<uint32_t>(
            surface->w * surface->h * surface_format->bytes_per_pixel);
        tex_transfer_buffer = SDL_CreateGPUTransferBuffer(m_rendering_context.device, &tex_transfer_buffer_create_info);
        void * tex_transfer_ptr = SDL_MapGPUTransferBuffer(
            m_rendering_context.device,
            tex_transfer_buffer,
            false);
        memcpy(tex_transfer_ptr, surface->pixels, tex_transfer_buffer_create_info.size);
        SDL_UnmapGPUTransferBuffer(m_rendering_context.device, tex_transfer_buffer);
    }

    {
        SDL_GPUCommandBuffer * upload_cmd = SDL_AcquireGPUCommandBuffer(m_rendering_context.device);
        SDL_GPUCopyPass * upload_pass = SDL_BeginGPUCopyPass(upload_cmd);
        SDL_GPUTextureTransferInfo texture_transfer_src = {};
        texture_transfer_src.transfer_buffer = tex_transfer_buffer;
        texture_transfer_src.offset = 0;
        SDL_GPUTextureRegion texture_transfer_dest = {};
        texture_transfer_dest.texture = texture;
        texture_transfer_dest.w = static_cast<uint32_t>(surface->w);
        texture_transfer_dest.h = static_cast<uint32_t>(surface->h);
        texture_transfer_dest.d = 1;
        SDL_UploadToGPUTexture(upload_pass, &texture_transfer_src, &texture_transfer_dest, false);
        SDL_EndGPUCopyPass(upload_pass);
        SDL_SubmitGPUCommandBuffer(upload_cmd);
    }

    SDL_ReleaseGPUTransferBuffer(m_rendering_context.device, tex_transfer_buffer);

    if(surface != &_surface)
        SDL_DestroySurface(surface);

    return Texture(SDLPtr::make(m_rendering_context.device, texture), FSize(_surface.w, _surface.h));
}

Texture Renderer::createTexture(float _width, float _height, const char * _name) const
{
    SDL_GPUTextureCreateInfo texture_create_info = {};
    texture_create_info.type = SDL_GPU_TEXTURETYPE_2D;
    texture_create_info.format = SDL_GetGPUSwapchainTextureFormat(
        m_rendering_context.device,
        m_rendering_context.window);
    texture_create_info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    texture_create_info.width = _width;
    texture_create_info.height = _height;
    texture_create_info.layer_count_or_depth = 1;
    texture_create_info.num_levels = 1;
    SDL_GPUTexture * texture = SDL_CreateGPUTexture(m_rendering_context.device, &texture_create_info);
    if(!texture)
        throw SDLException("Unable to create texture");
    if(_name)
        SDL_SetGPUTextureName(m_rendering_context.device, texture, _name);
    return Texture(SDLPtr::make(m_rendering_context.device, texture), Size(_width, _height));
}

void Renderer::beginStep()
{
    if(m_rendering_context.command_buffer)
    {
        throw InvalidOperationException(
            "It is not possible to start a new rendering step until the previous one has completed");
    }

    m_rendering_context.command_buffer = SDL_AcquireGPUCommandBuffer(m_rendering_context.device);
    if(!m_rendering_context.command_buffer)
        throw SDLException("Unable to acquire a command buffer");

    if(!SDL_WaitAndAcquireGPUSwapchainTexture(
            m_rendering_context.command_buffer,
            m_rendering_context.window,
            &mp_swapchain_texture,
            &m_rendering_context.window_size.w,
            &m_rendering_context.window_size.h) ||
        !mp_swapchain_texture)
    {
        throw SDLException("Unable to acquire a swapchain texture");
    }
}

void Renderer::beginRenderPass(Texture & _texture, const SDL_FColor & _clear_color)
{
    if(!m_rendering_context.command_buffer)
    {
        throw InvalidOperationException(
            "A new rendering step cannot be started because the rendering step has not started");
    }
    if(m_rendering_context.render_pass)
    {
        throw InvalidOperationException(
            "It is not possible to start a new rendering pass until the previous one has completed");
    }

    // FIXME: sometimes a generic render pass cannot be used (MSAA, Stencil test)
    SDL_GPUColorTargetInfo color_target_info = {};
    color_target_info.texture = _texture.getTexture();
    color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
    color_target_info.store_op = SDL_GPU_STOREOP_STORE;
    color_target_info.clear_color = _clear_color;
    m_rendering_context.render_pass = SDL_BeginGPURenderPass(
        m_rendering_context.command_buffer,
        &color_target_info,
        1,
        nullptr);
    m_rendering_context.texture_size = _texture.getSize();
}

void Renderer::endRenderPass(const Texture & _texture, const SDL_FRect & _output_rect)
{
    if(!m_rendering_context.render_pass)
        throw InvalidOperationException("Render pass not running");

    m_line_renderer.beginRendering();
    while(!m_queue.empty())
    {
        Primitive * primitive = m_queue.front();
        primitive->render(m_rendering_context);
        delete primitive;
        m_queue.pop();
    }
    m_line_renderer.endRendering();
    SDL_EndGPURenderPass(m_rendering_context.render_pass);
    m_rendering_context.render_pass = nullptr;

    {
        SDL_GPUBlitInfo blit_info = {};
        blit_info.load_op = SDL_GPU_LOADOP_LOAD;
        blit_info.source.texture = _texture.getTexture();
        blit_info.source.x = .0f;
        blit_info.source.y = .0f;
        blit_info.source.w = _texture.getWidth();
        blit_info.source.h = _texture.getHeight();
        blit_info.destination.texture = mp_swapchain_texture;
        blit_info.destination.x = _output_rect.x;
        blit_info.destination.y = _output_rect.y;
        blit_info.destination.w = _output_rect.w;
        blit_info.destination.h = _output_rect.h;
        blit_info.filter = SDL_GPU_FILTER_NEAREST;
        SDL_BlitGPUTexture(m_rendering_context.command_buffer, &blit_info);
    }
}

void Renderer::submitStep()
{
    if(!m_rendering_context.command_buffer)
        throw InvalidOperationException("Rendering step not running");

    SDL_SubmitGPUCommandBuffer(m_rendering_context.command_buffer);
    m_rendering_context.command_buffer = nullptr;
    mp_swapchain_texture = nullptr;
}


void Renderer::renderRect(RectRenderingData && _data)
{
    m_queue.push(new RectPrimitive(m_rect_renderer, std::forward<RectRenderingData>(_data)));
}

void Renderer::renderRect(SolidRectRenderingData && _data)
{
    m_queue.push(new SolidRectPrimitive(m_rect_renderer, std::forward<SolidRectRenderingData>(_data)));
}

void Renderer::renderTexture(TextureRenderingData && _data)
{
    m_queue.push(new TexturePrimitive(m_rect_renderer, std::forward<TextureRenderingData>(_data)));
}

void Renderer::renderLine(const SDL_FPoint & _point1, const SDL_FPoint & _point2, const SDL_FColor & _color)
{
    m_queue.push(new LinePrimitive(m_line_renderer, m_line_renderer.enqueueLine(_point1, _point2), _color));
}

void Renderer::renderLines(const std::vector<SDL_FPoint> & _points, const SDL_FColor & _color)
{
    m_queue.push(new LinePrimitive(m_line_renderer, m_line_renderer.enqueueLines(_points), _color));
}

void Renderer::renderPolyline(const std::vector<SDL_FPoint> & _points, const SDL_FColor & _color, bool _close)
{
    m_queue.push(new LinePrimitive(m_line_renderer, m_line_renderer.enqueuePolyline(_points, _close), _color));
}
