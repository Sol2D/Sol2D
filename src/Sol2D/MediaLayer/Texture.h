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

#pragma once

#include <Sol2D/MediaLayer/Size.h>
#include <Sol2D/MediaLayer/SDLPtr.h>
#include <Sol2D/Def.h>

namespace Sol2D {

class Texture final
{
public:
    S2_DEFAULT_COPY_AND_MOVE(Texture)

    Texture()
    {
    }

    Texture(std::shared_ptr<SDL_GPUTexture> _texture, const FSize & _size) :
        m_texture(_texture),
        m_size(_size)
    {
    }

    operator bool () const
    {
        return m_texture != nullptr;
    }

    bool operator== (std::nullopt_t) const
    {
        return m_texture == nullptr;
    }

    bool operator!= (std::nullopt_t) const
    {
        return m_texture != nullptr;
    }

    SDL_GPUTexture * getTexture() const
    {
        return m_texture.get();
    }

    operator SDL_GPUTexture * () const
    {
        return m_texture.get();
    }

    const FSize & getSize() const
    {
        return m_size;
    }

    float getWidth() const
    {
        return m_size.w;
    }

    float getHeight() const
    {
        return m_size.h;
    }

    void reset()
    {
        m_texture.reset();
        m_size.w = .0f;
        m_size.h = .0f;
    }

private:
    std::shared_ptr<SDL_GPUTexture> m_texture;
    FSize m_size;
};

} // namespace Sol2D
