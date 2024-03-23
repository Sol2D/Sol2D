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

#include <Sol2D/Def.h>
#include <unordered_map>
#include <memory>

namespace Sol2D::Utils {

template<typename T, typename V>
concept KeyValueStorageDeleterConcept = std::default_initializable<T> && std::invocable<T, V*>;

template<typename T>
concept KeyValueStorageKeyConcept = std::copy_constructible<T>;

template<
    KeyValueStorageKeyConcept Key,
    typename Value,
    KeyValueStorageDeleterConcept<Value> Deleter = std::default_delete<Value>>
class KeyValueStorage final
{
    S2_DISABLE_COPY_AND_MOVE(KeyValueStorage)

public:
    using Map = std::unordered_map<Key, Value *>;
    using iterator = Map::iterator;
    using const_iterator = Map::const_iterator;

public:
    KeyValueStorage() { }
    ~KeyValueStorage();
    Value & addOrReplaceItem(const Key & _key, Value * _value);
    bool deleteItem(const Key & _key);
    Value * getItem(const Key & _key);
    const Value * getItem(const Key & _key) const;

    iterator begin() noexcept { return m_map.begin(); }
    const_iterator begin() const noexcept { return m_map.begin(); }
    const_iterator cbegin() const noexcept { return m_map.cbegin(); }
    iterator end() noexcept { return m_map.end(); }
    const_iterator end() const noexcept { return m_map.end(); }
    const_iterator cend() const noexcept { return m_map.cend(); }

    iterator find(const Key & _key) { return m_map.find(_key); }
    const_iterator find(const Key & _key) const { return m_map.find(_key); }

private:
    Map m_map;
    Deleter m_deleter;
};

template<KeyValueStorageKeyConcept Key, typename Value, KeyValueStorageDeleterConcept<Value> Deleter>
KeyValueStorage<Key, Value, Deleter>::~KeyValueStorage()
{
    for(auto & pair : m_map)
        m_deleter(pair.second);
}

template<KeyValueStorageKeyConcept Key, typename Value, KeyValueStorageDeleterConcept<Value> Deleter>
Value & KeyValueStorage<Key, Value, Deleter>::addOrReplaceItem(const Key & _key, Value * _value)
{
    deleteItem(_key);
    m_map[_key] = _value;
    return *_value;
}

template<KeyValueStorageKeyConcept Key, typename Value, KeyValueStorageDeleterConcept<Value> Deleter>
bool KeyValueStorage<Key, Value, Deleter>::deleteItem(const Key & _key)
{
    auto it = m_map.find(_key);
    if(it == m_map.end())
        return false;
    m_deleter(it->second);
    m_map.erase(it);
    return true;
}

template<KeyValueStorageKeyConcept Key, typename Value, KeyValueStorageDeleterConcept<Value> Deleter>
Value * KeyValueStorage<Key, Value, Deleter>::getItem(const Key & _key)
{
    auto it = m_map.find(_key);
    return it == m_map.end() ? nullptr : it->second;
}

template<KeyValueStorageKeyConcept Key, typename Value, KeyValueStorageDeleterConcept<Value> Deleter>
const Value * KeyValueStorage<Key, Value, Deleter>::getItem(const Key & _key) const
{
    auto it = m_map.find(_key);
    return it == m_map.cend() ? nullptr : it->second;
}

} // namespace Sol2D::Utils
