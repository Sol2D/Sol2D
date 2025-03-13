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

#include <Sol2D/Forms/Button.h>

using namespace Sol2D;
using namespace Sol2D::Forms;

Button::Button(const Canvas & _parent, const std::string & _text, Renderer & _renderer) :
    Label(_parent, _text, _renderer)
{
}

void Button::step(const StepState & _state)
{
    handleState(_state);
    Label::step(_state);
}

void Button::handleState(const StepState & _state)
{
    SDL_FRect rect {
        .x = m_x.getPixels(m_parent.getWidth()),
        .y = m_y.getPixels(m_parent.getHeight()),
        .w = m_width.getPixels(m_parent.getWidth()),
        .h = m_height.getPixels(m_parent.getHeight())
    };
    if(!isPointIn(_state.mouse_state.position, rect))
    {
        setState(WidgetState::Default);
        return;
    }
    switch(_state.mouse_state.lb_click.state)
    {
    case MouseClickState::None:
        setState(WidgetState::Focused);
        break;
    case MouseClickState::Started:
        if(isPointIn(_state.mouse_state.lb_click.start, rect))
            setState(WidgetState::Activated);
        else
            setState(WidgetState::Default);
        break;
    case MouseClickState::Finished:
        setState(WidgetState::Focused);
        if(isPointIn(_state.mouse_state.lb_click.start, rect))
            callObservers(&ButtonClickObserver::onClick);
        break;
    }
}

bool Button::isPointIn(const SDL_FPoint & _point, const SDL_FRect & _rect) const
{
    SDL_FPoint point = m_parent.getTranslatedPoint(_point.x, _point.y);
    return point.x >= _rect.x && point.y >= _rect.y && (point.x < _rect.x + _rect.w) && (point.y < _rect.y + _rect.h);
}
