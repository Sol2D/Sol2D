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

#include <Sol2D/Element.h>

namespace Sol2D {

class Canvas : public Element
{
public:
    Canvas(Renderer & _renderer, Node & _node) :
        Element(_node, true),
        m_renderer(_renderer)
    {
    }

    void setClearColor(const SDL_FColor & _color)
    {
        m_clear_color = _color;
    }

    const SDL_FColor & getClearColor() const
    {
        return m_clear_color;
    }

    void step(const StepState & _step) override;

protected:
    virtual const char * getTextureName() const
    {
        return "Canvas";
    }

    Texture & getTexture()
    {
        return m_texture;
    }

    virtual void executeStep(const StepState & _step) = 0;

private:
    Renderer & m_renderer;
    SDL_FColor m_clear_color;
    Texture m_texture;
};

} // namespace Sol2D
