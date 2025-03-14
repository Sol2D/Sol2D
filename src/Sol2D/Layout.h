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

#include <list>
#include <optional>
#include <unordered_map>

struct YGNode;

namespace Sol2D {

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

class LayoutNode
{
public:
    struct Position
    {
        enum class Type
        {
            Relative,
            Absolute,
            Static
        };

        enum class Units
        {
            Auto,
            Points,
            Percentage
        };

        constexpr Position() :
            value(.0f),
            units(Units::Auto)
        {
        }

        constexpr Position(float _value = .0f, Units _units = Units::Points) :
            value(_value),
            units(_units)
        {
        }

        constexpr Position(const Percentage & _percentage) :
            value(_percentage),
            units(Units::Percentage)
        {
        }

        using Type::Absolute;
        using Type::Relative;
        using Type::Static;

        float value;
        Units units;
    };

    struct Size
    {
        enum class Units
        {
            Auto,
            Points,
            Percentage,
            MaxContent,
            FitContent,
            Stretch
        };

        constexpr Size(Units _units = Units::Auto) :
            value(.0f),
            units(_units)
        {
        }

        constexpr Size(const Percentage _percentage) :
            value(_percentage),
            units(Units::Percentage)
        {
        }

        constexpr Size(float _value = .0f, Units _units = Units::Points) :
            value(_value),
            units(_units)
        {
        }

        using Units::Auto;
        using Units::Points;
        using Units::Percentage;
        using Units::MaxContent;
        using Units::FitContent;
        using Units::Stretch;

        float value;
        Units units;
    };

    struct SizeLimit
    {
        enum class Units
        {
            Points,
            Percentage,
            MaxContent,
            FitContent,
            Stretch
        };

        constexpr SizeLimit(Units _units = Units::FitContent) :
            value(.0f),
            units(_units)
        {
        }

        constexpr SizeLimit(float _value = .0f, Units _units = Units::Points) :
            value(_value),
            units(_units)
        {
        }

        constexpr SizeLimit(const Percentage & _percentage) :
            value(_percentage),
            units(Units::Percentage)
        {
        }

        using Units::Points;
        using Units::Percentage;
        using Units::MaxContent;
        using Units::FitContent;
        using Units::Stretch;

        float value;
        Units units;
    };

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

    struct Style
    {
        std::unordered_map<Edge, float> margin;
        std::unordered_map<Edge, float> padding;
        std::optional<Size> width;
        std::optional<Size> height;
        std::optional<SizeLimit> min_width;
        std::optional<SizeLimit> min_height;
        std::optional<SizeLimit> max_width;
        std::optional<SizeLimit> max_height;
        std::optional<float> flex_basis;
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
        std::unordered_map<GapGutter, float> gap;
        std::unordered_map<Edge, Position> position;
        std::optional<Position::Type> position_type;

        // NOTE: no border
        // NOTE: no direction
    };

    class Children
    {
    public:
        explicit Children(std::list<LayoutNode *> & _children) :
            m_children(_children)
        {
        }

        std::list<LayoutNode *>::iterator begin()
        {
            return m_children.begin();
        }

        std::list<LayoutNode *>::iterator end()
        {
            return m_children.end();
        }

    private:
        std::list<LayoutNode *> & m_children;
    };

    class ConstChildren
    {
    public:
        explicit ConstChildren(const std::list<LayoutNode *> & _children) :
            m_children(_children)
        {
        }

        std::list<LayoutNode *>::const_iterator begin() const
        {
            return m_children.begin();
        }

        std::list<LayoutNode *>::const_iterator end() const
        {
            return m_children.end();
        }

        std::list<LayoutNode *>::const_iterator cbegin() const
        {
            return m_children.cbegin();
        }

        std::list<LayoutNode *>::const_iterator cend() const
        {
            return m_children.cend();
        }

    private:
        const std::list<LayoutNode *> & m_children;
    };

protected:
    LayoutNode(LayoutNode * _parent, const Style & _style);
    LayoutNode(const LayoutNode &) = delete;
    LayoutNode(LayoutNode &&) = delete;
    LayoutNode & operator= (const LayoutNode &) = delete;
    LayoutNode & operator= (LayoutNode &&) = delete;
    virtual void forceRecalculation();

public:
    virtual ~LayoutNode();
    void setPositionType(Position::Type _type);
    void setPosition(const std::unordered_map<Edge, Position> & _positions);
    void setPosition(Edge _edge, Position _posotion);
    void setMargin(const std::unordered_map<Edge, float> & _margins);
    void setMargin(Edge _edge, float _value);
    void setPadding(const std::unordered_map<Edge, float> & _paddings);
    void setPadding(Edge _edge, float _value);
    void setWidth(const Size & _width);
    void setHeight(const Size & _height);
    void setMinWidth(const SizeLimit & _min_width);
    void setMinHeight(const SizeLimit & _min_height);
    void setMaxWidth(const SizeLimit & _max_width);
    void setMaxHeight(const SizeLimit & _max_height);
    void setFlexBasis(float _basis);
    void setFlexGrow(float _grow);
    void setFlexShrink(float _shrink);
    void setFlexDirection(FlexDirection _direction);
    void setFlexWrap(FlexWrap _wrap);
    void setGap(const std::unordered_map<GapGutter, float> & _gaps);
    void setGap(GapGutter _gutter, float _gap);
    void setContentAlignment(ContentAlignment _alignment);
    void setContentJustification(ContentJustification _justification);
    void setItemsAlignment(ItemAlignment _alignment);
    void setSelfAlignment(ItemAlignment _alignment);
    void setAspectRatio(float _ratio);
    void setDisplayMode(DisplayMode _mode);
    LayoutNode & addNode(const Style & _style = Style());
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

    ConstChildren getChildren() const
    {
        return ConstChildren(m_children);
    }

    Children getChildren()
    {
        return Children(m_children);
    }

protected:
    YGNode * m_node;
    LayoutNode * m_parent;
    std::list<LayoutNode *> m_children;
};

class Layout final : public LayoutNode
{
public:
    explicit Layout(const Style & _style = Style());
    void recalculate(float _width, float _height);

protected:
    void forceRecalculation() override;

private:
    float m_calculated_width;
    float m_calculated_height;
    bool m_force_recalculate;
};

} // namespace Sol2D
