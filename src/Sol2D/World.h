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
#include <Sol2D/Forms/Form.h>
#include <Sol2D/Larder.h>
#include <Sol2D/Outlet.h>
#include <list>

namespace Sol2D {

using FragmentID = uint16_t;

class World final
{
    enum class RenderableKind
    {
        Scene,
        Form
    };

    struct Renderable
    {
        RenderableKind kind;
        std::shared_ptr<Canvas> canvas;
    };

    S2_DISABLE_COPY_AND_MOVE(World)

public:
    World(SDL_Renderer & _renderer, const Workspace & _workspace);
    std::shared_ptr<Scene> createScene(const std::string & _name);
    std::shared_ptr<Scene> getScene(const std::string & _name) const;
    std::shared_ptr<Forms::Form> createForm(const std::string & _name);
    std::shared_ptr<Forms::Form> getFrom(const std::string & _name) const;
    std::shared_ptr<Larder> createLarder(const std::string & _key);
    std::shared_ptr<Larder> getLarder(const std::string _key) const;
    bool deleteLarder(const std::string & _key);
    FragmentID createFragment(const Fragment & _fragment);
    const Fragment * getFragment(FragmentID _id) const;
    bool updateFragment(FragmentID _id, const Fragment & _fragment);
    bool deleteFragment(FragmentID _id);
    bool bindFragment(FragmentID _fragment_id, const std::string & _name);
    Uint8 * getKeyboardState() const;
    void resize();
    void render(const RenderState & _state);

private:
    void emplaceOrderedOutlet(Outlet * _outlet);
    void eraseOrderedOutlet(Outlet * _outlet);

private:
    SDL_Renderer & mr_renderer;
    const Workspace & mr_workspace;
    std::unordered_map<std::string, std::shared_ptr<Larder>> m_larders;
    std::unordered_map<std::string, Renderable> m_renderables;
    std::unordered_map<FragmentID, std::unique_ptr<Outlet>> m_outlets;
    FragmentID m_next_fragment_id;
    std::list<Outlet *> m_ordered_outlets;
};

} // namespace Sol2D
