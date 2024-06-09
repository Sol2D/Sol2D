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

#include <Sol2D/Forms/Widget.h>
#include <Sol2D/Forms/Label.h>
#include <Sol2D/Forms/Button.h>
#include <Sol2D/Canvas.h>
#include <SDL3/SDL.h>
#include <vector>

namespace Sol2D::Forms {

class Form final : public Canvas
{
public:
    explicit Form(SDL_Renderer & _renderer);
    ~Form() override;
    void render(const RenderState & _state) override;
    void setBackgroundColor(const SDL_Color & _color);
    Label & createLabel(const std::string & _text);
    Button & createButton(const std::string & _text);

private:
    SDL_Renderer & mr_renderer;
    SDL_Color m_bg_color;
    std::vector<Widget *> m_widgets;
};

inline void Form::setBackgroundColor(const SDL_Color & _color)
{
    m_bg_color = _color;
}

} // namespace Sol2D::Forms
