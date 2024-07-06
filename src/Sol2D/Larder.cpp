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

std::shared_ptr<Sprite> Larder::createSprite(const std::string & _key)
{
    std::shared_ptr<Sprite> sprite(new Sprite(mr_renderer));
    m_sprites[_key] = sprite;
    return sprite;
}

bool Larder::deleteSprite(const std::string & _key)
{
    return m_sprites.erase(_key) > 0;
}

std::shared_ptr<Sprite> Larder::getSprite(const std::string & _key) const
{
    auto it = m_sprites.find(_key);
    return it == m_sprites.cend() ? nullptr : it->second;
}

std::shared_ptr<SpriteSheet> Larder::createSpriteSheet(const std::string & _key)
{
    std::shared_ptr<SpriteSheet> sprite_sheet(new SpriteSheet(mr_renderer));
    m_sprite_sheets[_key] = sprite_sheet;
    return sprite_sheet;
}

bool Larder::deleteSpriteSheet(const std::string & _key)
{
    return m_sprite_sheets.erase(_key) > 0;
}

std::shared_ptr<SpriteSheet> Larder::getSpriteSheet(const std::string & _key) const
{
    auto it = m_sprite_sheets.find(_key);
    return it == m_sprite_sheets.cend() ? nullptr : it->second;
}

std::shared_ptr<SpriteAnimation> Larder::createSpriteAnimation(const std::string & _key)
{
    std::shared_ptr<SpriteAnimation> animation(new SpriteAnimation(mr_renderer));
    m_animations[_key] = animation;
    return animation;
}

bool Larder::deleteSpriteAnimation(const std::string & _key)
{
    return m_animations.erase(_key) > 0;
}

std::shared_ptr<SpriteAnimation> Larder::getSpriteAnimation(const std::string & _key) const
{
    auto it = m_animations.find(_key);
    return it == m_animations.cend() ? nullptr : it->second;
}

std::shared_ptr<BodyPrototype> Larder::createBodyPrototype(const std::string & _key, BodyType _body_type)
{
    std::shared_ptr<BodyPrototype> proto(new BodyPrototype(_body_type));
    m_body_prototypes[_key] = proto;
    return proto;
}

bool Larder::deleteBodyPrototype(const std::string & _key)
{
    return m_body_prototypes.erase(_key) > 0;
}

std::shared_ptr<BodyPrototype> Larder::getBodyPrototype(const std::string & _key) const
{
    auto it = m_body_prototypes.find(_key);
    return it == m_body_prototypes.cend() ? nullptr : it->second;
}

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
