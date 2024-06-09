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

#include <Sol2D/Forms/WidgetProperty.h>
#include <Sol2D/Forms/WidgetPadding.h>
#include <Sol2D/Canvas.h>
#include <Sol2D/SDL/TTF.h>
#include <Sol2D/SDL/SDL.h>

namespace Sol2D::Forms {

class Widget
{
    S2_DISABLE_COPY_AND_MOVE(Widget)

public:
    virtual ~Widget();
    void setX(const Dimension<float> & _x);
    const Dimension<float> & getX() const;
    void setY(const Dimension<float> & _y);
    const Dimension<float> & getY() const;
    void setWidth(const Dimension<float> & _width);
    const Dimension<float> & getWidth() const;
    void setHeight(const Dimension<float> & _height);
    const Dimension<float> & setHeight() const;
    virtual void render(const RenderState & _state);
    virtual bool setState(WidgetState _state);
    WidgetState getState() const;

public:
    WidgetProperty<SDL::FontPtr> font;
    WidgetProperty<SDL_Color> foreground_color;
    WidgetProperty<SDL_Color> background_color;
    WidgetProperty<float> border_width;
    WidgetProperty<SDL_Color> border_color;
    WidgetProperty<WidgetPadding> padding;

protected:
    Widget(const Canvas & _parent, SDL_Renderer & _renderer);
    const Canvas & getParent() const;

private:
    void renderBorder();

protected:
    const Canvas & mr_parent;
    SDL_Renderer & mr_renderer;
    WidgetState m_state;
    Dimension<float> m_x;
    Dimension<float> m_y;
    Dimension<float> m_width;
    Dimension<float> m_height;

    static constexpr SDL_Color sc_default_foreground_color{255, 255, 255, 255};
    static constexpr SDL_Color sc_default_background_color{0, 0, 0, 0};
    static constexpr SDL_Color sc_default_border_color = sc_default_foreground_color;
};

inline Widget::Widget(const Canvas & _parent, SDL_Renderer & _renderer):
    font(nullptr),
    foreground_color(sc_default_foreground_color),
    background_color(sc_default_background_color),
    border_width(0),
    border_color(sc_default_border_color),
    padding(.0f),
    mr_parent(_parent),
    mr_renderer(_renderer),
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
    return mr_parent;
}

inline void Widget::setX(const Dimension<float> & _x)
{
    m_x = _x;
}

inline const Dimension<float> & Widget::getX() const
{
    return m_x;
}

inline void Widget::setY(const Dimension<float> & _y)
{
    m_y = _y;
}

inline const Dimension<float> & Widget::getY() const
{
    return m_y;
}

inline void Widget::setWidth(const Dimension<float> & _width)
{
    m_width = _width;
}

inline const Dimension<float> & Widget::getWidth() const
{
    return m_width;
}

inline void Widget::setHeight(const Dimension<float> & _height)
{
    m_height = _height;
}

inline const Dimension<float> & Widget::setHeight() const
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
