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

#include <Sol2D/Outlet.h>

using namespace Sol2D;

Outlet::Outlet(const Fragment & _fragmet, Renderer & _renderer) :
    m_fragment(_fragmet),
    mr_renderer(_renderer),
    m_rect{.0f, .0f, .0f, .0f}
{
}

void Outlet::resize()
{
    if(!m_canvas || !m_fragment.is_visible)
        return;

    const FSize output_size = mr_renderer.getOutputSize();
    m_rect.x = m_fragment.left.has_value() ? m_fragment.left.value().getPixels(output_size.w) : .0f;
    m_rect.y = m_fragment.top.has_value() ? m_fragment.top.value().getPixels(output_size.h): .0f;

    if(m_fragment.width.has_value())
    {
        m_rect.w = m_fragment.width.value().getPixels(output_size.w);
    }
    else if(m_fragment.right.has_value())
    {
        float right_margin = m_fragment.right.value().getPixels(output_size.w);
        m_rect.w = output_size.w - (m_rect.x + right_margin);
    }
    else
    {
        m_rect.w = output_size.w - m_rect.x;
    }

    if(m_fragment.height.has_value())
    {
        m_rect.h = m_fragment.height.value().getPixels(output_size.h);
    }
    else if(m_fragment.bottom.has_value())
    {
        float bottom_margin = m_fragment.bottom.value().getPixels(output_size.h);
        m_rect.h = output_size.h - (m_rect.y + bottom_margin);
    }
    else
    {
        m_rect.h = output_size.h - m_rect.y;
    }
    m_canvas->reconfigure(m_rect);
    m_texture = mr_renderer.createTexture(m_rect.w, m_rect.h, "Outlet");
}

void Outlet::bind(std::shared_ptr<Canvas> _canvas)
{
    m_canvas = _canvas;
    resize();
}

void Outlet::reconfigure(const Fragment & _fragment)
{
    m_fragment = _fragment;
    resize();
}

void Outlet::step(const StepState & _state)
{
    if(!m_canvas) return;
    mr_renderer.beginRenderPass(m_texture, m_canvas->getClearColor());
    m_canvas->step(_state);
    mr_renderer.endRenderPass(m_texture, m_rect);
}
