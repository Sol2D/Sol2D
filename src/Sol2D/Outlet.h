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

#include <Sol2D/SDL.h>
#include <Sol2D/Fragment.h>
#include <Sol2D/Scene.h>
#include <chrono>

namespace Sol2D {

class Outlet final
{
    S2_DISABLE_COPY_AND_MOVE(Outlet)

public:
    Outlet(const Fragment & _fragmet, SDL_Renderer & _renderer);
    void resize();
    void bind(Scene & _scene);
    void reconfigure(const Fragment & _fragment);
    void render(std::chrono::milliseconds _time_passed);
    const Fragment & getFragment() const;

private:
    Fragment m_fragment;
    SDL_Renderer & mr_renderer;
    SDL_FRect m_rect;
    Scene * mp_scene;
    SDL_TexturePtr m_texture_ptr;
};

inline const Fragment & Outlet::getFragment() const
{
    return m_fragment;
}

} // namespace Sol2D
