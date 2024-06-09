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

#include <Sol2D/Forms/Form.h>

using namespace Sol2D::Forms;

Form::Form(SDL_Renderer & _renderer) :
    mr_renderer(_renderer),
    m_bg_color{0, 0, 0, 255}
{
}

Form::~Form()
{
    for(auto * widget : m_widgets)
        delete widget;
}

void Form::render(const RenderState & _state)
{
    SDL_SetRenderDrawColor(&mr_renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, m_bg_color.a);
    SDL_RenderClear(&mr_renderer);
    for(auto * widget : m_widgets)
        widget->render(_state);
}

Label & Form::createLabel(const std::string & _text)
{
    Label * label = new Label(*this, _text, mr_renderer);
    m_widgets.push_back(label);
    return *label;
}

Button & Form::createButton(const std::string & _text)
{
    Button * button = new Button(*this, _text, mr_renderer);
    m_widgets.push_back(button);
    return *button;
}
