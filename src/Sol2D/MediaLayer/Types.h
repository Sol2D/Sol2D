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

#include <cstdint>

namespace Sol2D {

struct GPUBool
{
    GPUBool() :
        value(0u)
    {
    }

    GPUBool(bool _value) :
        value(_value ? 1u : 0u)
    {
    }

    void operator = (bool _value)
    {
        value = _value ? 1u : 0u;
    }

    operator bool () const
    {
        return value != 0u;
    }

    uint32_t value;
};

} // namespace Sol2D
