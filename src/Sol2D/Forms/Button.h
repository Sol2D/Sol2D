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

namespace Sol2D::Forms {

class ButtonClickObserver
{
public:
    virtual ~ButtonClickObserver()
    {
    }

    virtual void onClick() = 0;
};

class Button : public Label, public Utils::Observable<ButtonClickObserver>
{
public:
    Button(const Canvas & _parent, const std::string & _text, Renderer & _renderer);
    void step(const StepState & _state) override;

private:
    void handleState(const StepState & _state);
    bool isPointIn(const SDL_FPoint & _point, const SDL_FRect & _rect) const;
};

} // namespace Sol2D::Forms
