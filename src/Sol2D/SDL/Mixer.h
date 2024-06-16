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

#include <SDL3_mixer/SDL_mixer.h>
#include <memory>

namespace Sol2D::SDL {

using SoundChunkPtr = std::shared_ptr<Mix_Chunk>;
using MusicPtr = std::shared_ptr<Mix_Music>;

inline SoundChunkPtr wrapSoundChunk(Mix_Chunk * _chunk)
{
    return std::shared_ptr<Mix_Chunk>(_chunk, Mix_FreeChunk);
}

inline MusicPtr wrapMusic(Mix_Music * _music)
{
    return std::shared_ptr<Mix_Music>(_music, Mix_FreeMusic);
}

} // namespace Sol2D::SDL
