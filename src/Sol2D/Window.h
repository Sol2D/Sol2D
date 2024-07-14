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

#include <Sol2D/View.h>
#include <SDL3/SDL_render.h>
#include <memory>

namespace Sol2D {

class Window final
{
    S2_DISABLE_COPY_AND_MOVE(Window)

public:
    explicit Window(SDL_Renderer & _renderer);
    void render(const RenderState & _state);
    void setView(std::shared_ptr<View> _view);
    std::shared_ptr<View> getView() const;
    void resize();

private:
    SDL_Renderer & mr_renderer;
    std::shared_ptr<View> m_view;
};

inline Window:: Window(SDL_Renderer & _renderer) :
    mr_renderer(_renderer)
{
}

inline void Window::setView(std::shared_ptr<View> _view)
{
    m_view = _view;
}

inline std::shared_ptr<View> Window::getView() const
{
    return m_view;
}

inline void Window::resize()
{
    if(m_view) m_view->resize();
}

} // namespace Sol2D
