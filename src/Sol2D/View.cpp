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


#include <Sol2D/View.h>

using namespace Sol2D;

uint16_t View::createFragment(const Area & _area)
{
    uint16_t id = m_next_fragment_id++;
    Outlet * outlet = new Outlet(_area, mr_renderer);
    m_outlets[id] = std::unique_ptr<Outlet>(outlet);
    emplaceOrderedOutlet(outlet);
    return id;
}

const Area * View::getFragmentArea(uint16_t _id) const
{
    auto it = m_outlets.find(_id);
    return it == m_outlets.cend() ? nullptr : &it->second->getFragmentArea();
}

bool View::updateFragment(uint16_t _id, const Area & _area)
{
    auto it = m_outlets.find(_id);
    if(it == m_outlets.end() )
        return false;
    bool need_to_reorder = _area.z_index != it->second->getFragmentArea().z_index;
    it->second->reconfigure(_area);
    if(need_to_reorder)
    {
        eraseOrderedOutlet(it->second.get());
        emplaceOrderedOutlet(it->second.get());
    }
    return true;
}

void View::emplaceOrderedOutlet(Outlet * _outlet)
{
    uint16_t z_index = _outlet->getFragmentArea().z_index;
    auto it = std::find_if(
        m_ordered_outlets.begin(),
        m_ordered_outlets.end(),
        [z_index](const Outlet * __outlet) { return z_index < __outlet->getFragmentArea().z_index; });
    m_ordered_outlets.insert(it, _outlet);
}

void View::eraseOrderedOutlet(Outlet * _outlet)
{
    auto it = std::find(m_ordered_outlets.begin(), m_ordered_outlets.end(), _outlet);
    if(it != m_ordered_outlets.end())
        m_ordered_outlets.erase(it);
}

bool View::deleteFragment(uint16_t _id)
{
    auto it = m_outlets.find(_id);
    if(it == m_outlets.end() )
        return false;
    eraseOrderedOutlet(it->second.get());
    m_outlets.erase(_id);
    return true;
}

bool View::bindFragment(uint16_t _fragment_id, std::shared_ptr<Canvas> _canvas)
{
    auto outlet_it = m_outlets.find(_fragment_id);
    if(outlet_it == m_outlets.end() )
        return false;
    outlet_it->second->bind(_canvas);
    return true;
}

void View::resize()
{
    for(auto & pair : m_outlets)
        pair.second->resize();
}

void View::step(const StepState & _state)
{
    for(auto & pair : m_outlets)
    {
        pair.second->step(_state);
    }
    if(m_ui)
    {
        mr_renderer.beginDefaultRenderPass();
        mr_renderer.renderUI(*m_ui);
        mr_renderer.endDefaultRenderPass();
    }
}
