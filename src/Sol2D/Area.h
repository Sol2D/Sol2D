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

#include <Sol2D/MediaLayer/Size.h>
#include <Sol2D/Dimension.h>
#include <Sol2D/Def.h>
#include <SDL3/SDL_rect.h>
#include <cstdint>
#include <optional>

namespace Sol2D {

struct Area
{
    S2_DEFAULT_COPY_AND_MOVE(Area)

    Area() :
        z_index(0),
        is_visible(true)
    {
    }

    SDL_FRect calculateRect(const FSize & _output_size) const;

    std::optional<Dimension_Obsolete<int32_t>> top;
    std::optional<Dimension_Obsolete<int32_t>> right;
    std::optional<Dimension_Obsolete<int32_t>> left;
    std::optional<Dimension_Obsolete<int32_t>> bottom;
    std::optional<Dimension_Obsolete<uint32_t>> width;
    std::optional<Dimension_Obsolete<uint32_t>> height;
    uint16_t z_index;
    bool is_visible;
};

} // namespace Sol2D
