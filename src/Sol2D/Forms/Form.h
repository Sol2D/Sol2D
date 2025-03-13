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

#include <Sol2D/Forms/Label.h>
#include <Sol2D/Forms/Button.h>
#include <Sol2D/Canvas.h>
#include <vector>

namespace Sol2D::Forms {

class Form final : public Canvas
{
public:
    explicit Form(Renderer & _renderer);
    void step(const StepState & _state) override;
    std::shared_ptr<Label> createLabel(const std::string & _text);
    std::shared_ptr<Button> createButton(const std::string & _text);

private:
    Renderer & m_renderer;
    std::vector<std::shared_ptr<Widget>> m_widgets;
};

} // namespace Sol2D::Forms
