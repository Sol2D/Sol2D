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

#include <Sol2D/Window.h>

using namespace Sol2D;

void Window::step(const StepState & _state)
{
    if(m_view_list.empty())
        return;
    m_view_list.resize(1);
    SDL_SetRenderDrawColor(&mr_renderer, 255, 165, 0, 255);
    SDL_RenderClear(&mr_renderer);
    m_view_list.back()->step(_state);
    SDL_RenderPresent(&mr_renderer);
}
