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

Outlet::Outlet(const Area & _area, Renderer & _renderer) :
    m_aria(_area),
    m_renderer(_renderer),
    m_rect {.0f, .0f, .0f, .0f}
{
}

void Outlet::resize()
{
    if(m_canvas && m_aria.is_visible)
    {
        m_rect = m_aria.calculateRect(m_renderer.getOutputSize());
        m_canvas->reconfigure(m_rect);
        m_texture = m_renderer.createTexture(m_rect.w, m_rect.h, "Outlet");
    }
}

void Outlet::bind(std::shared_ptr<Canvas> _canvas)
{
    m_canvas = _canvas;
    resize();
}

void Outlet::reconfigure(const Area & _area)
{
    m_aria = _area;
    resize();
}

void Outlet::step(const StepState & _state)
{
    if(m_canvas && m_aria.is_visible)
    {
        m_renderer.beginRenderPass(m_texture, &m_canvas->getClearColor());
        m_canvas->step(_state);
        m_renderer.endRenderPass(m_texture, m_rect);
    }
}
