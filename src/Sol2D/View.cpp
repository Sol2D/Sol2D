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

#include <Sol2D/View.h>
#include <yoga/Yoga.h>

using namespace Sol2D;

View::View(const Style & _style) :
    m_layout(Node(*this, nullptr, _style)),
    m_calculated_width(.0f),
    m_calculated_height(.0f),
    m_force_recalculate(false),
    m_destroying(false)
{
}

View::~View()
{
    m_destroying = true;
}

void View::forceRecalculation()
{
    m_force_recalculate = true;
}

void View::recalculate(float _width, float _height)
{
    if(m_force_recalculate || m_calculated_width != _width || m_calculated_height != _height)
    {
        YGNodeCalculateLayout(m_layout.m_node, _width, _height, YGDirectionLTR);
        m_force_recalculate = false;
        m_calculated_width = _width;
        m_calculated_height = _height;
    }
}

void View::registerElement(Element & _element)
{
    m_elements.push_back(&_element);
}

void View::unregisterElement(Element & _element)
{
    if(!m_destroying)
    {
        auto end = std::remove(m_elements.begin(), m_elements.end(), &_element);
        m_elements.resize(end - m_elements.begin());
    }
}

void View::step(const StepState & _step)
{
    bool is_render_pass_started = false;
    for(Element * element : m_elements)
    {
        if(element->isCustomRenderPassEnabled())
        {
            if(is_render_pass_started)
            {
                // submit defalt render pass
                is_render_pass_started = false;
            }
            element->step(_step);
        }
        else
        {
            if(!is_render_pass_started)
            {
                // start render pass
                is_render_pass_started = true;
            }
            element->step(_step);
        }
    }
    if(is_render_pass_started)
    {
        // submit defalt render pass
    }
}
