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

#include <SDL3/SDL.h>
#include <memory>

namespace Sol2D::SDL {

using TexturePtr = std::shared_ptr<SDL_Texture>;

inline TexturePtr wrapSdlTexturePtr(SDL_Texture * _texture)
{
    return std::shared_ptr<SDL_Texture>(_texture, SDL_DestroyTexture);
}

void sdlRenderCircle(SDL_Renderer * _renderer, const SDL_FPoint & _center, uint32_t _radius);

} // namespace Sol2D::SDL

bool operator == (const SDL_Color & _color1, const SDL_Color & _color2);
