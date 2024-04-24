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

using namespace Sol2D;

Outlet::Outlet(const Fragment & _fragmet, SDL_Renderer & _renderer) :
    m_fragment(_fragmet),
    mr_renderer(_renderer),
    m_rect{.0f, .0f, .0f, .0f},
    mp_scene(nullptr)
{
}

void Outlet::resize()
{
    if(!mp_scene || !m_fragment.is_visible)
        return;

    int viewport_w, viewport_h;
    SDL_GetRenderOutputSize(&mr_renderer, &viewport_w, &viewport_h);

    if(m_fragment.left.has_value())
    {
        if(m_fragment.left.value().unit == FragmentSizeUnit::Percent)
            m_rect.x = viewport_w / 100.0f * m_fragment.left.value().value;
        else
            m_rect.x = static_cast<float>(m_fragment.left.value().value);
    }
    else
    {
        m_rect.x = .0f;
    }

    if(m_fragment.top.has_value())
    {
        if(m_fragment.top.value().unit == FragmentSizeUnit::Percent)
            m_rect.y = viewport_h / 100.0f * m_fragment.top.value().value;
        else
            m_rect.y = static_cast<float>(m_fragment.top.value().value);
    }
    else
    {
        m_rect.y = .0f;
    }

    if(m_fragment.width.has_value())
    {
        if(m_fragment.width.value().unit == FragmentSizeUnit::Percent)
            m_rect.w = viewport_w / 100.0f * m_fragment.width.value().value;
        else
            m_rect.w = static_cast<float>(m_fragment.width.value().value);
    }
    else if(m_fragment.right.has_value())
    {
        float right_margin = static_cast<float>(m_fragment.right.value().value);
        if(m_fragment.right.value().unit == FragmentSizeUnit::Percent)
            right_margin = viewport_w / 100.0f * right_margin;
        m_rect.w = viewport_w - (m_rect.x + right_margin);
    }
    else
    {
        m_rect.w = viewport_w - m_rect.x;
    }

    if(m_fragment.height.has_value())
    {
        if(m_fragment.height.value().unit == FragmentSizeUnit::Percent)
            m_rect.h = viewport_h / 100.0f * m_fragment.height.value().value;
        else
            m_rect.h = static_cast<float>(m_fragment.height.value().value);
    }
    else if(m_fragment.bottom.has_value())
    {
        float bottom_margin = static_cast<float>(m_fragment.bottom.value().value);
        if(m_fragment.bottom.value().unit == FragmentSizeUnit::Percent)
            bottom_margin = viewport_h / 100.0f * bottom_margin;
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
        m_texture_ptr = wrapSdlTexturePtr(texture);
    }
}

void Outlet::bind(Scene & _scene)
{
    mp_scene = &_scene;
    resize();
}

void Outlet::reconfigure(const Fragment & _fragment)
{
    if(!_fragment.is_visible && m_fragment.is_visible)
        m_texture_ptr.reset();
    m_fragment = _fragment;
    resize();
}

void Outlet::render(std::chrono::milliseconds _time_passed)
{
    if(!mp_scene || !m_texture_ptr)
        return;
    SDL_SetRenderTarget(&mr_renderer, m_texture_ptr.get());
    mp_scene->render(_time_passed);
    SDL_SetRenderTarget(&mr_renderer, nullptr);
    SDL_RenderTexture(&mr_renderer, m_texture_ptr.get(), nullptr, &m_rect);
}
