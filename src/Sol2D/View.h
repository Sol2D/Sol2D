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

#pragma once

#include <Sol2D/Node.h>
#include <Sol2D/MediaLayer/Renderer.h>
#include <vector>

namespace Sol2D {

class View final
{
    friend class Node;

public:
    explicit View(Renderer & _renderer, const Style & _style = Style());
    ~View();
    void recalculate(float _width, float _height);
    Node & getLayout() { return m_layout; }
    const Node & getLayout() const { return m_layout; }
    void step(const StepState & _step);

private:
    void forceRecalculation();
    void registerElement(Element & _element);
    void unregisterElement(Element & _element);

private:
    Renderer & m_renderer;
    Node m_layout;
    std::vector<Element *> m_elements;
    float m_calculated_width;
    float m_calculated_height;
    bool m_force_recalculate;
    bool m_destroying;
};

} // namespace Sol2D
