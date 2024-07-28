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

#include <memory>
#include <string>
#include <unordered_map>

namespace Sol2D {

class ObjectCompanion
{
public:
    virtual ~ObjectCompanion() { }
};

class Object
{
public:
    virtual ~Object()
    {
    }

    void addCompanion(const std::string & _key, std::unique_ptr<ObjectCompanion> && _companion)
    {
        m_compaions[_key] = std::move(_companion);
    }

    ObjectCompanion * getCompanion(const std::string & _key)
    {
        auto it = m_compaions.find(_key);
        return it == m_compaions.end() ? nullptr : it->second.get();
    }

    const ObjectCompanion * getCompanion(const std::string & _key) const
    {
        auto it = m_compaions.find(_key);
        return it == m_compaions.cend() ? nullptr : it->second.get();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ObjectCompanion>> m_compaions;
};

} // namespace Sol2D
