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

#include <Sol2D/Node.h>
#include <Sol2D/View.h>
#include <yoga/Yoga.h>

using namespace Sol2D;

namespace {

class YogaNodeWrapper
{
public:
    YogaNodeWrapper(YGNodeRef _node);
    void setPositionType(Position::Type _type);
    void setPosition(const std::unordered_map<Edge, Position> & _positions);
    void setPosition(Edge _edge, Position _posotion);
    void setMargin(const std::unordered_map<Edge, Dimension> & _margins);
    void setMargin(Edge _edge, const Dimension & _value);
    void setPadding(const std::unordered_map<Edge, Dimension> & _paddings);
    void setPadding(Edge _edge, Dimension _value);
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
    void setGap(GapGutter _gutter, const Dimension & _gap);
    void setContentAlignment(ContentAlignment _alignment);
    void setContentJustification(ContentJustification _justification);
    void setItemsAlignment(ItemAlignment _alignment);
    void setSelfAlignment(ItemAlignment _alignment);
    void setAspectRatio(float _ratio);
    void setDisplayMode(DisplayMode _mode);

private:
    static YGEdge edgeToYGEdge(Edge _edge);
    static YGFlexDirection flexDirectionToYGFlexDirection(FlexDirection _fd);
    static YGWrap flexWrapToYGWrap(FlexWrap _wrap);
    static YGAlign contentAlignmentToYGAlign(ContentAlignment _alignment);
    static YGAlign itemAlignmentToYGAlign(ItemAlignment _alignment);
    static YGJustify contentJustificationToYGJustify(ContentJustification _justification);
    static YGDisplay displayModeToYGDisplay(DisplayMode _mode);
    static YGGutter gapGutterToYGGutter(GapGutter _gutter);
    static YGPositionType positionTypeToYGPositionType(Position::Type _type);

private:
    YGNodeRef m_node;
};

// TODO: Layouting: delete?
// YGSize yogaMeasure(
//     YGNodeConstRef _node,
//     float _width,
//     YGMeasureMode _width_mode,
//     float _height,
//     YGMeasureMode _height_mode)
// {
//     const Element * element = static_cast<const LayoutNode *>(YGNodeGetContext(_node))->getElement();
//     if(!element)
//         return { .width = _width, .height = _height };
//     const FSize * size = element->getDesiredSize();
//     if(!size)
//         return { .width = _width, .height = _height };
//     YGSize result { .width = size->w, .height = size->h };
//     switch(_width_mode)
//     {
//     case YGMeasureModeExactly:
//         result.width = _width;
//         break;
//     case YGMeasureModeAtMost:
//         if(result.width > _width)
//             result.width = _width;
//     default:
//         break;
//     }
//     switch(_height_mode)
//     {
//     case YGMeasureModeExactly:
//         result.height = _height;
//         break;
//     case YGMeasureModeAtMost:
//         if(result.height > _height)
//             result.height = _height;
//     default:
//         break;
//     }
//     return result;
// }

} // namespace

inline YogaNodeWrapper::YogaNodeWrapper(YGNodeRef _node) :
    m_node(_node)
{
}

inline void YogaNodeWrapper::setPositionType(Position::Type _type)
{
    YGNodeStyleSetPositionType(m_node, positionTypeToYGPositionType(_type));
}

inline void YogaNodeWrapper::setPosition(
    const std::unordered_map<Edge, Position> & _positions)
{
    for(const auto & position_pair : _positions)
        setPosition(position_pair.first, position_pair.second);
}

inline void YogaNodeWrapper::setPosition(Edge _edge, Position _posotion)
{
    switch(_posotion.unit)
    {
    case Position::Unit::Auto:
        YGNodeStyleSetPositionAuto(m_node, edgeToYGEdge(_edge));
        break;
    case Position::Unit::Point:
        YGNodeStyleSetPosition(m_node, edgeToYGEdge(_edge), _posotion.value);
        break;
    case Position::Unit::Percent:
        YGNodeStyleSetPositionPercent(m_node, edgeToYGEdge(_edge), _posotion.value);
        break;
    }
}

inline void YogaNodeWrapper::setMargin(const std::unordered_map<Edge, Dimension> & _margins)
{
    for(const auto & margin_pair : _margins)
        setMargin(margin_pair.first, margin_pair.second);
}

inline void YogaNodeWrapper::setMargin(Edge _edge, const Dimension & _value)
{
    switch(_value.unit)
    {
    case Dimension::Unit::Auto:
        YGNodeStyleSetMarginAuto(m_node, edgeToYGEdge(_edge));
        break;
    case Dimension::Unit::Point:
        YGNodeStyleSetMargin(m_node, edgeToYGEdge(_edge), _value.value);
        break;
    case Dimension::Unit::Percent:
        YGNodeStyleSetMarginPercent(m_node, edgeToYGEdge(_edge), _value.value);
        break;
    default:
        break;
    }
}

inline void YogaNodeWrapper::setPadding(const std::unordered_map<Edge, Dimension> & _paddings)
{
    for(const auto & padding_pair : _paddings)
        setPadding(padding_pair.first, padding_pair.second);
}

inline void YogaNodeWrapper::setPadding(Edge _edge, Dimension _value)
{
    switch(_value.unit)
    {
    case Dimension::Unit::Point:
        YGNodeStyleSetPadding(m_node, edgeToYGEdge(_edge), _value.value);
        break;
    case Dimension::Unit::Percent:
        YGNodeStyleSetPaddingPercent(m_node, edgeToYGEdge(_edge), _value.value);
        break;
    default:
        break;
    }
}

inline void YogaNodeWrapper::setWidth(const Dimension & _width)
{
    switch(_width.unit)
    {
    case Dimension::Unit::Point:
        YGNodeStyleSetWidth(m_node, _width.value);
        break;
    case Dimension::Unit::Auto:
        YGNodeStyleSetWidthAuto(m_node);
        break;
    case Dimension::Unit::Percent:
        YGNodeStyleSetWidthPercent(m_node, _width.value);
        break;
    case Dimension::Unit::MaxContent:
        YGNodeStyleSetWidthMaxContent(m_node);
        break;
    case Dimension::Unit::FitContent:
        YGNodeStyleSetWidthFitContent(m_node);
        break;
    case Dimension::Unit::Stretch:
        YGNodeStyleSetWidthStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setHeight(const Dimension & _height)
{
    switch(_height.unit)
    {
    case Dimension::Unit::Point:
        YGNodeStyleSetHeight(m_node, _height.value);
        break;
    case Dimension::Unit::Auto:
        YGNodeStyleSetHeightAuto(m_node);
        break;
    case Dimension::Unit::Percent:
        YGNodeStyleSetHeightPercent(m_node, _height.value);
        break;
    case Dimension::Unit::MaxContent:
        YGNodeStyleSetHeightMaxContent(m_node);
        break;
    case Dimension::Unit::FitContent:
        YGNodeStyleSetHeightFitContent(m_node);
        break;
    case Dimension::Unit::Stretch:
        YGNodeStyleSetHeightStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setMinWidth(const DimensionLimit & _min_width)
{
    switch(_min_width.unit)
    {
    case DimensionLimit::Unit::Point:
        YGNodeStyleSetMinWidth(m_node, _min_width.value);
        break;
    case DimensionLimit::Unit::Percent:
        YGNodeStyleSetMinWidthPercent(m_node, _min_width.value);
        break;
    case DimensionLimit::Unit::MaxContent:
        YGNodeStyleSetMinWidthMaxContent(m_node);
        break;
    case DimensionLimit::Unit::FitContent:
        YGNodeStyleSetMinWidthFitContent(m_node);
        break;
    case DimensionLimit::Unit::Stretch:
        YGNodeStyleSetMinWidthStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setMinHeight(const DimensionLimit & _min_height)
{
    switch(_min_height.unit)
    {
    case DimensionLimit::Unit::Point:
        YGNodeStyleSetMinHeight(m_node, _min_height.value);
        break;
        break;
    case DimensionLimit::Unit::Percent:
        YGNodeStyleSetMinHeightPercent(m_node, _min_height.value);
        break;
    case DimensionLimit::Unit::MaxContent:
        YGNodeStyleSetMinHeightMaxContent(m_node);
        break;
    case DimensionLimit::Unit::FitContent:
        YGNodeStyleSetMinHeightFitContent(m_node);
        break;
    case DimensionLimit::Unit::Stretch:
        YGNodeStyleSetMinHeightStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setMaxWidth(const DimensionLimit & _max_width)
{
    switch(_max_width.unit)
    {
    case DimensionLimit::Unit::Point:
        YGNodeStyleSetMaxWidth(m_node, _max_width.value);
        break;
    case DimensionLimit::Unit::Percent:
        YGNodeStyleSetMaxWidthPercent(m_node, _max_width.value);
        break;
    case DimensionLimit::Unit::MaxContent:
        YGNodeStyleSetMaxWidthMaxContent(m_node);
        break;
    case DimensionLimit::Unit::FitContent:
        YGNodeStyleSetMaxWidthFitContent(m_node);
        break;
    case DimensionLimit::Unit::Stretch:
        YGNodeStyleSetMaxWidthStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setMaxHeight(const DimensionLimit & _max_height)
{
    switch(_max_height.unit)
    {
    case DimensionLimit::Unit::Point:
        YGNodeStyleSetMaxHeight(m_node, _max_height.value);
        break;
        break;
    case DimensionLimit::Unit::Percent:
        YGNodeStyleSetMaxHeightPercent(m_node, _max_height.value);
        break;
    case DimensionLimit::Unit::MaxContent:
        YGNodeStyleSetMaxHeightMaxContent(m_node);
        break;
    case DimensionLimit::Unit::FitContent:
        YGNodeStyleSetMaxHeightFitContent(m_node);
        break;
    case DimensionLimit::Unit::Stretch:
        YGNodeStyleSetMaxHeightStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setFlexBasis(const Dimension & _basis)
{
    switch(_basis.unit)
    {
    case Dimension::Unit::Auto:
        YGNodeStyleSetFlexBasisAuto(m_node);
        break;
    case Dimension::Unit::Point:
        YGNodeStyleSetFlexBasis(m_node, _basis.value);
        break;
    case Dimension::Unit::Percent:
        YGNodeStyleSetFlexBasisPercent(m_node, _basis.value);
        break;
    case Dimension::Unit::MaxContent:
        YGNodeStyleSetFlexBasisMaxContent(m_node);
        break;
    case Dimension::Unit::FitContent:
        YGNodeStyleSetFlexBasisFitContent(m_node);
        break;
    case Dimension::Unit::Stretch:
        YGNodeStyleSetFlexBasisStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setFlexGrow(float _grow)
{
    YGNodeStyleSetFlexGrow(m_node, _grow);
}

inline void YogaNodeWrapper::setFlexShrink(float _shrink)
{
    YGNodeStyleSetFlexShrink(m_node, _shrink);
}

inline void YogaNodeWrapper::setFlexDirection(FlexDirection _direction)
{
    YGNodeStyleSetFlexDirection(m_node, flexDirectionToYGFlexDirection(_direction));
}

inline void YogaNodeWrapper::setFlexWrap(FlexWrap _wrap)
{
    YGNodeStyleSetFlexWrap(m_node, flexWrapToYGWrap(_wrap));
}

inline void YogaNodeWrapper::setGap(const std::unordered_map<GapGutter, Dimension> & _gaps)
{
    for(const auto & gap_pair : _gaps)
        setGap(gap_pair.first, gap_pair.second);
}

inline void YogaNodeWrapper::setGap(GapGutter _gutter, const Dimension & _gap)
{
    switch(_gap.unit)
    {
    case Dimension::Unit::Point:
        YGNodeStyleSetGap(m_node, gapGutterToYGGutter(_gutter), _gap.value);
        break;
    case Dimension::Unit::Percent:
        YGNodeStyleSetGapPercent(m_node, gapGutterToYGGutter(_gutter), _gap.value);
        break;
    default:
        break;
    }
}

inline void YogaNodeWrapper::setContentAlignment(ContentAlignment _alignment)
{
    YGNodeStyleSetAlignContent(m_node, contentAlignmentToYGAlign(_alignment));
}

inline void YogaNodeWrapper::setContentJustification(ContentJustification _justification)
{
    YGNodeStyleSetJustifyContent(m_node, contentJustificationToYGJustify(_justification));
}

inline void YogaNodeWrapper::setItemsAlignment(ItemAlignment _alignment)
{
    YGNodeStyleSetAlignItems(m_node, itemAlignmentToYGAlign(_alignment));
}

inline void YogaNodeWrapper::setSelfAlignment(ItemAlignment _alignment)
{
    YGNodeStyleSetAlignSelf(m_node, itemAlignmentToYGAlign(_alignment));
}

inline void YogaNodeWrapper::setAspectRatio(float _ratio)
{
    YGNodeStyleSetAspectRatio(m_node, _ratio);
}

inline void YogaNodeWrapper::setDisplayMode(DisplayMode _mode)
{
    YGNodeStyleSetDisplay(m_node, displayModeToYGDisplay(_mode));
}

YGEdge YogaNodeWrapper::edgeToYGEdge(Edge _edge)
{
    switch(_edge)
    {
    case Edge::Left:
        return YGEdgeLeft;
    case Edge::Top:
        return YGEdgeTop;
    case Edge::Right:
        return YGEdgeRight;
    case Edge::Bottom:
        return YGEdgeBottom;
    case Edge::Start:
        return YGEdgeStart;
    case Edge::End:
        return YGEdgeEnd;
    case Edge::Horizontal:
        return YGEdgeHorizontal;
    case Edge::Vertical:
        return YGEdgeVertical;
    case Edge::All:
        return YGEdgeAll;
    default:
        return YGEdgeLeft;
    }
}

YGFlexDirection YogaNodeWrapper::flexDirectionToYGFlexDirection(FlexDirection _fd)
{
    switch(_fd)
    {
    case FlexDirection::Column:
        return YGFlexDirectionColumn;
    case FlexDirection::Row:
        return YGFlexDirectionRow;
    case FlexDirection::ColumnReverse:
        return YGFlexDirectionColumnReverse;
    case FlexDirection::RowReverse:
        return YGFlexDirectionRowReverse;
    default:
        return YGFlexDirectionColumn;
    }
}

YGWrap YogaNodeWrapper::flexWrapToYGWrap(FlexWrap _wrap)
{
    switch(_wrap)
    {
    case FlexWrap::None:
        return YGWrapNoWrap;
    case FlexWrap::Wrap:
        return YGWrapWrap;
    case FlexWrap::WrapReverse:
        return YGWrapWrapReverse;
    default:
        return YGWrapNoWrap;
    }
}

YGAlign YogaNodeWrapper::contentAlignmentToYGAlign(ContentAlignment _alignment)
{
    switch(_alignment)
    {
    case ContentAlignment::FlexStart:
        return YGAlignFlexStart;
    case ContentAlignment::FlexEnd:
        return YGAlignFlexEnd;
    case ContentAlignment::Stretch:
        return YGAlignStretch;
    case ContentAlignment::Center:
        return YGAlignCenter;
    case ContentAlignment::SpaceBetween:
        return YGAlignSpaceBetween;
    case ContentAlignment::SpaceAround:
        return YGAlignSpaceAround;
    default:
        return YGAlignFlexStart;
    }
}

YGAlign YogaNodeWrapper::itemAlignmentToYGAlign(ItemAlignment _alignment)
{
    switch(_alignment)
    {
    case ItemAlignment::Stretch:
        return YGAlignStretch;
    case ItemAlignment::FlexStart:
        return YGAlignFlexStart;
    case ItemAlignment::FlexEnd:
        return YGAlignFlexEnd;
    case ItemAlignment::Center:
        return YGAlignCenter;
    case ItemAlignment::Baseline:
        return YGAlignBaseline;
    default:
        return YGAlignStretch;
    }
}

YGJustify YogaNodeWrapper::contentJustificationToYGJustify(ContentJustification _justification)
{
    switch(_justification)
    {
    case ContentJustification::FlexStart:
        return YGJustifyFlexStart;
    case ContentJustification::FlexEnd:
        return YGJustifyFlexEnd;
    case ContentJustification::Center:
        return YGJustifyCenter;
    case ContentJustification::SpaceBetween:
        return YGJustifySpaceBetween;
    case ContentJustification::SpaceAround:
        return YGJustifySpaceAround;
    case ContentJustification::SpaceEvenly:
        return YGJustifySpaceEvenly;
    default:
        return YGJustifyFlexStart;
    }
}

YGDisplay YogaNodeWrapper::displayModeToYGDisplay(DisplayMode _mode)
{
    switch(_mode)
    {
    case DisplayMode::Flex:
        return YGDisplayFlex;
    case DisplayMode::None:
        return YGDisplayNone;
    default:
        return YGDisplayFlex;
    }
}

YGGutter YogaNodeWrapper::gapGutterToYGGutter(GapGutter _gutter)
{
    switch(_gutter)
    {
    case GapGutter::Column:
        return YGGutterColumn;
    case GapGutter::Row:
        return YGGutterRow;
    case GapGutter::All:
        return YGGutterAll;
    default:
        return YGGutterAll;
    }
}

YGPositionType YogaNodeWrapper::positionTypeToYGPositionType(Position::Type _type)
{
    switch(_type)
    {
    case Position::Type::Relative:
        return YGPositionTypeRelative;
    case Position::Type::Absolute:
        return YGPositionTypeAbsolute;
    case Position::Type::Static:
        return YGPositionTypeStatic;
    default:
        return YGPositionTypeRelative;
    }
}

Node::Node(View & _view, Node * _parent, const Style & _style) :
    m_node(YGNodeNew()),
    m_view(_view),
    m_parent(_parent)
{
    if(m_parent)
        YGNodeInsertChild(m_parent->m_node, m_node, YGNodeGetChildCount(m_parent->m_node));
    YGNodeSetContext(m_node, this);
    // YGNodeSetMeasureFunc(m_node, yogaMeasure); // TODO: Layouting: delete?


    YogaNodeWrapper wrapper(m_node);
    wrapper.setPosition(_style.position);
    wrapper.setMargin(_style.margin);
    wrapper.setPadding(_style.padding);
    wrapper.setGap(_style.gap);
    if(_style.position_type.has_value())
        wrapper.setPositionType(_style.position_type.value());
    if(_style.width.has_value())
        wrapper.setWidth(_style.width.value());
    if(_style.height.has_value())
        wrapper.setHeight(_style.height.value());
    if(_style.min_width.has_value())
        wrapper.setMinWidth(_style.min_width.value());
    if(_style.min_height.has_value())
        wrapper.setMinHeight(_style.min_height.value());
    if(_style.max_width.has_value())
        wrapper.setMaxWidth(_style.max_width.value());
    if(_style.max_height.has_value())
        wrapper.setMaxHeight(_style.max_height.value());
    if(_style.flex_basis.has_value())
        wrapper.setFlexBasis(_style.flex_basis.value());
    if(_style.flex_grow.has_value())
        wrapper.setFlexGrow(_style.flex_grow.value());
    if(_style.flex_shrink.has_value())
        wrapper.setFlexShrink(_style.flex_shrink.value());
    if(_style.flex_direction.has_value())
        wrapper.setFlexDirection(_style.flex_direction.value());
    if(_style.flex_wrap.has_value())
        wrapper.setFlexWrap(_style.flex_wrap.value());
    if(_style.content_alignment.has_value())
        wrapper.setContentAlignment(_style.content_alignment.value());
    if(_style.content_justification.has_value())
        wrapper.setContentJustification(_style.content_justification.value());
    if(_style.items_alignment.has_value())
        wrapper.setItemsAlignment(_style.items_alignment.value());
    if(_style.self_alignment.has_value())
        wrapper.setSelfAlignment(_style.self_alignment.value());
    if(_style.aspect_ratio.has_value())
        wrapper.setAspectRatio(_style.aspect_ratio.value());
    if(_style.display_mode.has_value())
        wrapper.setDisplayMode(_style.display_mode.value());
}

Node::~Node()
{
    if(m_element)
        m_view.unregisterElement(*m_element);
    for(auto * child : m_children)
        delete child;
    YGNodeFree(m_node);
}

void Node::setPositionType(Position::Type _type)
{
    YogaNodeWrapper(m_node).setPositionType(_type);
    m_view.forceRecalculation();
}

void Node::setPosition(const std::unordered_map<Edge, Position> & _positions)
{
    YogaNodeWrapper(m_node).setPosition(_positions);
    m_view.forceRecalculation();
}

void Node::setPosition(Edge _edge, Position _posotion)
{
    YogaNodeWrapper(m_node).setPosition(_edge, _posotion);
    m_view.forceRecalculation();
}

void Node::setMargin(const std::unordered_map<Edge, Dimension> & _margins)
{
    YogaNodeWrapper(m_node).setMargin(_margins);
    m_view.forceRecalculation();
}

void Node::setMargin(Edge _edge, const Dimension & _value)
{
    YogaNodeWrapper(m_node).setMargin(_edge, _value);
    m_view.forceRecalculation();
}

void Node::setPadding(const std::unordered_map<Edge, Dimension> & _paddings)
{
    YogaNodeWrapper(m_node).setPadding(_paddings);
    m_view.forceRecalculation();
}

void Node::setPadding(Edge _edge, const Dimension & _value)
{
    YogaNodeWrapper(m_node).setPadding(_edge, _value);
    m_view.forceRecalculation();
}

void Node::setWidth(const Dimension & _width)
{
    YogaNodeWrapper(m_node).setWidth(_width);
    m_view.forceRecalculation();
}

void Node::setHeight(const Dimension & _height)
{
    YogaNodeWrapper(m_node).setHeight(_height);
    m_view.forceRecalculation();
}

void Node::setMinWidth(const DimensionLimit & _min_width)
{
    YogaNodeWrapper(m_node).setMinWidth(_min_width);
    m_view.forceRecalculation();
}

void Node::setMinHeight(const DimensionLimit & _min_height)
{
    YogaNodeWrapper(m_node).setMinHeight(_min_height);
    m_view.forceRecalculation();
}

void Node::setMaxWidth(const DimensionLimit & _max_width)
{
    YogaNodeWrapper(m_node).setMaxWidth(_max_width);
    m_view.forceRecalculation();
}

void Node::setMaxHeight(const DimensionLimit & _max_height)
{
    YogaNodeWrapper(m_node).setMaxHeight(_max_height);
    m_view.forceRecalculation();
}

void Node::setFlexBasis(const Dimension & _basis)
{
    YogaNodeWrapper(m_node).setFlexBasis(_basis);
    m_view.forceRecalculation();
}

void Node::setFlexGrow(float _grow)
{
    YogaNodeWrapper(m_node).setFlexGrow(_grow);
    m_view.forceRecalculation();
}

void Node::setFlexShrink(float _shrink)
{
    YogaNodeWrapper(m_node).setFlexShrink(_shrink);
    m_view.forceRecalculation();
}

void Node::setFlexDirection(FlexDirection _direction)
{
    YogaNodeWrapper(m_node).setFlexDirection(_direction);
    m_view.forceRecalculation();
}

void Node::setFlexWrap(FlexWrap _wrap)
{
    YogaNodeWrapper(m_node).setFlexWrap(_wrap);
    m_view.forceRecalculation();
}

void Node::setGap(const std::unordered_map<GapGutter, Dimension> & _gaps)
{
    YogaNodeWrapper(m_node).setGap(_gaps);
    m_view.forceRecalculation();
}

void Node::setGap(GapGutter _gutter, float _gap)
{
    YogaNodeWrapper(m_node).setGap(_gutter, _gap);
    m_view.forceRecalculation();
}

void Node::setContentAlignment(ContentAlignment _alignment)
{
    YogaNodeWrapper(m_node).setContentAlignment(_alignment);
    m_view.forceRecalculation();
}

void Node::setContentJustification(ContentJustification _justification)
{
    YogaNodeWrapper(m_node).setContentJustification(_justification);
    m_view.forceRecalculation();
}

void Node::setItemsAlignment(ItemAlignment _alignment)
{
    YogaNodeWrapper(m_node).setItemsAlignment(_alignment);
    m_view.forceRecalculation();
}

void Node::setSelfAlignment(ItemAlignment _alignment)
{
    YogaNodeWrapper(m_node).setSelfAlignment(_alignment);
    m_view.forceRecalculation();
}

void Node::setAspectRatio(float _ratio)
{
    YogaNodeWrapper(m_node).setAspectRatio(_ratio);
    m_view.forceRecalculation();
}

void Node::setDisplayMode(DisplayMode _mode)
{
    YogaNodeWrapper(m_node).setDisplayMode(_mode);
    m_view.forceRecalculation();
}

Node & Node::addNode(const Style & _style)
{
    Node * node = new Node(m_view, this, _style);
    m_children.push_back(node);
    m_view.forceRecalculation();
    return *node;
}

float Node::getX() const
{
    float x = YGNodeLayoutGetLeft(m_node);
    if(m_parent && YGNodeStyleGetPositionType(m_node) != YGPositionTypeAbsolute)
        x += m_parent->getX();
    return x;
}

float Node::getY() const
{
    float y = YGNodeLayoutGetTop(m_node);
    if(m_parent && YGNodeStyleGetPositionType(m_node) != YGPositionTypeAbsolute)
        y += m_parent->getY();
    return y;
}

float Node::getWidth() const
{
    return YGNodeLayoutGetWidth(m_node);
}

float Node::getHeight() const
{
    return YGNodeLayoutGetHeight(m_node);
}

void Node::setElement(std::shared_ptr<Element> _element)
{
    if(m_element)
        m_view.unregisterElement(*m_element);
    m_element = _element;
    if(m_element)
        m_view.registerElement(*m_element);
    m_view.forceRecalculation();
}
