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

#include <Sol2D/Forms/WidgetProperty.h>
#include <Sol2D/Forms/WidgetPadding.h>
#include <Sol2D/Canvas.h>
#include <Sol2D/Object.h>
#include <Sol2D/MediaLayer/MediaLayer.h>

namespace Sol2D::Forms {

class Widget : public Object
{
    S2_DISABLE_COPY_AND_MOVE(Widget)

public:
    ~Widget() override;
    void setX(const Dimension_Obsolete<float> & _x);
    const Dimension_Obsolete<float> & getX() const;
    void setY(const Dimension_Obsolete<float> & _y);
    const Dimension_Obsolete<float> & getY() const;
    void setWidth(const Dimension_Obsolete<float> & _width);
    const Dimension_Obsolete<float> & getWidth() const;
    void setHeight(const Dimension_Obsolete<float> & _height);
    const Dimension_Obsolete<float> & setHeight() const;
    virtual void step(const StepState & _state);
    virtual bool setState(WidgetState _state);
    WidgetState getState() const;

public:
    WidgetProperty<std::shared_ptr<TTF_Font>> font;
    WidgetProperty<SDL_FColor> foreground_color;
    WidgetProperty<SDL_FColor> background_color;
    WidgetProperty<float> border_width;
    WidgetProperty<SDL_FColor> border_color;
    WidgetProperty<WidgetPadding> padding;

protected:
    Widget(const Canvas & _parent, Renderer & _renderer);
    const Canvas & getParent() const;

private:
    void renderBorder();

protected:
    const Canvas & m_parent;
    Renderer & m_renderer;
    WidgetState m_state;
    Dimension_Obsolete<float> m_x;
    Dimension_Obsolete<float> m_y;
    Dimension_Obsolete<float> m_width;
    Dimension_Obsolete<float> m_height;

    static constexpr SDL_FColor sc_default_foreground_color = {1.0f, 1.0f, 1.0f, 1.0f};
    static constexpr SDL_FColor sc_default_background_color = {.0f, .0f, .0f, .0f};
    static constexpr SDL_FColor sc_default_border_color = sc_default_foreground_color;
};

inline Widget::Widget(const Canvas & _parent, Renderer & _renderer) :
    font(nullptr),
    foreground_color(sc_default_foreground_color),
    background_color(sc_default_background_color),
    border_width(0),
    border_color(sc_default_border_color),
    padding(.0f),
    m_parent(_parent),
    m_renderer(_renderer),
    m_state(WidgetState::Default),
    m_x(.0f),
    m_y(.0f),
    m_width(100.f, DimensionUnit::Percent),
    m_height(100.0f, DimensionUnit::Percent)
{
}

inline Widget::~Widget()
{
}

inline const Canvas & Widget::getParent() const
{
    return m_parent;
}

inline void Widget::setX(const Dimension_Obsolete<float> & _x)
{
    m_x = _x;
}

inline const Dimension_Obsolete<float> & Widget::getX() const
{
    return m_x;
}

inline void Widget::setY(const Dimension_Obsolete<float> & _y)
{
    m_y = _y;
}

inline const Dimension_Obsolete<float> & Widget::getY() const
{
    return m_y;
}

inline void Widget::setWidth(const Dimension_Obsolete<float> & _width)
{
    m_width = _width;
}

inline const Dimension_Obsolete<float> & Widget::getWidth() const
{
    return m_width;
}

inline void Widget::setHeight(const Dimension_Obsolete<float> & _height)
{
    m_height = _height;
}

inline const Dimension_Obsolete<float> & Widget::setHeight() const
{
    return m_height;
}

inline bool Widget::setState(WidgetState _state)
{
    if(_state != m_state)
    {
        m_state = _state;
        return true;
    }
    return false;
}

inline WidgetState Widget::getState() const
{
    return m_state;
}

} // namespace Sol2D::Forms
