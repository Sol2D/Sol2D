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
#include <atomic>
#include <concepts>

namespace Sol2D::Utils {

template<std::integral T>
class SequentialId
{
public:
    S2_DISABLE_COPY_AND_MOVE(SequentialId)

    explicit SequentialId() :
        m_next_id(1)
    {
    }

    T getNext();

private:
    std::atomic<T> m_next_id;
};

template<std::integral T>
T SequentialId<T>::getNext()
{
    for(;;)
    {
        T id = m_next_id.load(std::memory_order::acquire);
        T next_id = id + 1;
        if(m_next_id.compare_exchange_strong(id, next_id, std::memory_order::release, std::memory_order::relaxed))
            return id;
    }
}

} // namespace Sol2D::Utils
