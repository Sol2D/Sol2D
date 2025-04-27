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

#include <Sol2D/Layouting/Layout.h>
#include <yoga/Yoga.h>

using namespace Sol2D::Layouting;

Layout::Layout(const Style & _style) :
    Node(nullptr, _style)
{
}

void Layout::forceRecalculation()
{
    m_force_recalculate = true;
}

void Layout::recalculate(float _width, float _height)
{
    if(m_force_recalculate || m_calculated_width != _width || m_calculated_height != _height)
    {
        YGNodeCalculateLayout(m_node, _width, _height, YGDirectionLTR);
        m_force_recalculate = false;
        m_calculated_width = _width;
        m_calculated_height = _height;
    }
}
