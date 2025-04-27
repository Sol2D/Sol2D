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

#include <Sol2D/StepState.h>
#include <Sol2D/Outlet.h>
#include <Sol2D/UI.h>
#include <unordered_map>
#include <list>

namespace Sol2D {

class View final
{
    S2_DISABLE_COPY_AND_MOVE(View)

public:
    explicit View(Renderer & _renderer);
    uint16_t createFragment(const Area & _aria);
    const Area * getFragmentArea(uint16_t _id) const;
    bool updateFragment(uint16_t _id, const Area & _aria);
    bool deleteFragment(uint16_t _id);
    bool bindFragment(uint16_t _fragment_id, std::shared_ptr<Canvas> _canvas);
    void bindUI(std::shared_ptr<UI> _ui);
    void deleteUI();
    void resize();
    void step(const StepState & _state);

private:
    void emplaceOrderedOutlet(Outlet * _outlet);
    void eraseOrderedOutlet(Outlet * _outlet);

private:
    Renderer & m_renderer;
    std::unordered_map<uint16_t, std::unique_ptr<Outlet>> m_outlets;
    uint16_t m_next_fragment_id;
    std::list<Outlet *> m_ordered_outlets;
    std::shared_ptr<UI> m_ui; // TODO: move to Window
};

inline View::View(Renderer & _renderer) :
    m_renderer(_renderer),
    m_next_fragment_id(1)
{
}

inline void View::bindUI(std::shared_ptr<UI> _ui)
{
    m_ui = _ui;
}

inline void View::deleteUI()
{
    m_ui.reset();
}

} // namespace Sol2D
