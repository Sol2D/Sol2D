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

namespace Sol2D::Utils {

class SequentialId
{
public:
    S2_DISABLE_COPY_AND_MOVE(SequentialId)

    SequentialId() :
        m_next_id(1)
    {
    }

    uint64_t getNext();

private:
    std::atomic_uint64_t m_next_id;
};

} // namespace Sol2D::Utils
