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

#include <Sol2D/Forms/Widget.h>

using namespace Sol2D::Forms;

void Widget::step(const StepState & /*_state*/)
{
    renderBorder();
}

void Widget::renderBorder()
{
    float border_width = this->border_width[m_state];
    if(border_width <= .0f)
    {
        return;
    }
    mr_renderer.renderRect(RectRenderingData(
        {
            .x = m_x.getPixels(mr_parent.getWidth()),
            .y = m_y.getPixels(mr_parent.getHeight()),
            .w = m_width.getPixels(mr_parent.getWidth()),
            .h = m_height.getPixels(mr_parent.getHeight())
        },
        this->border_color[m_state],
        border_width));
}
