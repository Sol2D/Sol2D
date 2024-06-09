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

#include <Sol2D/World.h>
#include <SDL3_image/SDL_image.h>

using namespace Sol2D;
using namespace Sol2D::Forms;

World::World(SDL_Renderer & _renderer, const Workspace & _workspace) :
    mr_renderer(_renderer),
    mr_workspace(_workspace),
    m_next_fragment_id(1)
{
}

Scene & World::createScene(const std::string & _name)
{
    Scene * scene = new Scene(mr_workspace, mr_renderer);
    Renderable * renderable = new Renderable(scene);
    m_renderables.addOrReplaceItem(_name, renderable);
    return *scene;
}

Scene * World::getScene(const std::string & _name)
{
    Renderable * renderable = m_renderables.getItem(_name);
    return renderable ? renderable->tryGetScene() : nullptr;
}

Form & World::createForm(const std::string & _name)
{
    Form * form = new Form(mr_renderer);
    Renderable * renderable = new Renderable(form);
    m_renderables.addOrReplaceItem(_name, renderable);
    return *form;
}

Form * World::getFrom(const std::string & _name)
{
    Renderable * renderable = m_renderables.getItem(_name);
    return renderable ? renderable->tryGetForm() : nullptr;
}

FragmentID World::createFragment(const Fragment & _fragment)
{
    FragmentID id = m_next_fragment_id++;
    Outlet * outlet = new Outlet(_fragment, mr_renderer);
    m_outlets.addOrReplaceItem(id, outlet);
    emplaceOrderedOutlet(outlet);
    return id;
}

const Fragment * World::getFragment(FragmentID _id) const
{
    const Outlet * outlet = m_outlets.getItem(_id);
    return outlet ? &outlet->getFragment() : nullptr;
}

bool World::updateFragment(FragmentID _id, const Fragment & _fragment)
{
    Outlet * outlet = m_outlets.getItem(_id);
    if(!outlet)
        return false;
    bool need_to_reorder = _fragment.z_index != outlet->getFragment().z_index;
    outlet->reconfigure(_fragment);
    if(need_to_reorder)
    {
        eraseOrderedOutlet(outlet);
        emplaceOrderedOutlet(outlet);
    }
    return true;
}

void World::emplaceOrderedOutlet(Outlet * _outlet)
{
    uint16_t z_index = _outlet->getFragment().z_index;
    auto it = std::find_if(
        m_ordered_outlets.begin(),
        m_ordered_outlets.end(),
        [z_index](Outlet * __outlet) { return z_index < __outlet->getFragment().z_index; });
    m_ordered_outlets.insert(it, _outlet);
}

void World::eraseOrderedOutlet(Outlet * _outlet)
{
    auto it = std::find(m_ordered_outlets.begin(), m_ordered_outlets.end(), _outlet);
    if(it != m_ordered_outlets.end())
        m_ordered_outlets.erase(it);
}

bool World::deleteFragment(FragmentID _id)
{
    Outlet * outlet = m_outlets.getItem(_id);
    if(!outlet) return false;
    eraseOrderedOutlet(outlet);
    m_outlets.deleteItem(_id);
    return true;
}

bool World::bindFragment(FragmentID _fragment_id, const std::string & _name)
{
    Outlet * outlet = m_outlets.getItem(_fragment_id);
    if(!outlet) return false;
    Renderable * renderable = m_renderables.getItem(_name);
    if(!renderable) return false;
    outlet->bind(*renderable->getCanvas());
    return true;
}

void World::resize()
{
    for(auto & pair : m_outlets)
        pair.second->resize();
}

void World::render(const RenderState & _state)
{
    SDL_SetRenderDrawColor(&mr_renderer, 255, 165, 0, 0); // TODO: from Lua
    SDL_RenderClear(&mr_renderer);
    for(auto & pair : m_outlets)
        pair.second->render(_state);
    SDL_RenderPresent(&mr_renderer);
}
