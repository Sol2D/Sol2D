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
#include <Sol2D/Layouting/Layout.h>
#include <memory>
#include <list>

namespace Sol2D {

class Window final
{
    S2_DISABLE_COPY_AND_MOVE(Window)

public:
    Window();
    void step(const StepState & _state);
    void setView(std::shared_ptr<View> _view);
    std::shared_ptr<View> getView() const;
    void setLayout(std::shared_ptr<Layouting::Layout> _layout);
    std::shared_ptr<Layouting::Layout> getLayout();
    void resize(float _width, float _height);

private:
    // The list is used to allow a new view to be set during step processing.
    // If the view is replaced during the step, the application will crash.
    // Instead, we add the new view to the list and continue using the old view until the next step.
    // The list is truncated before each step.
    std::list<std::shared_ptr<View>> m_view_list; // TODO: delete
    // The list is used to allow a new layout to be set during step processing.
    // If the layout is replaced during the step, the application will crash.
    // Instead, we add the new layout to the list and continue using the old layout until the next step.
    // The list is truncated before each step.
    std::list<std::shared_ptr<Layouting::Layout>> m_layout_list;
};

inline Window::Window()
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

inline void Window::setLayout(std::shared_ptr<Layouting::Layout> _layout)
{
    m_layout_list.push_front(_layout);

}

inline std::shared_ptr<Layouting::Layout> Window::getLayout()
{
    return m_layout_list.empty() ? nullptr : m_layout_list.back();
}

inline void Window::resize(float _width, float _height)
{
    if(!m_view_list.empty())
        m_view_list.back()->resize(); // TODO: delete
    if(!m_layout_list.empty())
        m_layout_list.back()->recalculate(_width, _height);
}

} // namespace Sol2D
