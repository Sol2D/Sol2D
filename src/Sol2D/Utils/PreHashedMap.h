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

#include <Sol2D/Def.h>
#include <unordered_map>

namespace Sol2D::Utils {

template<typename Key>
concept PreHashedInnerKeyConcept = requires(Key _key, Key _key2) {
    std::hash<Key>()(_key);
    { _key == _key2 } -> std::same_as<bool>;
};

template<PreHashedInnerKeyConcept InnerKey>
class PreHashedKey
{
public:
    S2_DEFAULT_COPY_AND_MOVE(PreHashedKey)

    explicit PreHashedKey(const InnerKey & _key) :
        key(_key),
        hash(s_hash(key))
    {
    }

    explicit PreHashedKey(InnerKey && _key) :
        key(std::forward<InnerKey>(_key)),
        hash(s_hash(key))
    {
    }

    bool operator== (const PreHashedKey & _key) const
    {
        return key == _key.key;
    }

    InnerKey key;
    size_t hash;

private:
    static std::hash<InnerKey> s_hash;
};

template<PreHashedInnerKeyConcept InnerKey>
inline PreHashedKey<InnerKey> makePreHashedKey(InnerKey && _inner_key)
{
    return PreHashedKey<InnerKey>(std::forward<InnerKey>(_inner_key));
}

template<PreHashedInnerKeyConcept InnerKey>
inline PreHashedKey<InnerKey> makePreHashedKey(const InnerKey & _inner_key)
{
    return PreHashedKey<InnerKey>(_inner_key);
}

template<PreHashedInnerKeyConcept T>
std::hash<T> PreHashedKey<T>::s_hash = std::hash<T>();

template<PreHashedInnerKeyConcept Key>
struct PreHashedKeyHash
{
    size_t operator() (const PreHashedKey<Key> & _key) const
    {
        return _key.hash;
    }
};

template<PreHashedInnerKeyConcept Key, typename Value>
using PreHashedMap = std::unordered_map<PreHashedKey<Key>, Value, PreHashedKeyHash<Key>>;

} // namespace Sol2D::Utils
