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

#include <unordered_map>
#include <optional>

namespace Sol2D {

enum class ContentAlignment
{
    FlexStart,
    FlexEnd,
    Stretch,
    Center,
    SpaceBetween,
    SpaceAround
};

enum class ContentJustification
{
    FlexStart,
    FlexEnd,
    Center,
    SpaceBetween,
    SpaceAround,
    SpaceEvenly
};

enum class ItemAlignment
{
    Stretch,
    FlexStart,
    FlexEnd,
    Center,
    Baseline
};

enum class DisplayMode
{
    Flex,
    None
};

enum class FlexDirection
{
    Column,
    Row,
    ColumnReverse,
    RowReverse
};

enum class FlexWrap
{
    None,
    Wrap,
    WrapReverse
};

enum class Edge
{
    Left,
    Top,
    Right,
    Bottom,
    Start,
    End,
    Horizontal,
    Vertical,
    All
};

enum class GapGutter
{
    Column,
    Row,
    All
};

struct Percentage
{
    constexpr Percentage(float _value) :
        value(_value)
    {
    }

    constexpr Percentage(const Percentage & _rhs)
    {
        value = _rhs.value;
    }

    constexpr Percentage & operator = (const Percentage & _rhs)
    {
        if(this != &_rhs)
            value = _rhs.value;
        return *this;
    }

    constexpr void operator = (float _value)
    {
        value = _value;
    }

    constexpr operator float () const
    {
        return value;
    }

    float value;
};

struct Position
{
    enum class Type
    {
        Relative,
        Absolute,
        Static
    };

    enum class Unit
    {
        Auto,
        Point,
        Percent
    };

    constexpr Position(Unit _unit, float _value = .0f) :
        value(_value),
        unit(_unit)
    {
    }

    constexpr Position(float _value = .0f, Unit _unit = Unit::Point) :
        value(_value),
        unit(_unit)
    {
    }

    constexpr Position(const Percentage & _percentage) :
        value(_percentage),
        unit(Unit::Percent)
    {
    }

    using Type::Absolute;
    using Type::Relative;
    using Type::Static;

    float value;
    Unit unit;
};

struct Dimension
{
    enum class Unit
    {
        Auto,
        Point,
        Percent,
        MaxContent,
        FitContent,
        Stretch
    };

    constexpr Dimension(Unit _unit = Unit::Auto) :
        value(.0f),
        unit(_unit)
    {
    }

    constexpr Dimension(const Percentage _percentage) :
        value(_percentage),
        unit(Unit::Percent)
    {
    }

    constexpr Dimension(float _value, Unit _units = Unit::Point) :
        value(_value),
        unit(_units)
    {
    }

    using Unit::Auto;
    using Unit::Point;
    using Unit::Percent;
    using Unit::MaxContent;
    using Unit::FitContent;
    using Unit::Stretch;

    float value;
    Unit unit;
};

struct DimensionLimit
{
    enum class Unit
    {
        Point,
        Percent,
        MaxContent,
        FitContent,
        Stretch
    };

    constexpr DimensionLimit(Unit _unit = Unit::FitContent) :
        value(.0f),
        unit(_unit)
    {
    }

    constexpr DimensionLimit(float _value, Unit _unit = Unit::Point) :
        value(_value),
        unit(_unit)
    {
    }

    constexpr DimensionLimit(const Percentage & _percentage) :
        value(_percentage),
        unit(Unit::Percent)
    {
    }

    using Unit::Point;
    using Unit::Percent;
    using Unit::MaxContent;
    using Unit::FitContent;
    using Unit::Stretch;

    float value;
    Unit unit;
};

struct Style
{
    std::unordered_map<Edge, Dimension> margin;
    std::unordered_map<Edge, Dimension> padding;
    std::optional<Dimension> width;
    std::optional<Dimension> height;
    std::optional<DimensionLimit> min_width;
    std::optional<DimensionLimit> min_height;
    std::optional<DimensionLimit> max_width;
    std::optional<DimensionLimit> max_height;
    std::optional<Dimension> flex_basis;
    std::optional<float> flex_grow;
    std::optional<float> flex_shrink;
    std::optional<FlexDirection> flex_direction;
    std::optional<FlexWrap> flex_wrap;
    std::optional<ContentAlignment> content_alignment;
    std::optional<ContentJustification> content_justification;
    std::optional<ItemAlignment> items_alignment;
    std::optional<ItemAlignment> self_alignment;
    std::optional<float> aspect_ratio;
    std::optional<DisplayMode> display_mode;
    std::unordered_map<GapGutter, Dimension> gap;
    std::unordered_map<Edge, Position> position;
    std::optional<Position::Type> position_type;

    // NOTE: no border
    // NOTE: no direction
};

} // namespace Sol2D
