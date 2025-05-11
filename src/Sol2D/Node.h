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

#include <Sol2D/Element.h>
#include <Sol2D/Style.h>
#include <Sol2D/Object.h>

struct YGNode;

namespace Sol2D {

class View;

class Node final : public Object
{
private:
    friend class View;

    class ChildrenIteratorBase
    {
    public:
        S2_DEFAULT_COPY(ChildrenIteratorBase);

        virtual ~ChildrenIteratorBase() { }

    protected:
        ChildrenIteratorBase(YGNode * _node, size_t _index, size_t _count) :
            m_node(_node),
            m_index(_index),
            m_count(_count)
        {
        }

        Node * getNode() const;

    protected:
        YGNode * m_node;
        size_t m_index;
        size_t m_count;
    };

public:
    template<typename NodeType>
    class ChildrenIterator : private ChildrenIteratorBase
    {
    public:
        S2_DEFAULT_COPY(ChildrenIterator);

        ChildrenIterator(YGNode * _node, size_t _index, size_t _count) :
            ChildrenIteratorBase(_node, _index, _count)
        {
        }

        ChildrenIterator<NodeType> & operator ++ ()
        {
            ++m_index;
            return *this;
        }

        ChildrenIterator<NodeType> operator ++ (int)
        {
            ChildrenIterator<NodeType> it = *this;
            ++m_index;
            return it;
        }

        ChildrenIterator<NodeType> & operator += (size_t _n)
        {
            m_index += _n;
            return *this;
        }

        ChildrenIterator<NodeType> & operator -- ()
        {
            if(m_index > 0)
                --m_index;
            return *this;
        }

        ChildrenIterator<NodeType> operator -- (int)
        {
            ChildrenIterator<NodeType> it = *this;
            if(m_index > 0)
                --m_index;
            return it;
        }

        ChildrenIterator<NodeType> & operator -= (size_t _n)
        {
            if(m_index < _n)
                m_index = 0;
            else
                m_index -= _n;
            return *this;
        }

        NodeType * operator * () const
        {
            return getNode();
        }

        NodeType & operator -> () const
        {
            return *getNode();
        }

        bool operator == (const ChildrenIterator<NodeType> & _other) const
        {
            return _other.m_node == m_node &&
                (_other.m_index == m_index || (_other.m_index >= _other.m_count && m_index >= m_count));
        }

        bool operator != (const ChildrenIterator<NodeType> & _other) const
        {
            return !(_other == *this);
        }
    };

    class Children
    {
    public:
        explicit Children(YGNode * _node) :
            m_node(_node)
        {
        }

        ChildrenIterator<Node> begin();
        ChildrenIterator<Node> end();

    private:
        YGNode * m_node;
    };

    class ConstChildren
    {
    public:
        explicit ConstChildren(YGNode * _node) :
            m_node(_node)
        {
        }

        ChildrenIterator<const Node> begin() const
        {
            return cbegin();
        }

        ChildrenIterator<const Node> end() const
        {
            return cend();
        }

        ChildrenIterator<const Node> cbegin() const;
        ChildrenIterator<const Node> cend() const;

    private:
        YGNode * m_node;
    };
    
public:
    S2_DISABLE_COPY_AND_MOVE(Node)
    Node(View & _view, Node * _parent, const Style & _style);
    ~Node();
    void setPositionType(Position::Type _type);
    void setPosition(const std::unordered_map<Edge, Position> & _positions);
    void setPosition(Edge _edge, Position _posotion);
    void setMargin(const std::unordered_map<Edge, Dimension> & _margins);
    void setMargin(Edge _edge, const Dimension & _value);
    void setPadding(const std::unordered_map<Edge, Dimension> & _paddings);
    void setPadding(Edge _edge, const Dimension & _value);
    void setWidth(const Dimension & _width);
    void setHeight(const Dimension & _height);
    void setMinWidth(const DimensionLimit & _min_width);
    void setMinHeight(const DimensionLimit & _min_height);
    void setMaxWidth(const DimensionLimit & _max_width);
    void setMaxHeight(const DimensionLimit & _max_height);
    void setFlexBasis(const Dimension & _basis);
    void setFlexGrow(float _grow);
    void setFlexShrink(float _shrink);
    void setFlexDirection(FlexDirection _direction);
    void setFlexWrap(FlexWrap _wrap);
    void setGap(const std::unordered_map<GapGutter, Dimension> & _gaps);
    void setGap(GapGutter _gutter, float _gap);
    void setContentAlignment(ContentAlignment _alignment);
    void setContentJustification(ContentJustification _justification);
    void setItemsAlignment(ItemAlignment _alignment);
    void setSelfAlignment(ItemAlignment _alignment);
    void setAspectRatio(float _ratio);
    void setDisplayMode(DisplayMode _mode);
    Node & addNode(const Style & _style = Style());
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;
    
    ConstChildren getChildren() const
    {
        return ConstChildren(m_node);
    }
    
    Children getChildren()
    {
        return Children(m_node);
    }
    
    const Element * getElement() const
    {
        return m_element.get();
    }
    
    Element * getElement()
    {
        return m_element.get();
    }
    
    void setElement(std::shared_ptr<Element> _element);
    
private:
    YGNode * m_node;
    View & m_view;
    Node * m_parent;
    std::shared_ptr<Element> m_element;
};

} // namespace Sol2D
