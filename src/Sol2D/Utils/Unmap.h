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

#include <iterator>

namespace Sol2D::Utils {

template<typename Map, typename InnerIterator>
class UnmapIterator
{
    struct MapTraits
    {
        using MapType = Map;
        using ValueType = Map::mapped_type;
    };

    struct ConstMapTraits
    {
        using MapType = const Map;
        using ValueType = const Map::mapped_type;
    };

    using TraitsType =
        std::conditional<std::is_same<InnerIterator, typename Map::iterator>::value, MapTraits, ConstMapTraits>::type;

public:
    using iterator = UnmapIterator<Map, InnerIterator>;
    using value_type = TraitsType::ValueType;
    using reference = TraitsType::ValueType &;
    using pointer = TraitsType::ValueType *;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = ptrdiff_t;
    using MapType = TraitsType::MapType;

    UnmapIterator(MapType & _map, const InnerIterator & _iterator) :
        m_map(_map),
        m_iterator(_iterator)
    {
    }

    iterator operator++ ()
    {
        ++m_iterator;
        return *this;
    }

    iterator operator++ (int)
    {
        UnmapIterator it(m_map, m_iterator);
        ++m_iterator;
        return it;
    }

    reference operator* ()
    {
        return m_iterator->second;
    }

    pointer operator->()
    {
        return &m_iterator->second;
    }

    bool operator== (const iterator & _rhs)
    {
        return m_iterator == _rhs.m_iterator;
    }

    bool operator!= (const iterator & _rhs)
    {
        return m_iterator != _rhs.m_iterator;
    }

private:
    MapType & m_map;
    InnerIterator m_iterator;
};

template<typename Map>
class Unmap
{
public:
    using Iterator = UnmapIterator<Map, typename Map::iterator>;

    Unmap(Map & _map) :
        m_map(_map)
    {
    }
    Iterator begin()
    {
        return Iterator(m_map, m_map.begin());
    }
    Iterator end()
    {
        return Iterator(m_map, m_map.end());
    }

private:
    Map & m_map;
};

template<typename Map>
class ConstUnmap
{
public:
    using Iterator = UnmapIterator<Map, typename Map::const_iterator>;

    ConstUnmap(const Map & _map) :
        m_map(_map)
    {
    }
    Iterator begin() const
    {
        return cbegin();
    }
    Iterator end() const
    {
        return cend();
    }
    Iterator cbegin() const
    {
        return Iterator(m_map, m_map.cbegin());
    }
    Iterator cend() const
    {
        return Iterator(m_map, m_map.cend());
    }

private:
    const Map & m_map;
};

} // namespace Sol2D::Utils
