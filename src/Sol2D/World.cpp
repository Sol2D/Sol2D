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

World::World(SDL_Renderer & _renderer, const Workspace & _workspace) :
    mr_renderer(_renderer),
    mr_workspace(_workspace)
{
}

World::~World()
{
    for(auto & pair : m_scenes)
        delete pair.second;
}

Scene & World::createScene(const std::string & _name)
{
    Scene * scene = new Scene(mr_workspace, mr_renderer);
    m_scenes.insert(std::make_pair(_name, scene));
    return *scene;
}

Scene * World::getScene(const std::string & _name)
{
    auto it = m_scenes.find(_name);
    return it == m_scenes.end() ? nullptr : it->second;
}

void World::render(const SDL_FRect & _viewport, std::chrono::milliseconds _time_passed)
{
    SDL_SetRenderDrawColor(&mr_renderer, 255, 165, 0, 0); // TODO: from Lua
    SDL_RenderClear(&mr_renderer);

    for(auto & pair : m_scenes) // TODO: layer order, position(?)
        pair.second->render(_viewport, _time_passed);

    SDL_RenderPresent(&mr_renderer);
}
