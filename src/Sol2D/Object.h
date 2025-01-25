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

#include <Sol2D/Utils/SequentialId.h>
#include <memory>
#include <unordered_map>

namespace Sol2D {

constexpr uint64_t null_companion_id = 0;

class ObjectCompanion
{
public:
    virtual ~ObjectCompanion() { }
};

class Object
{
public:
    S2_DISABLE_COPY_AND_MOVE(Object)

    Object()
    {
    }

    virtual ~Object() { }

    uint64_t addCompanion(std::unique_ptr<ObjectCompanion> && _companion)
    {
        uint64_t id = m_sequential_id.getNext();
        m_compaions[id] = std::move(_companion);
        return id;
    }

    ObjectCompanion * getCompanion(uint64_t _id)
    {
        auto it = m_compaions.find(_id);
        return it == m_compaions.end() ? nullptr : it->second.get();
    }

    const ObjectCompanion * getCompanion(uint64_t _id) const
    {
        auto it = m_compaions.find(_id);
        return it == m_compaions.cend() ? nullptr : it->second.get();
    }

    bool removeCompanion(uint64_t _id)
    {
        return m_compaions.erase(_id) > 0;
    }

private:
    Utils::SequentialId<uint64_t> m_sequential_id;
    std::unordered_map<uint64_t, std::unique_ptr<ObjectCompanion>> m_compaions;
};

} // namespace Sol2D
