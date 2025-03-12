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

#include <unordered_map>
#include <memory>

namespace Sol2D::Utils {

template<typename T>
struct ObjectFactory
{
    ObjectFactory() = delete;
};

template<typename T>
concept ObjectConcept = std::default_initializable<ObjectFactory<T>> &&
    std::is_member_function_pointer<decltype(&ObjectFactory<T>::produce)>::value;

template<typename T, typename... Object>
concept AnyObjectConcept = (std::same_as<T, Object> || ...);

template<typename T>
struct ObjectStoreData
{
    std::unordered_map<std::string, std::shared_ptr<T>> m_objects;
};

template<ObjectConcept... ObjectType>
class ObjectStore : private ObjectStoreData<ObjectType>...
{
public:
    template<AnyObjectConcept<ObjectType...> T, typename... FactoryArgs>
    std::shared_ptr<T> createObject(const std::string & _key, FactoryArgs &&... _factory_args)
    {
        ObjectFactory<T> factory;
        std::shared_ptr<T> object = factory.produce(_factory_args...);
        ObjectStoreData<T>::m_objects[_key] = object;
        return object;
    }

    template<AnyObjectConcept<ObjectType...> T>
    std::shared_ptr<T> getObject(const std::string & _key) const
    {
        auto it = ObjectStoreData<T>::m_objects.find(_key);
        return it == ObjectStoreData<T>::m_objects.cend() ? nullptr : it->second;
    }

    template<AnyObjectConcept<ObjectType...> T>
    bool freeObject(const std::string & _key)
    {
        return ObjectStoreData<T>::m_objects.erase(_key) > 0;
    }
};

} // namespace Sol2D::Utils
