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

#include <Sol2D/Scene.h>
#include <SDL3_image/SDL_image.h>

using namespace Sol2D;

Scene::Scene(SDL_Renderer & _renderer, const Workspace & _workspace) :
    mr_renderer(_renderer),
    mr_workspace(_workspace),
    mp_level(nullptr)
{
}

Scene::~Scene()
{
    delete mp_level;
    for(BodyPrototype * proto : m_body_prototypes)
        delete proto;
}

bool Scene::loadLevelFromTmx(const std::filesystem::path & _tmx_file)
{
    if(!mp_level)
        mp_level = new Level(mr_renderer, mr_workspace);
    return mp_level->loadFromTmx(_tmx_file);
}

Level * Scene::getLevel()
{
    return mp_level;
}

BodyPrototype & Scene::createBodyPrototype()
{
    BodyPrototype * proto = new BodyPrototype(mr_renderer);
    m_body_prototypes.push_back(proto);
    return *proto;
}

void Scene::render(const SDL_FRect & _viewport)
{
    SDL_SetRenderDrawColor(&mr_renderer, 255, 165, 0, 0);
    SDL_RenderClear(&mr_renderer);
    mp_level->render(_viewport);
    SDL_RenderPresent(&mr_renderer);
}
