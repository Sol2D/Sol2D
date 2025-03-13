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

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <memory>

namespace Sol2D {

class SDLPtr
{
    class TextureDeleter
    {
    public:
        TextureDeleter(SDL_GPUDevice * _device) :
            m_device(_device)
        {
        }

        void operator() (SDL_GPUTexture * _texture) noexcept
        {
            if(_texture)
                SDL_ReleaseGPUTexture(m_device, _texture);
        }

    private:
        SDL_GPUDevice * m_device;
    };

public:
    static std::shared_ptr<SDL_GPUTexture> make(SDL_GPUDevice * _device, SDL_GPUTexture * _texture)
    {
        return std::shared_ptr<SDL_GPUTexture>(_texture, TextureDeleter(_device));
    }

    static std::shared_ptr<TTF_Font> make(TTF_Font * _font)
    {
        return std::shared_ptr<TTF_Font>(_font, TTF_CloseFont);
    }

    static std::shared_ptr<Mix_Chunk> make(Mix_Chunk * _chunk)
    {
        return std::shared_ptr<Mix_Chunk>(_chunk, Mix_FreeChunk);
    }

    static std::shared_ptr<Mix_Music> make(Mix_Music * _music)
    {
        return std::shared_ptr<Mix_Music>(_music, Mix_FreeMusic);
    }
};

} // namespace Sol2D
