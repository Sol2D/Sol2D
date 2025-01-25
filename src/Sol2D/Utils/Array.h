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

#include <array>
#include <cstddef>

template<typename T, size_t left_len, size_t right_len>
constexpr std::array<T, left_len + right_len> operator + (std::array<T, left_len> _left, std::array<T, right_len> _right)
{
    std::array<T, left_len + right_len> result;
    std::copy(_right.begin(), _right.end(), std::copy(_left.begin(), _left.end(), result.begin()));
    return result;
}
