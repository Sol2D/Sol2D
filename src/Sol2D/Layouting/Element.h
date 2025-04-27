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

#include <Sol2D/StepState.h>
#include <Sol2D/MediaLayer/MediaLayer.h>

namespace Sol2D::Layouting {

class Node;

class Element
{
    S2_DISABLE_COPY_AND_MOVE(Element)

public:
    explicit Element(const Node & _node) :
      m_node(_node)
    {
    }

    virtual ~Element() { }
    virtual void step(const StepState & _step) = 0;
    virtual const FSize * getDesiredSize() const { return nullptr; }
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

protected:
    const Node & m_node;
};

} // namespace Sol2D::Layouting
