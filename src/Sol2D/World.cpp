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
using namespace Sol2D::Utils;

World::World(SDL_Renderer & _renderer, const Workspace & _workspace) :
    mr_renderer(_renderer),
    mr_workspace(_workspace),
    m_next_fragment_id(1)
{
}

std::shared_ptr<Scene> World::createScene(const std::string & _name)
{
    Scene * scene = new Scene(mr_workspace, mr_renderer);
    Renderable renderable
    {
        .kind = RenderableKind::Scene,
        .canvas = std::shared_ptr<Canvas>(scene)
    };
    m_renderables[_name] = renderable;
    return std::static_pointer_cast<Scene>(renderable.canvas);
}

std::shared_ptr<Scene> World::getScene(const std::string & _name) const
{
    auto it = m_renderables.find(_name);
    if(it == m_renderables.cend() || it->second.kind != RenderableKind::Scene)
        return nullptr;
    return std::static_pointer_cast<Scene>(it->second.canvas);
}

std::shared_ptr<Form> World::createForm(const std::string & _name)
{
    Form * form = new Form(mr_renderer);
    Renderable renderable
    {
        .kind = RenderableKind::Form,
        .canvas = std::shared_ptr<Canvas>(form)
    };
    m_renderables[_name] = renderable;
    return std::static_pointer_cast<Form>(renderable.canvas);
}

std::shared_ptr<Form> World::getFrom(const std::string & _name) const
{
    auto it = m_renderables.find(_name);
    if(it == m_renderables.cend() || it->second.kind != RenderableKind::Form)
        return nullptr;
    return std::static_pointer_cast<Form>(it->second.canvas);
}

std::shared_ptr<Store> World::createStore(const std::string & _key)
{
    std::shared_ptr<Store> store = std::make_shared<Store>();
    m_stores[_key] = store;
    return store;
}

std::shared_ptr<Store> World::getStore(const std::string _key) const
{
    auto it = m_stores.find(_key);
    return it == m_stores.end() ? nullptr : it->second;
}

bool World::deleteStore(const std::string & _key)
{
    return m_stores.erase(_key) > 0;
}

FragmentID World::createFragment(const Fragment & _fragment)
{
    FragmentID id = m_next_fragment_id++;
    Outlet * outlet = new Outlet(_fragment, mr_renderer);
    m_outlets[id] = std::unique_ptr<Outlet>(outlet);
    emplaceOrderedOutlet(outlet);
    return id;
}

const Fragment * World::getFragment(FragmentID _id) const
{
    auto it = m_outlets.find(_id);
    return it == m_outlets.cend() ? nullptr : &it->second->getFragment();
}

bool World::updateFragment(FragmentID _id, const Fragment & _fragment)
{
    auto it = m_outlets.find(_id);
    if(it == m_outlets.end() )
        return false;
    bool need_to_reorder = _fragment.z_index != it->second->getFragment().z_index;
    it->second->reconfigure(_fragment);
    if(need_to_reorder)
    {
        eraseOrderedOutlet(it->second.get());
        emplaceOrderedOutlet(it->second.get());
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
    auto it = m_outlets.find(_id);
    if(it == m_outlets.end() )
        return false;
    eraseOrderedOutlet(it->second.get());
    m_outlets.erase(_id);
    return true;
}

bool World::bindFragment(FragmentID _fragment_id, const std::string & _name)
{
    auto outlet_it = m_outlets.find(_fragment_id);
    if(outlet_it == m_outlets.end() )
        return false;
    auto renderables_it = m_renderables.find(_name);
    if(renderables_it == m_renderables.end())
        return false;

    outlet_it->second->bind(*renderables_it->second.canvas.get());
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
