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

#include <Sol2D/Lua/LuaLayoutingApi.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>
#include <functional>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

class NodeCompanion : public ObjectCompanion
{
public:
    explicit NodeCompanion(std::function<void()> _on_destroyed) :
        m_on_destroyed(_on_destroyed)
    {
    }

    ~NodeCompanion() override
    {
        m_on_destroyed();
    }

private:
    std::function<void()> m_on_destroyed;
};

struct Self : LuaSelfBase
{
public:
    Self(Node & _node) :
        m_node(&_node)
    {
        auto companion = std::make_unique<NodeCompanion>([this]() {
            m_node = nullptr;
        });
        m_companion_id = _node.addCompanion(std::move(companion));
    }

    void beforeDelete(lua_State *) override
    {
        if(m_node)
            m_node->removeCompanion(m_companion_id);
    }

    Node * getNode(lua_State * _lua)
    {
        if(m_node)
            return m_node;
        luaL_error(_lua, LuaMessage::node_is_destroyed);
        return nullptr;
    }

private:
    Node * m_node;
    uint64_t m_companion_id;
};

using UserData = LuaUserData<Self, LuaTypeName::node>;

namespace Field
{
    namespace Style
    {
        constexpr const char margin[] = "margin";
        constexpr const char padding[] = "padding";
        constexpr const char width[] = "width";
        constexpr const char height[] = "height";
        constexpr const char min_width[] = "minWidth";
        constexpr const char min_height[] = "minHeight";
        constexpr const char max_width[] = "maxWidth";
        constexpr const char max_height[] = "maxHeight";
        constexpr const char flex_basis[] = "flexBasis";
        constexpr const char flex_grow[] = "flexGrow";
        constexpr const char flex_shrink[] = "flexShrink";
        constexpr const char flex_direction[] = "flexDirection";
        constexpr const char flex_wrap[] = "flexWrap";
        constexpr const char content_alignment[] = "contentAlignment";
        constexpr const char content_justification[] = "contentJustification";
        constexpr const char items_alignment[] = "itemsAlignment";
        constexpr const char self_alignment[] = "selfAlignment";
        constexpr const char aspect_ratio[] = "aspectRatio";
        constexpr const char display_mode[] = "displayMode";
        constexpr const char gap[] = "gap";
        constexpr const char position[] = "position";
        constexpr const char position_type[] = "positionType";
    }

    namespace Edge
    {
        constexpr const char left[] = "left";
        constexpr const char top[] = "top";
        constexpr const char right[] = "right";
        constexpr const char bottom[] = "bottom";
        constexpr const char start[] = "start";
        constexpr const char end[] = "end";
        constexpr const char horizontal[] = "horizontal";
        constexpr const char vertical[] = "vertical";
        constexpr const char all[] = "all";
    };

    namespace UnitAndValue
    {
        constexpr const char unit[] = "unit";
        constexpr const char value[] = "value";
    }

    namespace GapGutter
    {
        constexpr const char column[] = "column";
        constexpr const char row[] = "row";
        constexpr const char all[] = "all";
    }
}

static const std::unordered_map<std::string, Edge> gs_edge_map
{
    { Field::Edge::left, Edge::Left },
    { Field::Edge::top, Edge::Top },
    { Field::Edge::right, Edge::Right },
    { Field::Edge::bottom, Edge::Bottom },
    { Field::Edge::start, Edge::Start },
    { Field::Edge::end, Edge::End },
    { Field::Edge::horizontal, Edge::Horizontal },
    { Field::Edge::vertical, Edge::Vertical },
    { Field::Edge::all, Edge::All }
};

static const std::unordered_map<std::string, GapGutter> gs_gap_gutter_map
{
    { Field::GapGutter::column, GapGutter::Column},
    { Field::GapGutter::row, GapGutter::Row },
    { Field::GapGutter::all, GapGutter::All }
};

std::optional<Dimension> tryGetDimension(const LuaTableApi & _table)
{
    Dimension dimension;
    int unit_int;
    if(!_table.isValid() ||
       !_table.tryGetNumber(Field::UnitAndValue::value, &dimension.value) ||
       !_table.tryGetInteger(Field::UnitAndValue::unit, &unit_int))
    {
        return std::nullopt;
    }
    switch(unit_int)
    {
    case static_cast<int>(Dimension::Unit::Auto):
        dimension.unit = Dimension::Unit::Auto;
        break;
    case static_cast<int>(Dimension::Unit::Point):
        dimension.unit = Dimension::Unit::Point;
        break;
    case static_cast<int>(Dimension::Unit::Percent):
        dimension.unit = Dimension::Unit::Percent;
        break;
    case static_cast<int>(Dimension::Unit::MaxContent):
        dimension.unit = Dimension::Unit::MaxContent;
        break;
    case static_cast<int>(Dimension::Unit::FitContent):
        dimension.unit = Dimension::Unit::FitContent;
        break;
    case static_cast<int>(Dimension::Unit::Stretch):
        dimension.unit = Dimension::Unit::Stretch;
        break;
    default:
        return std::nullopt;
    }
    return dimension;
}

std::optional<DimensionLimit> tryGetDimensionLimit(const LuaTableApi & _table)
{
    DimensionLimit limit;
    int unit_int;
    if(!_table.isValid() ||
       !_table.tryGetNumber(Field::UnitAndValue::value, &limit.value) ||
       !_table.tryGetInteger(Field::UnitAndValue::unit, &unit_int))
    {
        return std::nullopt;
    }
    switch(unit_int)
    {
    case static_cast<int>(DimensionLimit::Unit::Point):
        limit.unit = DimensionLimit::Unit::Point;
        break;
    case static_cast<int>(DimensionLimit::Unit::Percent):
        limit.unit = DimensionLimit::Unit::Percent;
        break;
    case static_cast<int>(DimensionLimit::Unit::MaxContent):
        limit.unit = DimensionLimit::Unit::MaxContent;
        break;
    case static_cast<int>(DimensionLimit::Unit::FitContent):
        limit.unit = DimensionLimit::Unit::FitContent;
        break;
    case static_cast<int>(DimensionLimit::Unit::Stretch):
        limit.unit = DimensionLimit::Unit::Stretch;
        break;
    default:
        return std::nullopt;
    }
    return limit;
}

std::optional<Position> tryGetPosition(const LuaTableApi & _table)
{
    Position position;
    int unit_int;
    if(!_table.isValid() ||
       !_table.tryGetNumber(Field::UnitAndValue::value, &position.value) ||
       !_table.tryGetInteger(Field::UnitAndValue::unit, &unit_int))
    {
        return std::nullopt;
    }
    switch(unit_int)
    {
    case static_cast<int>(Position::Unit::Auto):
        position.unit = Position::Unit::Auto;
        break;
    case static_cast<int>(Position::Unit::Point):
        position.unit = Position::Unit::Point;
        break;
    case static_cast<int>(Position::Unit::Percent):
        position.unit = Position::Unit::Percent;
        break;
    default:
        return std::nullopt;
    }
    return position;
}

template<typename Enum, typename Value>
void fillMap(
    const LuaTableApi & _table,
    const std::unordered_map<std::string, Enum> & _enum_map,
    std::unordered_map<Enum, Value> & _out,
    std::optional<Value> (* _get_value)(const LuaTableApi &))
{
    for(const auto & pair : _enum_map)
    {
        if(_table.tryGetTable(pair.first.c_str()))
        {
            LuaTableApi sub_table(_table.getLua());
            if(std::optional<Value> value = _get_value(sub_table))
                _out[pair.second] = value.value();
            lua_pop(_table.getLua(), 1);
        }
    }
}

std::optional<FlexDirection> tryGetFlexDirection(int _value)
{
    switch(_value)
    {
    case static_cast<int>(FlexDirection::Column):
        return FlexDirection::Column;
    case static_cast<int>(FlexDirection::Row):
        return FlexDirection::Row;
    case static_cast<int>(FlexDirection::ColumnReverse):
        return FlexDirection::ColumnReverse;
    case static_cast<int>(FlexDirection::RowReverse):
        return FlexDirection::RowReverse;
    default:
        return std::nullopt;
    }
}

std::optional<FlexWrap> tryGetFlexWrap(int _value)
{
    switch(_value)
    {
    case static_cast<int>(FlexWrap::None):
        return FlexWrap::None;
    case static_cast<int>(FlexWrap::Wrap):
        return FlexWrap::Wrap;
    case static_cast<int>(FlexWrap::WrapReverse):
        return FlexWrap::WrapReverse;
    default:
        return std::nullopt;
    }
}

std::optional<ContentAlignment> tryGetContentAlignment(int _value)
{
    switch(_value)
    {
    case static_cast<int>(ContentAlignment::FlexStart):
        return ContentAlignment::FlexStart;
    case static_cast<int>(ContentAlignment::FlexEnd):
        return ContentAlignment::FlexEnd;
    case static_cast<int>(ContentAlignment::Stretch):
        return ContentAlignment::Stretch;
    case static_cast<int>(ContentAlignment::Center):
        return ContentAlignment::Center;
    case static_cast<int>(ContentAlignment::SpaceBetween):
        return ContentAlignment::SpaceBetween;
    case static_cast<int>(ContentAlignment::SpaceAround):
        return ContentAlignment::SpaceAround;
    default:
        return std::nullopt;
    }
}

std::optional<ContentJustification> tryGetContentJustification(int _value)
{
    switch(_value)
    {
    case static_cast<int>(ContentJustification::FlexStart):
        return ContentJustification::FlexStart;
    case static_cast<int>(ContentJustification::FlexEnd):
        return ContentJustification::FlexEnd;
    case static_cast<int>(ContentJustification::Center):
        return ContentJustification::Center;
    case static_cast<int>(ContentJustification::SpaceBetween):
        return ContentJustification::SpaceBetween;
    case static_cast<int>(ContentJustification::SpaceAround):
        return ContentJustification::SpaceAround;
    case static_cast<int>(ContentJustification::SpaceEvenly):
        return ContentJustification::SpaceEvenly;
    default:
        return std::nullopt;
    }
}

std::optional<ItemAlignment> tryGetItemAlignment(int _value)
{
    switch(_value)
    {
    case static_cast<int>(ItemAlignment::Stretch):
        return ItemAlignment::Stretch;
    case static_cast<int>(ItemAlignment::FlexStart):
        return ItemAlignment::FlexStart;
    case static_cast<int>(ItemAlignment::FlexEnd):
        return ItemAlignment::FlexEnd;
    case static_cast<int>(ItemAlignment::Center):
        return ItemAlignment::Center;
    case static_cast<int>(ItemAlignment::Baseline):
        return ItemAlignment::Baseline;
    default:
        return std::nullopt;
    }
}

std::optional<DisplayMode> tryGetDisplayMode(int _value)
{
    switch(_value)
    {
    case static_cast<int>(DisplayMode::Flex):
        return DisplayMode::Flex;
    case static_cast<int>(DisplayMode::None):
        return DisplayMode::None;
    default:
        return std::nullopt;
    }
}

std::optional<Position::Type> tryGetPositionType(int _value)
{
    switch(_value)
    {
    case static_cast<int>(Position::Type::Relative):
        return Position::Type::Relative;
    case static_cast<int>(Position::Type::Absolute):
        return Position::Type::Absolute;
    case static_cast<int>(Position::Type::Static):
        return Position::Type::Static;
    default:
        return std::nullopt;
    }
}

std::optional<Edge> tryGetEdge(int _value)
{
    switch(_value)
    {
    case static_cast<int>(Edge::Left):
        return Edge::Left;
    case static_cast<int>(Edge::Top):
        return Edge::Top;
    case static_cast<int>(Edge::Right):
        return Edge::Right;
    case static_cast<int>(Edge::Bottom):
        return Edge::Bottom;
    case static_cast<int>(Edge::Start):
        return Edge::Start;
    case static_cast<int>(Edge::End):
        return Edge::End;
    case static_cast<int>(Edge::Horizontal):
        return Edge::Horizontal;
    case static_cast<int>(Edge::Vertical):
        return Edge::Vertical;
    case static_cast<int>(Edge::All):
        return Edge::All;
    default:
        return std::nullopt;
    }
}

std::optional<GapGutter> tryGetGapGutter(int _value)
{
    switch(_value)
    {
    case static_cast<int>(GapGutter::Column):
        return GapGutter::Column;
    case static_cast<int>(GapGutter::Row):
        return GapGutter::Row;
    case static_cast<int>(GapGutter::All):
        return GapGutter::All;
    default:
        return std::nullopt;
    }
}

// 1 self
// 2 type
int luaApi_SetPositionType(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isinteger(_lua, 2), 2, LuaTypeName::integer);
    std::optional<Position::Type> type;
    switch(lua_tointeger(_lua, 2))
    {
    case static_cast<lua_Integer>(Position::Type::Absolute):
        type = Position::Type::Absolute;
        break;
    case static_cast<lua_Integer>(Position::Type::Static):
        type = Position::Type::Static;
        break;
    case static_cast<lua_Integer>(Position::Type::Relative):
        type = Position::Type::Relative;
        break;
    }
    self->getNode(_lua)->setPositionType(type.value());
    return 0;
}

// 1 self     | 1 self
// 2 edge     | 2 position map
// 3 position |
int luaApi_SetPosition(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<Edge> edge = tryGetEdge(lua_tointeger(_lua, 2));
        std::optional<Position> position = tryGetPosition(LuaTableApi(_lua, 3));
        if(edge && position)
        {
            self->getNode(_lua)->setPosition(*edge, *position);
            return 0;
        }
    }
    else
    {
        LuaTableApi position_map_table(_lua, 2);
        if(position_map_table.isValid())
        {
            std::unordered_map<Edge, Position> position_map;
            fillMap(LuaTableApi(_lua), gs_edge_map, position_map, tryGetPosition);
            self->getNode(_lua)->setPosition(position_map);
            return 0;
        }
    }
    luaL_argexpected(_lua, false, 2,  LuaTypeName::joinTypes(LuaTypeName::edge, LuaTypeName::position_map).c_str());
    return 0;
}

// 1 self   | 1 self
// 2 edge   | 2 margin map
// 3 margin |
int luaApi_SetMargin(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<Edge> edge = tryGetEdge(lua_tointeger(_lua, 2));
        std::optional<Dimension> margin = tryGetDimension(LuaTableApi(_lua, 3));
        if(edge && margin)
        {
            self->getNode(_lua)->setMargin(*edge, *margin);
            return 0;
        }
    }
    else
    {
        LuaTableApi margin_map_table(_lua, 2);
        if(margin_map_table.isValid())
        {
            std::unordered_map<Edge, Dimension> margin_map;
            fillMap(LuaTableApi(_lua), gs_edge_map, margin_map, tryGetDimension);
            self->getNode(_lua)->setMargin(margin_map);
            return 0;
        }
    }
    luaL_argexpected(_lua, false, 2,  LuaTypeName::joinTypes(LuaTypeName::edge, LuaTypeName::dimension_map).c_str());
    return 0;
}

// 1 self    | 1 self
// 2 edge    | 2 padding map
// 3 padding |
int luaApi_SetPadding(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<Edge> edge = tryGetEdge(lua_tointeger(_lua, 2));
        std::optional<Dimension> padding = tryGetDimension(LuaTableApi(_lua, 3));
        if(edge && padding)
        {
            self->getNode(_lua)->setPadding(*edge, *padding);
            return 0;
        }
    }
    else
    {
        LuaTableApi padding_map_table(_lua, 2);
        if(padding_map_table.isValid())
        {
            std::unordered_map<Edge, Dimension> padding_map;
            fillMap(LuaTableApi(_lua), gs_edge_map, padding_map, tryGetDimension);
            self->getNode(_lua)->setPadding(padding_map);
            return 0;
        }
    }
    luaL_argexpected(_lua, false, 2,  LuaTypeName::joinTypes(LuaTypeName::edge, LuaTypeName::dimension_map).c_str());
    return 0;
}

// 1 self
// 2 width
int luaApi_SetWidth(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<Dimension> width = tryGetDimension(LuaTableApi(_lua, 2));
    luaL_argexpected(_lua, false, width.has_value(), LuaTypeName::dimension);
    self->getNode(_lua)->setWidth(width.value());
    return 0;
}

// 1 self
// 2 height
int luaApi_SetHeight(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<Dimension> height = tryGetDimension(LuaTableApi(_lua, 2));
    luaL_argexpected(_lua, false, height.has_value(), LuaTypeName::dimension);
    self->getNode(_lua)->setWidth(height.value());
    return 0;
}

// 1 self
// 2 limit
int luaApi_SetMinWidth(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<DimensionLimit> limit = tryGetDimensionLimit(LuaTableApi(_lua, 2));
    luaL_argexpected(_lua, false, limit.has_value(), LuaTypeName::dimension_limit);
    self->getNode(_lua)->setMinWidth(limit.value());
    return 0;
}

// 1 self
// 2 limit
int luaApi_SetMinHeight(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<DimensionLimit> limit = tryGetDimensionLimit(LuaTableApi(_lua, 2));
    luaL_argexpected(_lua, false, limit.has_value(), LuaTypeName::dimension_limit);
    self->getNode(_lua)->setMinHeight(limit.value());
    return 0;
}

// 1 self
// 2 limit
int luaApi_SetMaxWidth(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<DimensionLimit> limit = tryGetDimensionLimit(LuaTableApi(_lua, 2));
    luaL_argexpected(_lua, false, limit.has_value(), LuaTypeName::dimension_limit);
    self->getNode(_lua)->setMaxWidth(limit.value());
    return 0;
}

// 1 self
// 2 limit
int luaApi_SetMaxHeight(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<DimensionLimit> limit = tryGetDimensionLimit(LuaTableApi(_lua, 2));
    luaL_argexpected(_lua, false, limit.has_value(), LuaTypeName::dimension_limit);
    self->getNode(_lua)->setMaxHeight(limit.value());
    return 0;
}

// 1 self
// 2 basis
int luaApi_SetFlexBasis(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<Dimension> basis = tryGetDimension(LuaTableApi(_lua, 2));
    luaL_argexpected(_lua, false, basis.has_value(), LuaTypeName::dimension);
    self->getNode(_lua)->setFlexBasis(basis.value());
    return 0;
}

// 1 self
// 2 grow
int luaApi_SetFlexGrow(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::number);
    self->getNode(_lua)->setFlexGrow(static_cast<float>(lua_tonumber(_lua, 2)));
    return 0;
}

// 1 self
// 2 shrink
int luaApi_SetFlexShrink(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::number);
    self->getNode(_lua)->setFlexShrink(static_cast<float>(lua_tonumber(_lua, 2)));
    return 0;
}

// 1 self
// 2 direction
int luaApi_SetFlexDirection(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<FlexDirection> direction = tryGetFlexDirection(lua_tointeger(_lua, 2));
        if(direction.has_value())
        {
            self->getNode(_lua)->setFlexDirection(direction.value());
            return 0;
        }
    }
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::integer);
    return 0;
}

// 1 self
// 2 wrap
int luaApi_SetFlexWrap(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<FlexWrap> wrap = tryGetFlexWrap(lua_tointeger(_lua, 2));
        if(wrap.has_value())
        {
            self->getNode(_lua)->setFlexWrap(wrap.value());
            return 0;
        }
    }
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::integer);
    return 0;
}

// 1 self       | 1 self
// 2 gap gutter | 2 gap gutter map
// 3 gap        |
int luaApi_SetGap(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<GapGutter> gap_gutter = tryGetGapGutter(lua_tointeger(_lua, 2));
        std::optional<Dimension> gap = tryGetDimension(LuaTableApi(_lua, 3));
        if(gap_gutter && gap)
        {
            self->getNode(_lua)->setGap(*gap_gutter, *gap);
            return 0;
        }
    }
    else
    {
        LuaTableApi gap_gutter_map_table(_lua, 2);
        if(gap_gutter_map_table.isValid())
        {
            std::unordered_map<GapGutter, Dimension> gap_gutter_map;
            fillMap(LuaTableApi(_lua), gs_gap_gutter_map, gap_gutter_map, tryGetDimension);
            self->getNode(_lua)->setGap(gap_gutter_map);
            return 0;
        }
    }
    luaL_argexpected(
        _lua,
        false,
        2,
        LuaTypeName::joinTypes(LuaTypeName::gap_gutter, LuaTypeName::dimension_map).c_str());
    return 0;
}

// 1 self
// 2 alignment
int luaApi_SetContentAlignment(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<ContentAlignment> alignment = tryGetContentAlignment(lua_tointeger(_lua, 2));
        if(alignment.has_value())
        {
            self->getNode(_lua)->setContentAlignment(alignment.value());
            return 0;
        }
    }
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::integer);
    return 0;
}

// 1 self
// 2 justification
int luaApi_SetContentJustification(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<ContentJustification> justification = tryGetContentJustification(lua_tointeger(_lua, 2));
        if(justification.has_value())
        {
            self->getNode(_lua)->setContentJustification(justification.value());
            return 0;
        }
    }
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::integer);
    return 0;
}

// 1 self
// 2 alignment
int luaApi_SetItemsAlignment(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<ItemAlignment> alignment = tryGetItemAlignment(lua_tointeger(_lua, 2));
        if(alignment.has_value())
        {
            self->getNode(_lua)->setItemsAlignment(alignment.value());
            return 0;
        }
    }
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::integer);
    return 0;
}

// 1 self
// 2 alignment
int luaApi_SetSelfAlignment(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<ItemAlignment> alignment = tryGetItemAlignment(lua_tointeger(_lua, 2));
        if(alignment.has_value())
        {
            self->getNode(_lua)->setSelfAlignment(alignment.value());
            return 0;
        }
    }
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::integer);
    return 0;
}

// 1 self
// 2 ratio
int luaApi_SetAspectRatio(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::number);
    self->getNode(_lua)->setAspectRatio(static_cast<float>(lua_tonumber(_lua, 2)));
    return 0;
}

// 1 self
// 2 mode
int luaApi_SetDisplayMode(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    if(lua_isinteger(_lua, 2))
    {
        std::optional<DisplayMode> mode = tryGetDisplayMode(lua_tointeger(_lua, 2));
        if(mode.has_value())
        {
            self->getNode(_lua)->setDisplayMode(mode.value());
            return 0;
        }
    }
    luaL_argexpected(_lua, false, lua_isnumber(_lua, 2), LuaTypeName::integer);
    return 0;
}

// 1 self
// 2 style (optional)
int luaApi_AddNode(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    Style style;
    if(lua_gettop(_lua) > 1)
        luaL_argexpected(_lua, tryGetStyle(_lua, 2, style), 2, LuaTypeName::style);
    Node & node = self->getNode(_lua)->addNode(style);
    pushLayoutNodeApi(_lua, node);
    return 1;
}

// 1 self
int luaApi_GetX(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getNode(_lua)->getX());
    return 1;
}

// 1 self
int luaApi_GetY(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getNode(_lua)->getY());
    return 1;
}

// 1 self
int luaApi_GetWidth(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getNode(_lua)->getWidth());
    return 1;
}

// 1 self
int luaApi_GetHeight(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getNode(_lua)->getHeight());
    return 1;
}

// 1 self
int luaApi_GetChildren(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_newtable(_lua);
    int idx = 0;
    for(Node & node : self->getNode(_lua)->getChildren())
    {
        pushLayoutNodeApi(_lua, node);
        lua_rawseti(_lua, -2, ++idx);
    }
    return 1;
}

} // namespace name

void Sol2D::Lua::pushContentAlignmentEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::content_alignment) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("FLEX_START", static_cast<lua_Integer>(ContentAlignment::FlexStart));
        table.setIntegerValue("FLEX_END", static_cast<lua_Integer>(ContentAlignment::FlexEnd));
        table.setIntegerValue("STRETCH", static_cast<lua_Integer>(ContentAlignment::Stretch));
        table.setIntegerValue("CENTER", static_cast<lua_Integer>(ContentAlignment::Center));
        table.setIntegerValue("SPACE_BETWEEN", static_cast<lua_Integer>(ContentAlignment::SpaceBetween));
        table.setIntegerValue("SPACE_AROUND", static_cast<lua_Integer>(ContentAlignment::SpaceAround));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushContentJustificationEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::content_justification) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("FLEX_START", static_cast<lua_Integer>(ContentJustification::FlexStart));
        table.setIntegerValue("FLEX_END", static_cast<lua_Integer>(ContentJustification::FlexEnd));
        table.setIntegerValue("CENTER", static_cast<lua_Integer>(ContentJustification::Center));
        table.setIntegerValue("SPACE_BETWEEN", static_cast<lua_Integer>(ContentJustification::SpaceBetween));
        table.setIntegerValue("SPACE_AROUND", static_cast<lua_Integer>(ContentJustification::SpaceAround));
        table.setIntegerValue("SPACE_EVENLY", static_cast<lua_Integer>(ContentJustification::SpaceEvenly));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushItemAlignmentEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::item_alignment) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("STRETCH", static_cast<lua_Integer>(ItemAlignment::Stretch));
        table.setIntegerValue("FLEX_START", static_cast<lua_Integer>(ItemAlignment::FlexStart));
        table.setIntegerValue("FLEX_END", static_cast<lua_Integer>(ItemAlignment::FlexEnd));
        table.setIntegerValue("CENTER", static_cast<lua_Integer>(ItemAlignment::Center));
        table.setIntegerValue("BASELINE", static_cast<lua_Integer>(ItemAlignment::Baseline));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushDisplayModeEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::display_mode) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("FLEX", static_cast<lua_Integer>(DisplayMode::Flex));
        table.setIntegerValue("NONE", static_cast<lua_Integer>(DisplayMode::None));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushFlexDirectionEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::flex_direction) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("COLUMN", static_cast<lua_Integer>(FlexDirection::Column));
        table.setIntegerValue("ROW", static_cast<lua_Integer>(FlexDirection::Row));
        table.setIntegerValue("COLUMN_REVERSE", static_cast<lua_Integer>(FlexDirection::ColumnReverse));
        table.setIntegerValue("ROW_REVERSE", static_cast<lua_Integer>(FlexDirection::RowReverse));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushFlexWrapEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::flex_wrap) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("NONE", static_cast<lua_Integer>(FlexWrap::None));
        table.setIntegerValue("WRAP", static_cast<lua_Integer>(FlexWrap::Wrap));
        table.setIntegerValue("WRAP_REVERSE", static_cast<lua_Integer>(FlexWrap::WrapReverse));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushEdgeEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::edge) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("LEFT", static_cast<lua_Integer>(Edge::Left));
        table.setIntegerValue("TOP", static_cast<lua_Integer>(Edge::Top));
        table.setIntegerValue("RIGHT", static_cast<lua_Integer>(Edge::Right));
        table.setIntegerValue("BOTTOM", static_cast<lua_Integer>(Edge::Bottom));
        table.setIntegerValue("START", static_cast<lua_Integer>(Edge::Start));
        table.setIntegerValue("END", static_cast<lua_Integer>(Edge::End));
        table.setIntegerValue("HORIZONTAL", static_cast<lua_Integer>(Edge::Horizontal));
        table.setIntegerValue("VERTICAL", static_cast<lua_Integer>(Edge::Vertical));
        table.setIntegerValue("ALL", static_cast<lua_Integer>(Edge::All));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushGapGutterEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::gap_gutter) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("COLUMN", static_cast<lua_Integer>(GapGutter::Column));
        table.setIntegerValue("ROW", static_cast<lua_Integer>(GapGutter::Row));
        table.setIntegerValue("ALL", static_cast<lua_Integer>(GapGutter::All));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushPositionTypeEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::position_type) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("RELATIVE", static_cast<lua_Integer>(Position::Type::Relative));
        table.setIntegerValue("ABSOLUTE", static_cast<lua_Integer>(Position::Type::Absolute));
        table.setIntegerValue("STATIC", static_cast<lua_Integer>(Position::Type::Static));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushPositionUnitEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::position_unit) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("AUTO", static_cast<lua_Integer>(Position::Unit::Auto));
        table.setIntegerValue("POINT", static_cast<lua_Integer>(Position::Unit::Point));
        table.setIntegerValue("PERCENT", static_cast<lua_Integer>(Position::Unit::Percent));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushDimensionUnitEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::dimension_unit) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("AUTO", static_cast<lua_Integer>(Dimension::Unit::Auto));
        table.setIntegerValue("POINT", static_cast<lua_Integer>(Dimension::Unit::Point));
        table.setIntegerValue("PERCENT", static_cast<lua_Integer>(Dimension::Unit::Percent));
        table.setIntegerValue("MAX_CONTENT", static_cast<lua_Integer>(Dimension::Unit::MaxContent));
        table.setIntegerValue("FIT_CONTENT", static_cast<lua_Integer>(Dimension::Unit::FitContent));
        table.setIntegerValue("STRETCH", static_cast<lua_Integer>(Dimension::Unit::Stretch));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushDimensionLimitUnitEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::dimension_limit_unit) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("POINT", static_cast<lua_Integer>(DimensionLimit::Unit::Point));
        table.setIntegerValue("PERCENT", static_cast<lua_Integer>(DimensionLimit::Unit::Percent));
        table.setIntegerValue("MAX_CONTENT", static_cast<lua_Integer>(DimensionLimit::Unit::MaxContent));
        table.setIntegerValue("FIT_CONTENT", static_cast<lua_Integer>(DimensionLimit::Unit::FitContent));
        table.setIntegerValue("STRETCH", static_cast<lua_Integer>(DimensionLimit::Unit::Stretch));
    }
    lua_setmetatable(_lua, -2);
}

bool Sol2D::Lua::tryGetStyle(lua_State * _lua, int _idx, Style & _style)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return false;
    if(table.tryGetTable(Field::Style::margin))
    {
        fillMap(LuaTableApi(_lua), gs_edge_map, _style.margin, tryGetDimension);
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::padding))
    {
        fillMap(LuaTableApi(_lua), gs_edge_map, _style.padding, tryGetDimension);
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::gap))
    {
        fillMap(LuaTableApi(_lua), gs_gap_gutter_map, _style.gap, tryGetDimension);
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::position))
    {
        fillMap(LuaTableApi(_lua), gs_edge_map, _style.position, tryGetPosition);
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::width))
    {
        _style.width = tryGetDimension(LuaTableApi(_lua));
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::height))
    {
        _style.height = tryGetDimension(LuaTableApi(_lua));
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::min_width))
    {
        _style.min_width = tryGetDimensionLimit(LuaTableApi(_lua));
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::min_height))
    {
        _style.min_height = tryGetDimensionLimit(LuaTableApi(_lua));
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::max_width))
    {
        _style.max_width = tryGetDimensionLimit(LuaTableApi(_lua));
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::max_height))
    {
        _style.max_height = tryGetDimensionLimit(LuaTableApi(_lua));
        lua_pop(_lua, 1);
    }
    if(table.tryGetTable(Field::Style::flex_basis))
    {
        _style.flex_basis = tryGetDimension(LuaTableApi(_lua));
        lua_pop(_lua, 1);
    }
    {
        float f_value;
        if(table.tryGetNumber(Field::Style::flex_grow, &f_value))
            _style.flex_grow = f_value;
        if(table.tryGetNumber(Field::Style::flex_shrink, &f_value))
            _style.flex_shrink = f_value;
        if(table.tryGetNumber(Field::Style::aspect_ratio, &f_value))
            _style.aspect_ratio = f_value;
    }
    {
        int f_int;
        if(table.tryGetInteger(Field::Style::flex_direction, &f_int))
            _style.flex_direction = tryGetFlexDirection(f_int);
        if(table.tryGetInteger(Field::Style::flex_wrap, &f_int))
            _style.flex_wrap = tryGetFlexWrap(f_int);
        if(table.tryGetInteger(Field::Style::content_alignment, &f_int))
            _style.content_alignment = tryGetContentAlignment(f_int);
        if(table.tryGetInteger(Field::Style::content_justification, &f_int))
            _style.content_justification = tryGetContentJustification(f_int);
        if(table.tryGetInteger(Field::Style::items_alignment, &f_int))
            _style.items_alignment = tryGetItemAlignment(f_int);
        if(table.tryGetInteger(Field::Style::self_alignment, &f_int))
            _style.self_alignment = tryGetItemAlignment(f_int);
        if(table.tryGetInteger(Field::Style::display_mode, &f_int))
            _style.display_mode = tryGetDisplayMode(f_int);
        if(table.tryGetInteger(Field::Style::position_type, &f_int))
            _style.position_type = tryGetPositionType(f_int);
    }
    return true;
}

Node * Sol2D::Lua::tryGetNode(lua_State * _lua, int _idx)
{
    if(Self * self = UserData::tryGetUserData(_lua, _idx))
        return self->getNode(_lua);
    return nullptr;
}

void Sol2D::Lua::pushLayoutNodeApi(lua_State * _lua, Node & _node)
{
    UserData::pushUserData(_lua, _node);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "setPositionType", luaApi_SetPositionType },
            { "setPosition", luaApi_SetPosition },
            { "setMargin", luaApi_SetMargin },
            { "setPadding", luaApi_SetPadding },
            { "setWidth", luaApi_SetWidth },
            { "setHeight", luaApi_SetHeight },
            { "setMinWidth", luaApi_SetMinWidth },
            { "setMinHeight", luaApi_SetMinHeight },
            { "setMaxWidth", luaApi_SetMaxWidth },
            { "setMaxHeight", luaApi_SetMaxHeight },
            { "setFlexBasis", luaApi_SetFlexBasis },
            { "setFlexGrow", luaApi_SetFlexGrow },
            { "setFlexShrink", luaApi_SetFlexShrink },
            { "setFlexDirection", luaApi_SetFlexDirection },
            { "setFlexWrap", luaApi_SetFlexWrap },
            { "setGap", luaApi_SetGap },
            { "setContentAlignment", luaApi_SetContentAlignment },
            { "setContentJustification", luaApi_SetContentJustification },
            { "setItemsAlignment", luaApi_SetItemsAlignment },
            { "setSelfAlignment", luaApi_SetSelfAlignment },
            { "setAspectRatio", luaApi_SetAspectRatio },
            { "setDisplayMode", luaApi_SetDisplayMode },
            { "addNode", luaApi_AddNode },
            { "getX", luaApi_GetX },
            { "getY", luaApi_GetY },
            { "getWidth", luaApi_GetWidth },
            { "getHeight", luaApi_GetHeight },
            { "getChildren", luaApi_GetChildren },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
