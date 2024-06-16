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
#include <Sol2D/Utils/KeyValueStorage.h>
#include <Sol2D/Workspace.h>
#include <Sol2D/SDL/TTF.h>
#include <Sol2D/SDL/Mixer.h>
#include <SDL3/SDL_render.h>
#include <string>
#include <filesystem>

namespace Sol2D {

class Larder final
{
    S2_DISABLE_COPY_AND_MOVE(Larder)

public:
    explicit Larder(SDL_Renderer & _renderer) :
        mr_renderer(_renderer)
    {
    }

    Sprite & createSprite(const std::string & _key)
    {
        return m_sprites.addOrReplaceItem(_key, new Sprite(mr_renderer));
    }

    bool deleteSprite(const std::string & _key)
    {
        return m_sprites.deleteItem(_key);
    }

    Sprite * getSprite(const std::string & _key)
    {
        return m_sprites.getItem(_key);
    }

    const Sprite * getSprite(const std::string & _key) const
    {
        return m_sprites.getItem(_key);
    }

    SpriteSheet & createSpriteSheet(const std::string & _key)
    {
        return m_sprite_sheets.addOrReplaceItem(_key, new SpriteSheet(mr_renderer));
    }

    bool deleteSpriteSheet(const std::string & _key)
    {
        return m_sprite_sheets.deleteItem(_key);
    }

    SpriteSheet * getSpriteSheet(const std::string & _key)
    {
        return m_sprite_sheets.getItem(_key);
    }

    const SpriteSheet * getSpriteSheet(const std::string & _key) const
    {
        return m_sprite_sheets.getItem(_key);
    }

    SpriteAnimation & createSpriteAnimation(const std::string & _key)
    {
        return m_animations.addOrReplaceItem(_key, new SpriteAnimation(mr_renderer));
    }

    bool deleteSpriteAnimation(const std::string & _key)
    {
        return m_animations.deleteItem(_key);
    }

    SpriteAnimation * getSpriteAnimation(const std::string & _key)
    {
        return m_animations.getItem(_key);
    }

    const SpriteAnimation * getSpriteAnimation(const std::string & _key) const
    {
        return m_animations.getItem(_key);
    }

    BodyPrototype & createBodyPrototype(const std::string & _key, BodyType _body_type)
    {
        return m_body_prototypes.addOrReplaceItem(_key, new BodyPrototype(_body_type));
    }

    bool deleteBodyPrototype(const std::string & _key)
    {
        return m_body_prototypes.deleteItem(_key);
    }

    BodyPrototype * getBodyPrototype(const std::string & _key)
    {
        return m_body_prototypes.getItem(_key);
    }

    const BodyPrototype * getBodyPrototype(const std::string & _key) const
    {
        return m_body_prototypes.getItem(_key);
    }

    SDL::FontPtr getFont(const std::filesystem::path & _file_path, uint16_t _size);

    void freeFont(const std::filesystem::path & _file_path, uint16_t _size);

    SDL::SoundChunkPtr getSoundChunk(const std::filesystem::path & _file_path);

    void freeSoundChunk(const std::filesystem::path & _file_path);

private:
    SDL_Renderer & mr_renderer;
    Utils::KeyValueStorage<std::string, Sprite> m_sprites;
    Utils::KeyValueStorage<std::string, SpriteSheet> m_sprite_sheets;
    Utils::KeyValueStorage<std::string, SpriteAnimation> m_animations;
    Utils::KeyValueStorage<std::string, BodyPrototype> m_body_prototypes;
    std::unordered_map<std::string, SDL::FontPtr> m_fonts;
    std::unordered_map<std::string, SDL::SoundChunkPtr> m_sound_chunks;
};

} // namespace Sol2D
