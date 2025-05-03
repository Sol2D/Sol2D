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
#include <concepts>

namespace Sol2D {

enum class DimensionUnit
{
    Pixel,
    Percent
};

template<typename T>
concept DimensionValueConcept = std::integral<T> || std::floating_point<T>;

template<DimensionValueConcept Number>
struct Dimension_Obsolete
{
    S2_DEFAULT_COPY_AND_MOVE(Dimension_Obsolete)

    // cppcheck-suppress noExplicitConstructor
    Dimension_Obsolete(Number _pixels) :
        Dimension_Obsolete(_pixels, DimensionUnit::Pixel)
    {
    }

    Dimension_Obsolete(Number _value, DimensionUnit _unit) :
        value(_value),
        unit(_unit)
    {
    }

    Dimension_Obsolete & operator= (Number _pixels)
    {
        value = _pixels;
        unit = DimensionUnit::Pixel;
        return *this;
    }

    static Dimension_Obsolete<Number> fromPixels(Number _pixels)
    {
        return Dimension_Obsolete<Number>(_pixels, DimensionUnit::Pixel);
    }

    static Dimension_Obsolete<Number> fromPercents(Number _percents)
    {
        return Dimension_Obsolete<Number>(_percents, DimensionUnit::Percent);
    }

    float getPixels(Number _length) const
    {
        return unit == DimensionUnit::Pixel ? value : static_cast<float>((_length / 100.0f) * value);
    }

    Number value;
    DimensionUnit unit;
};

} // namespace Sol2D
