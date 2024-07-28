// Sol2D Game Engine
// Copyright (C) 2023-2024 Sergey Smolyannikov aka brainstream
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

#include <Sol2D/RenderState.h>
#include <Sol2D/Object.h>
#include <Sol2D/Def.h>

namespace Sol2D {

class Canvas : public Object
{
    S2_DISABLE_COPY_AND_MOVE(Canvas)

public:
    Canvas() { }
    void reconfigure(const Rect & _rect);
    float getWidth() const;
    float getHeight() const;
    virtual void render(const RenderState & _state) = 0;
    Point getTranslatedPoint(float _x, float _y) const;
    void translatePoint(float * _x, float * _y) const;

private:
    Rect m_rect;
};

inline void Canvas::reconfigure(const Rect & _rect)
{
    m_rect = _rect;
}

inline float Canvas::getWidth() const
{
    return m_rect.w;
}

inline float Canvas::getHeight() const
{
    return m_rect.h;
}

inline Point Canvas::getTranslatedPoint(float _x, float _y) const
{
    return makePoint(_x - m_rect.x, _y - m_rect.y);
}

inline void Canvas::translatePoint(float * _x, float * _y) const
{
    *_x -= m_rect.x;
    *_y -= m_rect.y;
}

} // namespace Sol2D
