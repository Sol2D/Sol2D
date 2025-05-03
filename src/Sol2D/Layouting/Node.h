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

#include <Sol2D/Layouting/Element.h>
#include <Sol2D/Layouting/Style.h>
#include <list>

struct YGNode;

namespace Sol2D::Layouting {

class Node
{
public:
    class Children
    {
    public:
        explicit Children(std::list<Node *> & _children) :
            m_children(_children)
        {
        }
        
        std::list<Node *>::iterator begin()
        {
            return m_children.begin();
        }
        
        std::list<Node *>::iterator end()
        {
            return m_children.end();
        }
        
    private:
        std::list<Node *> & m_children;
    };
    
    class ConstChildren
    {
    public:
        explicit ConstChildren(const std::list<Node *> & _children) :
            m_children(_children)
        {
        }
        
        std::list<Node *>::const_iterator begin() const
        {
            return m_children.begin();
        }
        
        std::list<Node *>::const_iterator end() const
        {
            return m_children.end();
        }
        
        std::list<Node *>::const_iterator cbegin() const
        {
            return m_children.cbegin();
        }
        
        std::list<Node *>::const_iterator cend() const
        {
            return m_children.cend();
        }
        
    private:
        const std::list<Node *> & m_children;
    };
    
protected:
    Node(Node * _parent, const Style & _style);
    Node(const Node &) = delete;
    Node(Node &&) = delete;
    Node & operator= (const Node &) = delete;
    Node & operator= (Node &&) = delete;
    virtual void forceRecalculation();
    
public:
    virtual ~Node();
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
        return ConstChildren(m_children);
    }
    
    Children getChildren()
    {
        return Children(m_children);
    }
    
    const Element * getElement() const
    {
        return m_element.get();
    }
    
    Element * getElement()
    {
        return m_element.get();
    }
    
    void setElement(std::unique_ptr<Element> && _element);
    void step(const StepState & _step);
    
protected:
    YGNode * m_node;
    Node * m_parent;
    std::list<Node *> m_children;
    std::unique_ptr<Element> m_element;
};

} // namespace Sol2D::Layouting
