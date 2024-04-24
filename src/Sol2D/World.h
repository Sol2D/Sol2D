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

#include <Sol2D/Workspace.h>
#include <Sol2D/BodyPrototype.h>
#include <Sol2D/Scene.h>
#include <Sol2D/Utils/KeyValueStorage.h>
#include <Sol2D/Larder.h>
#include <Sol2D/Outlet.h>
#include <unordered_map>
#include <list>

namespace Sol2D {

using FragmentID = uint16_t;

class World final
{
    S2_DISABLE_COPY_AND_MOVE(World)

public:
    World(SDL_Renderer & _renderer, const Workspace & _workspace);
    Scene & createScene(const std::string & _name);
    Scene * getScene(const std::string & _name);
    bool loadLevelFromTmx(const std::filesystem::path & _tmx_file);
    Larder & createLarder(const std::string & _key);
    Larder * getLarder(const std::string & _key);
    const Larder * getLarder(const std::string _key) const;
    bool deleteLarder(const std::string & _key);
    FragmentID createFragment(const Fragment & _fragment);
    const Fragment * getFragment(FragmentID _id) const;
    bool updateFragment(FragmentID _id, const Fragment & _fragment);
    bool deleteFragment(FragmentID _id);
    bool bindFragment(FragmentID _fragment_id, const std::string & _scene_name);
    Uint8 * getKeyboardState() const;
    void resize();
    void render(std::chrono::milliseconds /*_time_passed*/);

private:
    void emplaceOrderedOutlet(Outlet * _outlet);
    void eraseOrderedOutlet(Outlet * _outlet);

private:
    SDL_Renderer & mr_renderer;
    const Workspace & mr_workspace;
    Utils::KeyValueStorage<std::string, Larder> m_larders;
    Utils::KeyValueStorage<std::string, Scene> m_scenes;
    FragmentID m_next_fragment_id;
    Utils::KeyValueStorage<FragmentID, Outlet> m_outlets;
    std::list<Outlet *> m_ordered_outlets;
};

inline Larder & World::createLarder(const std::string & _key)
{
    return m_larders.addOrReplaceItem(_key, new Larder(mr_renderer));
}

inline Larder * World::getLarder(const std::string & _key)
{
    return m_larders.getItem(_key);
}

inline const Larder * World::getLarder(const std::string _key) const
{
    return m_larders.getItem(_key);
}
inline bool World::deleteLarder(const std::string & _key)
{
    return m_larders.deleteItem(_key);
}

} // namespace Sol2D
