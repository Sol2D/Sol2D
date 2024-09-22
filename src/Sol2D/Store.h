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

#include <Sol2D/View.h>
#include <Sol2D/Scene.h>
#include <Sol2D/Forms/Form.h>
#include <Sol2D/SDL/TTF.h>
#include <Sol2D/SDL/Mixer.h>
#include <Sol2D/Utils/ObjectStore.h>
#include <filesystem>

namespace Sol2D {

template<>
struct Utils::ObjectFactory<View>
{
    std::shared_ptr<View> produce(SDL_Renderer & _renderer) const
    {
        return std::make_shared<View>(_renderer);
    }
};

template<>
struct Utils::ObjectFactory<Sprite>
{
    std::shared_ptr<Sprite> produce(SDL_Renderer & _renderer) const
    {
        return std::make_shared<Sprite>(_renderer);
    }
};

template<>
struct Utils::ObjectFactory<SpriteSheet>
{
    std::shared_ptr<SpriteSheet> produce(SDL_Renderer & _renderer) const
    {
        return std::make_shared<SpriteSheet>(_renderer);
    }
};

template<>
struct Utils::ObjectFactory<GraphicsPack>
{
    std::shared_ptr<GraphicsPack> produce(SDL_Renderer & _renderer) const
    {
        return std::make_shared<GraphicsPack>(_renderer);
    }
};

template<>
struct Utils::ObjectFactory<BodyDefinition>
{
    std::shared_ptr<BodyDefinition> produce(std::shared_ptr<BodyDefinition> _definition) const
    {
        return _definition;
    }
};

template<>
struct Utils::ObjectFactory<Scene>
{
    std::shared_ptr<Scene> produce(
        const SceneOptions & _options,
        const Workspace & _workspace,
        SDL_Renderer & _renderer) const
    {
        return std::make_shared<Scene>(_options, _workspace, _renderer);
    }
};

template<>
struct Utils::ObjectFactory<Forms::Form>
{
    std::shared_ptr<Forms::Form> produce(SDL_Renderer & _renderer) const
    {
        return std::make_shared<Forms::Form>(_renderer);
    }
};

template<>
struct Utils::ObjectFactory<TTF_Font>
{
    std::shared_ptr<TTF_Font> produce(const std::filesystem::path & _file_path, uint16_t _size) const
    {
        return SDL::wrapFont(TTF_OpenFont(_file_path.c_str(), _size));
    }
};

template<>
struct Utils::ObjectFactory<Mix_Chunk>
{
    std::shared_ptr<Mix_Chunk> produce(const std::filesystem::path & _file_path) const
    {
        return SDL::wrapSoundChunk(Mix_LoadWAV(_file_path.c_str()));
    }
};

template<>
struct Utils::ObjectFactory<Mix_Music>
{
    std::shared_ptr<Mix_Music> produce(const std::filesystem::path & _file_path) const
    {
        return SDL::wrapMusic(Mix_LoadMUS(_file_path.c_str()));
    }
};

using Store = Utils::ObjectStore<
    View,
    Sprite,
    SpriteSheet,
    GraphicsPack,
    BodyDefinition,
    Scene,
    Forms::Form,
    TTF_Font,
    Mix_Chunk,
    Mix_Music>;

class StoreManager
{
public:
    std::shared_ptr<Store> createStore(const std::string & _key)
    {
        std::shared_ptr<Store> store = std::make_shared<Store>();
        m_stores[_key] = store;
        return store;
    }

    std::shared_ptr<Store> getStore(const std::string _key) const
    {
        auto it = m_stores.find(_key);
        return it == m_stores.end() ? nullptr : it->second;
    }

    bool deleteStore(const std::string & _key)
    {
        return m_stores.erase(_key) > 0;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Store>> m_stores;
};

} // namespace Sol2D
