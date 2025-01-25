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

#include <Sol2D/View.h>
#include <SDL3/SDL_render.h>
#include <memory>
#include <list>

namespace Sol2D {

class Window final
{
    S2_DISABLE_COPY_AND_MOVE(Window)

public:
    explicit Window(SDL_Renderer & _renderer);
    void step(const StepState & _state);
    void setView(std::shared_ptr<View> _view);
    std::shared_ptr<View> getView() const;
    void resize();

private:
    SDL_Renderer & mr_renderer;

    // The list is used to allow a new view to be set during step processing.
    // If the view is replaced during the step, the application will crash.
    // Instead, we add the new view to the list and continue using the old view until the next step.
    // The list is truncated before each step.
    std::list<std::shared_ptr<View>> m_view_list;
};

inline Window:: Window(SDL_Renderer & _renderer) :
    mr_renderer(_renderer)
{
}

inline void Window::setView(std::shared_ptr<View> _view)
{
    m_view_list.push_front(_view);
}

inline std::shared_ptr<View> Window::getView() const
{
    return m_view_list.empty() ? nullptr : m_view_list.back();
}

inline void Window::resize()
{
    if(!m_view_list.empty())
        m_view_list.back()->resize();
}

} // namespace Sol2D
