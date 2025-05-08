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

#include <Sol2D/Canvas.h>

using namespace Sol2D;

void Canvas::step(const StepState & _step)
{
    const float width = getWidth();
    const float height = getHeight();
    if(std::isnormal(width) && std::isnormal(height))
    {
        if(m_texture.getWidth() != width || m_texture.getHeight() != height)
            m_texture = m_renderer.createTexture(width, height, getTextureName());
        m_renderer.beginRenderPass(m_texture, &m_clear_color);
        executeStep(_step);
        m_renderer.endRenderPass(m_texture, { getX(), getY(), width, height });
    }
}
