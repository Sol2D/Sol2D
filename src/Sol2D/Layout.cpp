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

#include <Sol2D/Layout.h>
#include <yoga/Yoga.h>

using namespace Sol2D;

namespace {

class YogaNodeWrapper
{
public:
    YogaNodeWrapper(YGNodeRef _node);
    void setPositionType(LayoutNode::Position::Type _type);
    void setPosition(const std::unordered_map<LayoutNode::Edge, LayoutNode::Position> & _positions);
    void setPosition(LayoutNode::Edge _edge, LayoutNode::Position _posotion);
    void setMargin(const std::unordered_map<LayoutNode::Edge, float> & _margins);
    void setMargin(LayoutNode::Edge _edge, float _value);
    void setPadding(const std::unordered_map<LayoutNode::Edge, float> & _paddings);
    void setPadding(LayoutNode::Edge _edge, float _value);
    void setWidth(const LayoutNode::Size & _width);
    void setHeight(const LayoutNode::Size & _height);
    void setMinWidth(const LayoutNode::SizeLimit & _min_width);
    void setMinHeight(const LayoutNode::SizeLimit & _min_height);
    void setMaxWidth(const LayoutNode::SizeLimit & _max_width);
    void setMaxHeight(const LayoutNode::SizeLimit & _max_height);
    void setFlexBasis(float _basis);
    void setFlexGrow(float _grow);
    void setFlexShrink(float _shrink);
    void setFlexDirection(LayoutNode::FlexDirection _direction);
    void setFlexWrap(LayoutNode::FlexWrap _wrap);
    void setGap(const std::unordered_map<LayoutNode::GapGutter, float> & _gaps);
    void setGap(LayoutNode::GapGutter _gutter, float _gap);
    void setContentAlignment(LayoutNode::ContentAlignment _alignment);
    void setContentJustification(LayoutNode::ContentJustification _justification);
    void setItemsAlignment(LayoutNode::ItemAlignment _alignment);
    void setSelfAlignment(LayoutNode::ItemAlignment _alignment);
    void setAspectRatio(float _ratio);
    void setDisplayMode(LayoutNode::DisplayMode _mode);

private:
    static YGEdge edgeToYGEdge(LayoutNode::Edge _edge);
    static YGFlexDirection flexDirectionToYGFlexDirection(LayoutNode::FlexDirection _fd);
    static YGWrap flexWrapToYGWrap(LayoutNode::FlexWrap _wrap);
    static YGAlign contentAlignmentToYGAlign(LayoutNode::ContentAlignment _alignment);
    static YGAlign itemAlignmentToYGAlign(LayoutNode::ItemAlignment _alignment);
    static YGJustify contentJustificationToYGJustify(LayoutNode::ContentJustification _justification);
    static YGDisplay displayModeToYGDisplay(LayoutNode::DisplayMode _mode);
    static YGGutter gapGutterToYGGutter(LayoutNode::GapGutter _gutter);
    static YGPositionType positionTypeToYGPositionType(LayoutNode::Position::Type _type);

private:
    YGNodeRef m_node;
};

} // namespace

inline YogaNodeWrapper::YogaNodeWrapper(YGNodeRef _node) :
    m_node(_node)
{
}

inline void YogaNodeWrapper::setPositionType(LayoutNode::Position::Type _type)
{
    YGNodeStyleSetPositionType(m_node, positionTypeToYGPositionType(_type));
}

inline void YogaNodeWrapper::setPosition(
    const std::unordered_map<LayoutNode::Edge, LayoutNode::Position> & _positions)
{
    for(const auto & position_pair : _positions)
        setPosition(position_pair.first, position_pair.second);
}

inline void YogaNodeWrapper::setPosition(LayoutNode::Edge _edge, LayoutNode::Position _posotion)
{
    switch(_posotion.units)
    {
    case LayoutNode::Position::Units::Auto:
        YGNodeStyleSetPositionAuto(m_node, edgeToYGEdge(_edge));
        break;
    case LayoutNode::Position::Units::Points:
        YGNodeStyleSetPosition(m_node, edgeToYGEdge(_edge), _posotion.value);
        break;
    case LayoutNode::Position::Units::Percentage:
        YGNodeStyleSetPositionPercent(m_node, edgeToYGEdge(_edge), _posotion.value);
        break;
    }
}

inline void YogaNodeWrapper::setMargin(const std::unordered_map<LayoutNode::Edge, float> & _margins)
{
    for(const auto margin_pair : _margins)
        setMargin(margin_pair.first, margin_pair.second);
}

inline void YogaNodeWrapper::setMargin(LayoutNode::Edge _edge, float _value)
{
    YGNodeStyleSetMargin(m_node, edgeToYGEdge(_edge), _value);
}

inline void YogaNodeWrapper::setPadding(const std::unordered_map<LayoutNode::Edge, float> & _paddings)
{
    for(const auto padding_pair : _paddings)
        setPadding(padding_pair.first, padding_pair.second);
}

inline void YogaNodeWrapper::setPadding(LayoutNode::Edge _edge, float _value)
{
    YGNodeStyleSetPadding(m_node, edgeToYGEdge(_edge), _value);
}

inline void YogaNodeWrapper::setWidth(const LayoutNode::Size & _width)
{
    switch(_width.units)
    {
    case LayoutNode::Size::Units::Points:
        YGNodeStyleSetWidth(m_node, _width.value);
        break;
    case LayoutNode::Size::Units::Auto:
        YGNodeStyleSetWidthAuto(m_node);
        break;
    case LayoutNode::Size::Units::Percentage:
        YGNodeStyleSetWidthPercent(m_node, _width.value);
        break;
    case LayoutNode::Size::Units::MaxContent:
        YGNodeStyleSetWidthMaxContent(m_node);
        break;
    case LayoutNode::Size::Units::FitContent:
        YGNodeStyleSetWidthFitContent(m_node);
        break;
    case LayoutNode::Size::Units::Stretch:
        YGNodeStyleSetWidthStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setHeight(const LayoutNode::Size & _height)
{
    switch(_height.units)
    {
    case LayoutNode::Size::Units::Points:
        YGNodeStyleSetHeight(m_node, _height.value);
        break;
    case LayoutNode::Size::Units::Auto:
        YGNodeStyleSetHeightAuto(m_node);
        break;
    case LayoutNode::Size::Units::Percentage:
        YGNodeStyleSetHeightPercent(m_node, _height.value);
        break;
    case LayoutNode::Size::Units::MaxContent:
        YGNodeStyleSetHeightMaxContent(m_node);
        break;
    case LayoutNode::Size::Units::FitContent:
        YGNodeStyleSetHeightFitContent(m_node);
        break;
    case LayoutNode::Size::Units::Stretch:
        YGNodeStyleSetHeightStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setMinWidth(const LayoutNode::SizeLimit & _min_width)
{
    switch(_min_width.units)
    {
    case LayoutNode::SizeLimit::Units::Points:
        YGNodeStyleSetMinWidth(m_node, _min_width.value);
        break;
    case LayoutNode::SizeLimit::Units::Percentage:
        YGNodeStyleSetMinWidthPercent(m_node, _min_width.value);
        break;
    case LayoutNode::SizeLimit::Units::MaxContent:
        YGNodeStyleSetMinWidthMaxContent(m_node);
        break;
    case LayoutNode::SizeLimit::Units::FitContent:
        YGNodeStyleSetMinWidthFitContent(m_node);
        break;
    case LayoutNode::SizeLimit::Units::Stretch:
        YGNodeStyleSetMinWidthStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setMinHeight(const LayoutNode::SizeLimit & _min_height)
{
    switch(_min_height.units)
    {
    case LayoutNode::SizeLimit::Units::Points:
        YGNodeStyleSetMinHeight(m_node, _min_height.value);
        break;
        break;
    case LayoutNode::SizeLimit::Units::Percentage:
        YGNodeStyleSetMinHeightPercent(m_node, _min_height.value);
        break;
    case LayoutNode::SizeLimit::Units::MaxContent:
        YGNodeStyleSetMinHeightMaxContent(m_node);
        break;
    case LayoutNode::SizeLimit::Units::FitContent:
        YGNodeStyleSetMinHeightFitContent(m_node);
        break;
    case LayoutNode::SizeLimit::Units::Stretch:
        YGNodeStyleSetMinHeightStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setMaxWidth(const LayoutNode::SizeLimit & _max_width)
{
    switch(_max_width.units)
    {
    case LayoutNode::SizeLimit::Units::Points:
        YGNodeStyleSetMaxWidth(m_node, _max_width.value);
        break;
    case LayoutNode::SizeLimit::Units::Percentage:
        YGNodeStyleSetMaxWidthPercent(m_node, _max_width.value);
        break;
    case LayoutNode::SizeLimit::Units::MaxContent:
        YGNodeStyleSetMaxWidthMaxContent(m_node);
        break;
    case LayoutNode::SizeLimit::Units::FitContent:
        YGNodeStyleSetMaxWidthFitContent(m_node);
        break;
    case LayoutNode::SizeLimit::Units::Stretch:
        YGNodeStyleSetMaxWidthStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setMaxHeight(const LayoutNode::SizeLimit & _max_height)
{
    switch(_max_height.units)
    {
    case LayoutNode::SizeLimit::Units::Points:
        YGNodeStyleSetMaxHeight(m_node, _max_height.value);
        break;
        break;
    case LayoutNode::SizeLimit::Units::Percentage:
        YGNodeStyleSetMaxHeightPercent(m_node, _max_height.value);
        break;
    case LayoutNode::SizeLimit::Units::MaxContent:
        YGNodeStyleSetMaxHeightMaxContent(m_node);
        break;
    case LayoutNode::SizeLimit::Units::FitContent:
        YGNodeStyleSetMaxHeightFitContent(m_node);
        break;
    case LayoutNode::SizeLimit::Units::Stretch:
        YGNodeStyleSetMaxHeightStretch(m_node);
        break;
    }
}

inline void YogaNodeWrapper::setFlexBasis(float _basis)
{
    YGNodeStyleSetFlexBasis(m_node, _basis);
}

inline void YogaNodeWrapper::setFlexGrow(float _grow)
{
    YGNodeStyleSetFlexGrow(m_node, _grow);
}

inline void YogaNodeWrapper::setFlexShrink(float _shrink)
{
    YGNodeStyleSetFlexShrink(m_node, _shrink);
}

inline void YogaNodeWrapper::setFlexDirection(LayoutNode::FlexDirection _direction)
{
    YGNodeStyleSetFlexDirection(m_node, flexDirectionToYGFlexDirection(_direction));
}

inline void YogaNodeWrapper::setFlexWrap(LayoutNode::FlexWrap _wrap)
{
    YGNodeStyleSetFlexWrap(m_node, flexWrapToYGWrap(_wrap));
}

inline void YogaNodeWrapper::setGap(const std::unordered_map<LayoutNode::GapGutter, float> & _gaps)
{
    for(const auto & gap_pair : _gaps)
        setGap(gap_pair.first, gap_pair.second);
}

inline void YogaNodeWrapper::setGap(LayoutNode::GapGutter _gutter, float _gap)
{
    YGNodeStyleSetGap(m_node, gapGutterToYGGutter(_gutter), _gap);
}

inline void YogaNodeWrapper::setContentAlignment(LayoutNode::ContentAlignment _alignment)
{
    YGNodeStyleSetAlignContent(m_node, contentAlignmentToYGAlign(_alignment));
}

inline void YogaNodeWrapper::setContentJustification(LayoutNode::ContentJustification _justification)
{
    YGNodeStyleSetJustifyContent(m_node, contentJustificationToYGJustify(_justification));
}

inline void YogaNodeWrapper::setItemsAlignment(LayoutNode::ItemAlignment _alignment)
{
    YGNodeStyleSetAlignItems(m_node, itemAlignmentToYGAlign(_alignment));
}

inline void YogaNodeWrapper::setSelfAlignment(LayoutNode::ItemAlignment _alignment)
{
    YGNodeStyleSetAlignSelf(m_node, itemAlignmentToYGAlign(_alignment));
}

inline void YogaNodeWrapper::setAspectRatio(float _ratio)
{
    YGNodeStyleSetAspectRatio(m_node, _ratio);
}

inline void YogaNodeWrapper::setDisplayMode(LayoutNode::DisplayMode _mode)
{
    YGNodeStyleSetDisplay(m_node, displayModeToYGDisplay(_mode));
}

YGEdge YogaNodeWrapper::edgeToYGEdge(LayoutNode::Edge _edge)
{
    switch(_edge)
    {
    case LayoutNode::Edge::Left:
        return YGEdgeLeft;
    case LayoutNode::Edge::Top:
        return YGEdgeTop;
    case LayoutNode::Edge::Right:
        return YGEdgeRight;
    case LayoutNode::Edge::Bottom:
        return YGEdgeBottom;
    case LayoutNode::Edge::Start:
        return YGEdgeStart;
    case LayoutNode::Edge::End:
        return YGEdgeEnd;
    case LayoutNode::Edge::Horizontal:
        return YGEdgeHorizontal;
    case LayoutNode::Edge::Vertical:
        return YGEdgeVertical;
    case LayoutNode::Edge::All:
        return YGEdgeAll;
    default:
        return YGEdgeLeft;
    }
}

YGFlexDirection YogaNodeWrapper::flexDirectionToYGFlexDirection(LayoutNode::FlexDirection _fd)
{
    switch(_fd)
    {
    case LayoutNode::FlexDirection::Column:
        return YGFlexDirectionColumn;
    case LayoutNode::FlexDirection::Row:
        return YGFlexDirectionRow;
    case LayoutNode::FlexDirection::ColumnReverse:
        return YGFlexDirectionColumnReverse;
    case LayoutNode::FlexDirection::RowReverse:
        return YGFlexDirectionRowReverse;
    default:
        return YGFlexDirectionColumn;
    }
}

YGWrap YogaNodeWrapper::flexWrapToYGWrap(LayoutNode::FlexWrap _wrap)
{
    switch(_wrap)
    {
    case LayoutNode::FlexWrap::None:
        return YGWrapNoWrap;
    case LayoutNode::FlexWrap::Wrap:
        return YGWrapWrap;
    case LayoutNode::FlexWrap::WrapReverse:
        return YGWrapWrapReverse;
    default:
        return YGWrapNoWrap;
    }
}

YGAlign YogaNodeWrapper::contentAlignmentToYGAlign(LayoutNode::ContentAlignment _alignment)
{
    switch(_alignment)
    {
    case LayoutNode::ContentAlignment::FlexStart:
        return YGAlignFlexStart;
    case LayoutNode::ContentAlignment::FlexEnd:
        return YGAlignFlexEnd;
    case LayoutNode::ContentAlignment::Stretch:
        return YGAlignStretch;
    case LayoutNode::ContentAlignment::Center:
        return YGAlignCenter;
    case LayoutNode::ContentAlignment::SpaceBetween:
        return YGAlignSpaceBetween;
    case LayoutNode::ContentAlignment::SpaceAround:
        return YGAlignSpaceAround;
    default:
        return YGAlignFlexStart;
    }
}

YGAlign YogaNodeWrapper::itemAlignmentToYGAlign(LayoutNode::ItemAlignment _alignment)
{
    switch(_alignment)
    {
    case LayoutNode::ItemAlignment::Stretch:
        return YGAlignStretch;
    case LayoutNode::ItemAlignment::FlexStart:
        return YGAlignFlexStart;
    case LayoutNode::ItemAlignment::FlexEnd:
        return YGAlignFlexEnd;
    case LayoutNode::ItemAlignment::Center:
        return YGAlignCenter;
    case LayoutNode::ItemAlignment::Baseline:
        return YGAlignBaseline;
    default:
        return YGAlignStretch;
    }
}

YGJustify YogaNodeWrapper::contentJustificationToYGJustify(LayoutNode::ContentJustification _justification)
{
    switch(_justification)
    {
    case LayoutNode::ContentJustification::FlexStart:
        return YGJustifyFlexStart;
    case LayoutNode::ContentJustification::FlexEnd:
        return YGJustifyFlexEnd;
    case LayoutNode::ContentJustification::Center:
        return YGJustifyCenter;
    case LayoutNode::ContentJustification::SpaceBetween:
        return YGJustifySpaceBetween;
    case LayoutNode::ContentJustification::SpaceAround:
        return YGJustifySpaceAround;
    case LayoutNode::ContentJustification::SpaceEvenly:
        return YGJustifySpaceEvenly;
    default:
        return YGJustifyFlexStart;
    }
}

YGDisplay YogaNodeWrapper::displayModeToYGDisplay(LayoutNode::DisplayMode _mode)
{
    switch(_mode)
    {
    case LayoutNode::DisplayMode::Flex:
        return YGDisplayFlex;
    case LayoutNode::DisplayMode::None:
        return YGDisplayNone;
    default:
        return YGDisplayFlex;
    }
}

YGGutter YogaNodeWrapper::gapGutterToYGGutter(LayoutNode::GapGutter _gutter)
{
    switch(_gutter)
    {
    case LayoutNode::GapGutter::Column:
        return YGGutterColumn;
    case LayoutNode::GapGutter::Row:
        return YGGutterRow;
    case LayoutNode::GapGutter::All:
        return YGGutterAll;
    default:
        return YGGutterAll;
    }
}

YGPositionType YogaNodeWrapper::positionTypeToYGPositionType(LayoutNode::Position::Type _type)
{
    switch(_type)
    {
    case LayoutNode::Position::Type::Relative:
        return YGPositionTypeRelative;
    case LayoutNode::Position::Type::Absolute:
        return YGPositionTypeAbsolute;
    case LayoutNode::Position::Type::Static:
        return YGPositionTypeStatic;
    default:
        return YGPositionTypeRelative;
    }
}

LayoutNode::LayoutNode(LayoutNode * _parent, const Style & _style) :
    m_node(YGNodeNew()),
    m_parent(_parent)
{
    if(m_parent)
        YGNodeInsertChild(m_parent->m_node, m_node, YGNodeGetChildCount(m_parent->m_node));

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

LayoutNode::~LayoutNode()
{
    for(auto * child : m_children)
        delete child;
    YGNodeFree(m_node);
}

void LayoutNode::setPositionType(Position::Type _type)
{
    YogaNodeWrapper(m_node).setPositionType(_type);
    forceRecalculation();
}

void LayoutNode::setPosition(
    const std::unordered_map<Edge, Position> & _positions)
{
    YogaNodeWrapper(m_node).setPosition(_positions);
    forceRecalculation();
}

void LayoutNode::setPosition(Edge _edge, Position _posotion)
{
    YogaNodeWrapper(m_node).setPosition(_edge, _posotion);
    forceRecalculation();
}

void LayoutNode::setMargin(const std::unordered_map<Edge, float> & _margins)
{
    YogaNodeWrapper(m_node).setMargin(_margins);
    forceRecalculation();
}

void LayoutNode::setMargin(Edge _edge, float _value)
{
    YogaNodeWrapper(m_node).setMargin(_edge, _value);
    forceRecalculation();
}

void LayoutNode::setPadding(const std::unordered_map<Edge, float> & _paddings)
{
    YogaNodeWrapper(m_node).setPadding(_paddings);
    forceRecalculation();
}

void LayoutNode::setPadding(Edge _edge, float _value)
{
    YogaNodeWrapper(m_node).setPadding(_edge, _value);
    forceRecalculation();
}

void LayoutNode::setWidth(const Size & _width)
{
    YogaNodeWrapper(m_node).setWidth(_width);
    forceRecalculation();
}

void LayoutNode::setHeight(const Size & _height)
{
    YogaNodeWrapper(m_node).setHeight(_height);
    forceRecalculation();
}

void LayoutNode::setMinWidth(const SizeLimit & _min_width)
{
    YogaNodeWrapper(m_node).setMinWidth(_min_width);
    forceRecalculation();
}

void LayoutNode::setMinHeight(const SizeLimit & _min_height)
{
    YogaNodeWrapper(m_node).setMinHeight(_min_height);
    forceRecalculation();
}

void LayoutNode::setMaxWidth(const SizeLimit & _max_width)
{
    YogaNodeWrapper(m_node).setMaxWidth(_max_width);
    forceRecalculation();
}

void LayoutNode::setMaxHeight(const SizeLimit & _max_height)
{
    YogaNodeWrapper(m_node).setMaxHeight(_max_height);
    forceRecalculation();
}

void LayoutNode::setFlexBasis(float _basis)
{
    YogaNodeWrapper(m_node).setFlexBasis(_basis);
    forceRecalculation();
}

void LayoutNode::setFlexGrow(float _grow)
{
    YogaNodeWrapper(m_node).setFlexGrow(_grow);
    forceRecalculation();
}

void LayoutNode::setFlexShrink(float _shrink)
{
    YogaNodeWrapper(m_node).setFlexShrink(_shrink);
    forceRecalculation();
}

void LayoutNode::setFlexDirection(FlexDirection _direction)
{
    YogaNodeWrapper(m_node).setFlexDirection(_direction);
    forceRecalculation();
}

void LayoutNode::setFlexWrap(FlexWrap _wrap)
{
    YogaNodeWrapper(m_node).setFlexWrap(_wrap);
    forceRecalculation();
}

void LayoutNode::setGap(const std::unordered_map<GapGutter, float> & _gaps)
{
    YogaNodeWrapper(m_node).setGap(_gaps);
    forceRecalculation();
}

void LayoutNode::setGap(GapGutter _gutter, float _gap)
{
    YogaNodeWrapper(m_node).setGap(_gutter, _gap);
    forceRecalculation();
}

void LayoutNode::setContentAlignment(ContentAlignment _alignment)
{
    YogaNodeWrapper(m_node).setContentAlignment(_alignment);
    forceRecalculation();
}

void LayoutNode::setContentJustification(ContentJustification _justification)
{
    YogaNodeWrapper(m_node).setContentJustification(_justification);
    forceRecalculation();
}

void LayoutNode::setItemsAlignment(ItemAlignment _alignment)
{
    YogaNodeWrapper(m_node).setItemsAlignment(_alignment);
    forceRecalculation();
}

void LayoutNode::setSelfAlignment(ItemAlignment _alignment)
{
    YogaNodeWrapper(m_node).setSelfAlignment(_alignment);
    forceRecalculation();
}

void LayoutNode::setAspectRatio(float _ratio)
{
    YogaNodeWrapper(m_node).setAspectRatio(_ratio);
    forceRecalculation();
}

void LayoutNode::setDisplayMode(DisplayMode _mode)
{
    YogaNodeWrapper(m_node).setDisplayMode(_mode);
    forceRecalculation();
}

LayoutNode & LayoutNode::addNode(const Style & _style)
{
    LayoutNode * node = new LayoutNode(this, _style);
    m_children.push_back(node);
    forceRecalculation();
    return *node;
}

void LayoutNode::forceRecalculation()
{
    if(m_parent)
        m_parent->forceRecalculation();
}

float LayoutNode::getX() const
{
    float x = YGNodeLayoutGetLeft(m_node);
    if(m_parent && YGNodeStyleGetPositionType(m_node) != YGPositionTypeAbsolute)
        x += m_parent->getX();
    return x;
}

float LayoutNode::getY() const
{
    float y = YGNodeLayoutGetTop(m_node);
    if(m_parent && YGNodeStyleGetPositionType(m_node) != YGPositionTypeAbsolute)
        y += m_parent->getY();
    return y;
}

float LayoutNode::getWidth() const
{
    return YGNodeLayoutGetWidth(m_node);
}

float LayoutNode::getHeight() const
{
    return YGNodeLayoutGetHeight(m_node);
}

Layout::Layout(const Style & _style) :
    LayoutNode(nullptr, _style)
{
}

void Layout::forceRecalculation()
{
    m_force_recalculate = true;
}

void Layout::recalculate(float _width, float _height)
{
    if(m_force_recalculate || m_calculated_width != _width || m_calculated_height != _height)
    {
        YGNodeCalculateLayout(m_node, _width, _height, YGDirectionLTR);
        m_force_recalculate = false;
        m_calculated_width = _width;
        m_calculated_height = _height;
    }
}
