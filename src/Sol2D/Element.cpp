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

#include <Sol2D/Element.h>
#include <Sol2D/Node.h>
#include <Sol2D/View.h>

using namespace Sol2D;

Element::Element(Node & _node, bool _enable_custom_render_pass) :
    m_view(_node.getView()),
    m_node(_node),
    m_is_custom_render_pass_enabled(_enable_custom_render_pass)
{
    m_view.registerElement(*this);
}

Element::~Element()
{
    m_view.unregisterElement(*this);
}

float Element::getX() const
{
    return m_node.getX();
}

float Element::getY() const
{
    return m_node.getY();
}

float Element::getWidth() const
{
    return m_node.getWidth();
}

float Element::getHeight() const
{
    return m_node.getHeight();
}

