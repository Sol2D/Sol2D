// Sol2D Game Engine
// Copyright (C) 2023-2024 Sergey Smolyannikov aka brainstream
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

#include <Sol2D/Outlet.h>
#include <Sol2D/SDL/SDL.h>

using namespace Sol2D;
using namespace Sol2D::SDL;

Outlet::Outlet(const Fragment & _fragmet, SDL_Renderer & _renderer) :
    m_fragment(_fragmet),
    mr_renderer(_renderer),
    m_rect{.0f, .0f, .0f, .0f},
    mp_canvas(nullptr)
{
}

void Outlet::resize()
{
    if(!mp_canvas || !m_fragment.is_visible)
        return;

    int viewport_w, viewport_h;
    SDL_GetRenderOutputSize(&mr_renderer, &viewport_w, &viewport_h);
    m_rect.x = m_fragment.left.has_value() ? m_fragment.left.value().getPixels(viewport_w) : .0f;
    m_rect.y = m_fragment.top.has_value() ? m_fragment.top.value().getPixels(viewport_h): .0f;

    if(m_fragment.width.has_value())
    {
        m_rect.w = m_fragment.width.value().getPixels(viewport_w);
    }
    else if(m_fragment.right.has_value())
    {
        float right_margin = m_fragment.right.value().getPixels(viewport_w);
        m_rect.w = viewport_w - (m_rect.x + right_margin);
    }
    else
    {
        m_rect.w = viewport_w - m_rect.x;
    }

    if(m_fragment.height.has_value())
    {
        m_rect.h = m_fragment.height.value().getPixels(viewport_h);
    }
    else if(m_fragment.bottom.has_value())
    {
        float bottom_margin = m_fragment.bottom.value().getPixels(viewport_h);
        m_rect.h = viewport_h - (m_rect.y + bottom_margin);
    }
    else
    {
        m_rect.h = viewport_h - m_rect.y;
    }

    if(m_rect.h < 1.0f || m_rect.w < 1.0f)
    {
        m_texture_ptr = nullptr;
    }
    else
    {
        SDL_Texture * texture = SDL_CreateTexture(
            &mr_renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_TARGET,
            m_rect.w,
            m_rect.h
        );
        m_texture_ptr = wrapTexture(texture);
    }
    mp_canvas->reconfigure(m_rect);
}

void Outlet::bind(Canvas & _canvas)
{
    mp_canvas = &_canvas;
    resize();
}

void Outlet::reconfigure(const Fragment & _fragment)
{
    if(!_fragment.is_visible && m_fragment.is_visible)
        m_texture_ptr.reset();
    m_fragment = _fragment;
    resize();
}

void Outlet::render(const RenderState & _state)
{
    if(!mp_canvas || !m_texture_ptr)
        return;
    SDL_SetRenderTarget(&mr_renderer, m_texture_ptr.get());
    mp_canvas->render(_state);
    SDL_SetRenderTarget(&mr_renderer, nullptr);
    SDL_RenderTexture(&mr_renderer, m_texture_ptr.get(), nullptr, m_rect.toSdlPtr());
}
