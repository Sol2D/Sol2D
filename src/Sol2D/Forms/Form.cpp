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

void Form::render(const RenderState & _state)
{
    SDL_SetRenderDrawColor(&mr_renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, m_bg_color.a);
    SDL_RenderClear(&mr_renderer);
    for(auto & widget : m_widgets)
        widget->render(_state);
}

std::shared_ptr<Label> Form::createLabel(const std::string & _text)
{
    std::shared_ptr<Label> widget = std::make_shared<Label>(*this, _text, mr_renderer);
    m_widgets.push_back(widget);
    return widget;
}

std::shared_ptr<Button> Form::createButton(const std::string & _text)
{
    std::shared_ptr<Button> button = std::make_shared<Button>(*this, _text, mr_renderer);
    m_widgets.push_back(button);
    return button;
}
