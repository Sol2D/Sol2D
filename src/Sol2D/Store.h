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

#include <Sol2D/World/Scene.h>
// #include <Sol2D/Forms/Form.h>
#include <Sol2D/UI.h>
#include <Sol2D/View.h>
#include <Sol2D/Utils/ObjectStore.h>
#include <filesystem>

namespace Sol2D {

template<>
struct Utils::ObjectFactory<Sprite>
{
    std::shared_ptr<Sprite> produce(Renderer & _renderer) const
    {
        return std::make_shared<Sprite>(_renderer);
    }
};

template<>
struct Utils::ObjectFactory<SpriteSheet>
{
    std::shared_ptr<SpriteSheet> produce(Renderer & _renderer) const
    {
        return std::make_shared<SpriteSheet>(_renderer);
    }
};

template<>
struct Utils::ObjectFactory<World::Scene>
{
    std::shared_ptr<World::Scene> produce(
        Node & _node,
        const World::SceneOptions & _options,
        const Workspace & _workspace,
        Renderer & _renderer) const
    {
        return std::make_shared<World::Scene>(_node, _options, _workspace, _renderer);
    }
};

// template<>
// struct Utils::ObjectFactory<Forms::Form>
// {
//     std::shared_ptr<Forms::Form> produce(Renderer & _renderer) const
//     {
//         return std::make_shared<Forms::Form>(_renderer);
//     }
// };

template<>
struct Utils::ObjectFactory<UI>
{
    std::shared_ptr<UI> produce() const
    {
        return std::make_shared<UI>();
    }
};

template<>
struct Utils::ObjectFactory<View>
{
    std::shared_ptr<View> produce(Renderer & _renderer, const Style & _style = Style()) const
    {
        return std::make_shared<View>(_renderer, _style);
    }
};

template<>
struct Utils::ObjectFactory<TTF_Font>
{
    std::shared_ptr<TTF_Font> produce(const std::filesystem::path & _file_path, uint16_t _size) const
    {
        return SDLPtr::make(TTF_OpenFont(_file_path.c_str(), _size));
    }
};

template<>
struct Utils::ObjectFactory<Mix_Chunk>
{
    std::shared_ptr<Mix_Chunk> produce(const std::filesystem::path & _file_path) const
    {
        return SDLPtr::make(Mix_LoadWAV(_file_path.c_str()));
    }
};

template<>
struct Utils::ObjectFactory<Mix_Music>
{
    std::shared_ptr<Mix_Music> produce(const std::filesystem::path & _file_path) const
    {
        return SDLPtr::make(Mix_LoadMUS(_file_path.c_str()));
    }
};

using Store = Utils::ObjectStore<
    Sprite,
    SpriteSheet,
    World::Scene,
    // Forms::Form,
    UI,
    View,
    TTF_Font,
    Mix_Chunk,
    Mix_Music
>;

class StoreManager
{
public:
    std::shared_ptr<Store> createStore(const std::string & _key)
    {
        std::shared_ptr<Store> store = std::make_shared<Store>();
        m_stores[_key] = store;
        return store;
    }

    std::shared_ptr<Store> getStore(const std::string & _key) const
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
