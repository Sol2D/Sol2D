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
#include <memory>

namespace Sol2D {

class Window final
{
    S2_DISABLE_COPY_AND_MOVE(Window)

public:
    Window();
    void step(const StepState & _state);
    void setView(std::shared_ptr<View> _view);
    std::shared_ptr<View> getView();
    void resize(float _width, float _height);

private:
    std::shared_ptr<View> m_view;
    // This variable is used to set a new view during step processing.
    // If the view is replaced during the step, the application will crash.
    // Instead, we store the new view in this variable and continue using the old view until the next step.
    std::shared_ptr<View> m_next_view;
};

inline Window::Window()
{
}

inline void Window::setView(std::shared_ptr<View> _view)
{
    m_next_view = _view;
}

inline std::shared_ptr<View> Window::getView()
{
    return m_view;
}

inline void Window::resize(float _width, float _height)
{
    if(m_next_view)
        m_next_view->recalculate(_width, _height);
    else if(m_view)
        m_view->recalculate(_width, _height);
}

} // namespace Sol2D
