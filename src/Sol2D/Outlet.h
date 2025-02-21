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

#include <Sol2D/Area.h>
#include <Sol2D/Canvas.h>
#include <Sol2D/MediaLayer/MediaLayer.h>

namespace Sol2D {

class Outlet final
{
    S2_DISABLE_COPY_AND_MOVE(Outlet)

public:
    Outlet(const Area & _area, Renderer & _renderer);
    void resize();
    void bind(std::shared_ptr<Canvas> _canvas);
    void reconfigure(const Area & _area);
    void step(const StepState & _state);
    const Area & getFragmentArea() const;

private:
    Area m_aria;
    Renderer & mr_renderer;
    Texture m_texture;
    SDL_FRect m_rect;
    std::shared_ptr<Canvas> m_canvas;
};

inline const Area & Outlet::getFragmentArea() const
{
    return m_aria;
}

} // namespace Sol2D
