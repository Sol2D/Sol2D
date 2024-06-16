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

#include <Sol2D/Larder.h>

using namespace Sol2D;
using namespace Sol2D::SDL;

namespace {

inline std::string makeFontKey(const std::filesystem::path & _file_path, uint16_t _size)
{
    return std::format("{}_{}", _file_path.string(), _size);
}

} // namespace name

FontPtr Larder::getFont(const std::filesystem::path & _file_path, uint16_t _size)
{
    const std::string key = makeFontKey(_file_path, _size);
    {
        auto it = m_fonts.find(key);
        if(it != m_fonts.end())
            return it->second;
    }
    TTF_Font * font = TTF_OpenFont(_file_path.c_str(), _size);
    if(font)
    {
        FontPtr ptr = wrapTtfFont(font);
        m_fonts[key] = ptr;
        return ptr;
    }
    return nullptr;
}

void Larder::freeFont(const std::filesystem::path & _file_path, uint16_t _size)
{
    const std::string key = makeFontKey(_file_path, _size);
    m_fonts.erase(key);
}

SDL::SoundChunkPtr Larder::getSoundChunk(const std::filesystem::path & _file_path)
{
    {
        auto it = m_sound_chunks.find(_file_path.string());
        if(it != m_sound_chunks.end())
            return it->second;
    }
    Mix_Chunk * chunk = Mix_LoadWAV(_file_path.c_str());
    if(chunk)
    {
        SDL::SoundChunkPtr ptr = SDL::wrapSoundChunk(chunk);
        m_sound_chunks[_file_path.string()] = ptr;
        return ptr;
    }
    return nullptr;
}

void Larder::freeSoundChunk(const std::filesystem::path & _file_path)
{
    m_sound_chunks.erase(_file_path.string());
}

SDL::MusicPtr Larder::getMusic(const std::filesystem::path & _file_path)
{
    {
        auto it = m_musics.find(_file_path.string());
        if(it != m_musics.end())
            return it->second;
    }
    Mix_Music * music = Mix_LoadMUS(_file_path.c_str());
    if(music)
    {
        SDL::MusicPtr ptr = SDL::wrapMusic(music);
        m_musics[_file_path.string()] = ptr;
        return ptr;
    }
    return nullptr;
}

void Larder::freeMusic(const std::filesystem::path & _file_path)
{
    m_musics.erase(_file_path.string());
}
