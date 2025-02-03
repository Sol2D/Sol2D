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
#include <Sol2D/MediaLayer/RenderingData.h>
#include <Sol2D/MediaLayer/RectRenderer.h>
#include <Sol2D/MediaLayer/RenderingContext.h>
#include <Sol2D/MediaLayer/Primitive.h>
#include <Sol2D/Def.h>
#include <Sol2D/MediaLayer.h>
#include <queue>

namespace Sol2D {

class Renderer final
{
    S2_DISABLE_COPY_AND_MOVE(Renderer)

public:
    Renderer(const ResourceManager & _resource_manager, SDL_Window * _window, SDL_GPUDevice * _device);
    ~Renderer();
    const FSize getOutputSize() const;
    Texture createTexture(SDL_Surface & _surface, const char * _name = nullptr) const;
    Texture createTexture(float _width, float _height, const char * _name = nullptr) const;

    void beginStep();
    void beginRenderPass(Texture & _texture, const SDL_FColor & _clear_color);
    void endRenderPass(const Texture & _texture, const SDL_FRect & _output_rect);
    void submitStep();

    void renderRect(RectRenderingData && _data);
    void renderRect(SolidRectRenderingData && _data);
    void renderTexture(TextureRenderingData && _data);

private:
    const ResourceManager & mr_resource_manager;
    RenderingContext m_rendering_context;
    SDL_GPUTexture * mp_swapchain_texture;
    RectRenderer m_rect_renderer;
    std::queue<Primitive *> m_queue;
};

} // namespace Sol2D
