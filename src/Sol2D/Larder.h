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

#include <Sol2D/SpriteAnimation.h>
#include <Sol2D/BodyPrototype.h>
#include <Sol2D/Workspace.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace Sol2D {

class Larder final
{
    S2_DISABLE_COPY_AND_MOVE(Larder)

public:
    explicit Larder(SDL_Renderer & _renderer) :
        mr_renderer(_renderer)
    {
    }

    std::shared_ptr<Sprite> createSprite(const std::string & _key);
    bool deleteSprite(const std::string & _key);
    std::shared_ptr<Sprite> getSprite(const std::string & _key) const;

    std::shared_ptr<SpriteSheet> createSpriteSheet(const std::string & _key);
    bool deleteSpriteSheet(const std::string & _key);
    std::shared_ptr<SpriteSheet> getSpriteSheet(const std::string & _key) const;

    std::shared_ptr<SpriteAnimation> createSpriteAnimation(const std::string & _key);
    bool deleteSpriteAnimation(const std::string & _key);
    std::shared_ptr<SpriteAnimation> getSpriteAnimation(const std::string & _key) const;

    std::shared_ptr<BodyPrototype> createBodyPrototype(const std::string & _key, BodyType _body_type);
    bool deleteBodyPrototype(const std::string & _key);
    std::shared_ptr<BodyPrototype> getBodyPrototype(const std::string & _key) const;

    std::shared_ptr<TTF_Font> getFont(const std::filesystem::path & _file_path, uint16_t _size);
    void freeFont(const std::filesystem::path & _file_path, uint16_t _size);

    std::shared_ptr<Mix_Chunk> getSoundChunk(const std::filesystem::path & _file_path);
    void freeSoundChunk(const std::filesystem::path & _file_path);

    std::shared_ptr<Mix_Music> getMusic(const std::filesystem::path & _file_path);
    void freeMusic(const std::filesystem::path & _file_path);

private:
    SDL_Renderer & mr_renderer;
    std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;
    std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> m_sprite_sheets;
    std::unordered_map<std::string, std::shared_ptr<SpriteAnimation>> m_animations;
    std::unordered_map<std::string, std::shared_ptr<BodyPrototype>> m_body_prototypes;
    std::unordered_map<std::string, std::shared_ptr<TTF_Font>> m_fonts;
    std::unordered_map<std::string, std::shared_ptr<Mix_Chunk>> m_sound_chunks;
    std::unordered_map<std::string, std::shared_ptr<Mix_Music>> m_musics;
};

} // namespace Sol2D
