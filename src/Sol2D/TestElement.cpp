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

#include <Sol2D/TestElement.h>
#include <Sol2D/Layouting/Node.h>
#include <iostream>

using namespace Sol2D;
using namespace Sol2D::Layouting;

TestElement::TestElement(Renderer & _renderer, const SDL_FColor & _color, const Node & _node) :
    Element(_node),
    m_renderer(_renderer),
    m_color(_color)
{
}

void TestElement::step(const StepState & /*_step*/)
{
    static unsigned long frame = 0;

    m_renderer.renderRect(
        SolidRectRenderingData(
            { .x = getX(), .y = getY(), .w = getWidth(), .h = getHeight() },
            m_color
        )
    );

    std::cout << "rendering (" << ++frame << "): " << m_node.getWidth() << "x" << m_node.getHeight() << std::endl;
}
