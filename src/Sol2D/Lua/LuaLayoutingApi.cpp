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
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    Self(std::shared_ptr<Node> & _node) :
        node(_node)
    {
    }

    std::shared_ptr<Node> getNode(lua_State * _lua) const
    {
        std::shared_ptr<Node> ptr = node.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::node_is_destroyed);
        return ptr;
    }

    std::weak_ptr<Node> node;
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

template<typename Unit>
class UnitAndValue
{
public:
    using Table = LuaTable<
        LuaTableField<Field::UnitAndValue::unit, Unit>,
        LuaTableField<Field::UnitAndValue::value, float>
    >;

    explicit UnitAndValue(const Table & _table) :
        m_table(_table)
    {
    }

    template<typename Agg>
    std::optional<Agg> aggregate() const
    {
        auto unit = m_table.template tryGetValue<Field::UnitAndValue::unit>();
        auto value = m_table.template tryGetValue<Field::UnitAndValue::value>();
        if(unit && value)
            return Agg(*value, *unit);
        return std::nullopt;
    }

private:
    const Table & m_table;
};

class GapGutterMap
{
public:
    using Table = LuaTable<
        LuaTableField<Field::GapGutter::column, UnitAndValue<Dimension::Unit>::Table>,
        LuaTableField<Field::GapGutter::row, UnitAndValue<Dimension::Unit>::Table>,
        LuaTableField<Field::GapGutter::all, UnitAndValue<Dimension::Unit>::Table>
    >;

    explicit GapGutterMap(const Table & _table) :
        m_table(_table)
    {
    }

    void read(std::unordered_map<GapGutter, Dimension> & _dest_map) const
    {
        readGutter<Field::GapGutter::column>(_dest_map, GapGutter::Column);
        readGutter<Field::GapGutter::row>(_dest_map, GapGutter::Row);
        readGutter<Field::GapGutter::all>(_dest_map, GapGutter::All);
    }

    template<const char name[]>
    void readGutter(std::unordered_map<GapGutter, Dimension> & _dest_map, GapGutter _gutter) const
    {
        if(auto field = m_table.template tryGetValue<name>())
        {
            if(auto value = UnitAndValue<Dimension::Unit>(*field).aggregate<Dimension>())
                _dest_map[_gutter] = *value;
        }
    }

private:
    const Table & m_table;
};

template<typename Unit>
class EdgeMap
{
public:
    using Table = LuaTable<
        LuaTableField<Field::Edge::left, typename UnitAndValue<Unit>::Table>,
        LuaTableField<Field::Edge::top, typename UnitAndValue<Unit>::Table>,
        LuaTableField<Field::Edge::right, typename UnitAndValue<Unit>::Table>,
        LuaTableField<Field::Edge::bottom, typename UnitAndValue<Unit>::Table>,
        LuaTableField<Field::Edge::start, typename UnitAndValue<Unit>::Table>,
        LuaTableField<Field::Edge::end, typename UnitAndValue<Unit>::Table>,
        LuaTableField<Field::Edge::horizontal, typename UnitAndValue<Unit>::Table>,
        LuaTableField<Field::Edge::vertical, typename UnitAndValue<Unit>::Table>,
        LuaTableField<Field::Edge::all, typename UnitAndValue<Unit>::Table>
    >;

    explicit EdgeMap(const Table & _table) :
        m_table(_table)
    {
    }

    template<typename Value>
    void read(std::unordered_map<Edge, Value> & _dest_map) const
    {
        readEdge<Field::Edge::left, Value>(_dest_map, Edge::Left);
        readEdge<Field::Edge::top, Value>(_dest_map, Edge::Top);
        readEdge<Field::Edge::right, Value>(_dest_map, Edge::Right);
        readEdge<Field::Edge::bottom, Value>(_dest_map, Edge::Bottom);
        readEdge<Field::Edge::start, Value>(_dest_map, Edge::Start);
        readEdge<Field::Edge::end, Value>(_dest_map, Edge::End);
        readEdge<Field::Edge::horizontal, Value>(_dest_map, Edge::Horizontal);
        readEdge<Field::Edge::vertical, Value>(_dest_map, Edge::Vertical);
        readEdge<Field::Edge::all, Value>(_dest_map, Edge::All);
    }

    template<const char name[], typename Value>
    void readEdge(std::unordered_map<Edge, Value> & _dest_map, Edge _edge) const
    {
        if(auto field = m_table.template tryGetValue<name>())
        {
            if(auto value = UnitAndValue<Unit>(*field).template aggregate<Value>())
                _dest_map[_edge] = *value;
        }
    }

private:
    const Table & m_table;
};

using StyleTable = LuaTable<
    LuaTableField<Field::Style::margin, EdgeMap<Dimension::Unit>::Table>,
    LuaTableField<Field::Style::padding, EdgeMap<Dimension::Unit>::Table>,
    LuaTableField<Field::Style::width, UnitAndValue<Dimension::Unit>::Table>,
    LuaTableField<Field::Style::height, UnitAndValue<Dimension::Unit>::Table>,
    LuaTableField<Field::Style::min_width, UnitAndValue<DimensionLimit::Unit>::Table>,
    LuaTableField<Field::Style::min_height, UnitAndValue<DimensionLimit::Unit>::Table>,
    LuaTableField<Field::Style::max_width, UnitAndValue<DimensionLimit::Unit>::Table>,
    LuaTableField<Field::Style::max_height, UnitAndValue<DimensionLimit::Unit>::Table>,
    LuaTableField<Field::Style::flex_basis, UnitAndValue<Dimension::Unit>::Table>,
    LuaTableField<Field::Style::flex_grow, float>,
    LuaTableField<Field::Style::flex_shrink, float>,
    LuaTableField<Field::Style::flex_direction, FlexDirection>,
    LuaTableField<Field::Style::flex_wrap, FlexWrap>,
    LuaTableField<Field::Style::content_alignment, ContentAlignment>,
    LuaTableField<Field::Style::content_justification, ContentJustification>,
    LuaTableField<Field::Style::items_alignment, ItemAlignment>,
    LuaTableField<Field::Style::self_alignment, ItemAlignment>,
    LuaTableField<Field::Style::aspect_ratio, float>,
    LuaTableField<Field::Style::display_mode, DisplayMode>,
    LuaTableField<Field::Style::gap, GapGutterMap::Table>,
    LuaTableField<Field::Style::position, EdgeMap<Position::Unit>::Table>,
    LuaTableField<Field::Style::position_type, Position::Type>
>;

// 1 self
// 2 type (int or string)
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

// // const std::unordered_map<Edge, Position> & _positions
// // OR Edge _edge, Position _posotion
// int luaApi_SetPosition(lua_State * _lua)
// {
//     return -1;
// }

// // const std::unordered_map<Edge, float> & _margins
// // OR Edge _edge, float _value
// int luaApi_SetMargin(lua_State * _lua)
// {
//     return -1;
// }

// // const std::unordered_map<Edge, float> & _paddings
// // OR // Edge _edge, float _value
// int luaApi_SetPadding(lua_State * _lua)
// {
//     return -1;
// }

// // const Size & _width
// int luaApi_SetWidth(lua_State * _lua)
// {
//     return -1;
// }

// // const Size & _height
// int luaApi_SetHeight(lua_State * _lua)
// {
//     return -1;
// }

// // const SizeLimit & _min_width
// int luaApi_SetMinWidth(lua_State * _lua)
// {
//     return -1;
// }

// // const SizeLimit & _min_height
// int luaApi_SetMinHeight(lua_State * _lua)
// {
//     return -1;
// }

// // const SizeLimit & _max_width
// int luaApi_SetMaxWidth(lua_State * _lua)
// {
//     return -1;
// }

// // const SizeLimit & _max_height
// int luaApi_SetMaxHeight(lua_State * _lua)
// {
//     return -1;
// }

// // float _basis
// int luaApi_SetFlexBasis(lua_State * _lua)
// {
//     return -1;
// }

// // float _grow
// int luaApi_SetFlexGrow(lua_State * _lua)
// {
//     return -1;
// }

// // float _shrink
// int luaApi_SetFlexShrink(lua_State * _lua)
// {
//     return -1;
// }

// // FlexDirection _direction
// int luaApi_SetFlexDirection(lua_State * _lua)
// {
//     return -1;
// }

// // FlexWrap _wrap
// int luaApi_SetFlexWrap(lua_State * _lua)
// {
//     return -1;
// }

// // const std::unordered_map<GapGutter, float> & _gaps
// // OR // GapGutter _gutter, float _gap
// int luaApi_SetGap(lua_State * _lua)
// {
//     return -1;
// }

// // ContentAlignment _alignment
// int luaApi_SetContentAlignment(lua_State * _lua)
// {
//     return -1;
// }

// // ContentJustification _justification
// int luaApi_SetContentJustification(lua_State * _lua)
// {
//     return -1;
// }

// // ItemAlignment _alignment
// int luaApi_SetItemsAlignment(lua_State * _lua)
// {
//     return -1;
// }

// // ItemAlignment _alignment
// int luaApi_SetSelfAlignment(lua_State * _lua)
// {
//     return -1;
// }

// // float _ratio
// int luaApi_SetAspectRatio(lua_State * _lua)
// {
//     return -1;
// }

// // DisplayMode _mode
// int luaApi_SetDisplayMode(lua_State * _lua)
// {
//     return -1;
// }

// // const Style & _style = Style()
// // return LayoutNode &
// int luaApi_AddNode(lua_State * _lua)
// {
//     return -1;
// }

// // return float
// int luaApi_GetX(lua_State * _lua)
// {
//     return -1;
// }

// // return float
// int luaApi_GetY(lua_State * _lua)
// {
//     return -1;
// }

// // return float
// int luaApi_GetWidth(lua_State * _lua)
// {
//     return -1;
// }

// // return float
// int luaApi_GetHeight(lua_State * _lua)
// {
//     return -1;
// }

// // return Children
// int luaApi_GetChildren(lua_State * _lua)
// {
//     return -1;
// }

// // return Element *
// int luaApi_GetElement(lua_State * _lua)
// {
//     return -1;
// }

// // std::unique_ptr<Element> && _element
// int luaApi_SetElement(lua_State * _lua)
// {
//     return -1;
// }

} // namespace name

void Sol2D::Lua::pushContentAlignmentEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
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
    if(!lua_istable(_lua, _idx))
        return false;
    StyleTable table(_lua, _idx);
    if(auto margin_table = table.tryGetValue<Field::Style::margin>())
        EdgeMap<Dimension::Unit>(*margin_table).read<Dimension>(_style.margin);
    if(auto padding_table = table.tryGetValue<Field::Style::padding>())
        EdgeMap<Dimension::Unit>(*padding_table).read<Dimension>(_style.padding);
    if(auto width_table = table.tryGetValue<Field::Style::width>())
        _style.width = UnitAndValue<Dimension::Unit>(*width_table).aggregate<Dimension>();
    if(auto height_table = table.tryGetValue<Field::Style::height>())
        _style.height = UnitAndValue<Dimension::Unit>(*height_table).aggregate<Dimension>();
    if(auto min_width_table = table.tryGetValue<Field::Style::min_width>())
        _style.min_width = UnitAndValue<DimensionLimit::Unit>(*min_width_table).aggregate<DimensionLimit>();
    if(auto min_height_table = table.tryGetValue<Field::Style::min_height>())
        _style.min_height = UnitAndValue<DimensionLimit::Unit>(*min_height_table).aggregate<DimensionLimit>();
    if(auto max_width_table = table.tryGetValue<Field::Style::max_width>())
        _style.max_width = UnitAndValue<DimensionLimit::Unit>(*max_width_table).aggregate<DimensionLimit>();
    if(auto max_height_table = table.tryGetValue<Field::Style::max_height>())
        _style.max_height = UnitAndValue<DimensionLimit::Unit>(*max_height_table).aggregate<DimensionLimit>();
    if(auto flex_basis_table = table.tryGetValue<Field::Style::flex_basis>())
        _style.flex_basis = UnitAndValue<Dimension::Unit>(*flex_basis_table).aggregate<Dimension>();
    _style.flex_grow = table.tryGetValue<Field::Style::flex_grow>();
    _style.flex_shrink = table.tryGetValue<Field::Style::flex_shrink>();
    _style.flex_direction = table.tryGetValue<Field::Style::flex_direction>();
    _style.flex_wrap = table.tryGetValue<Field::Style::flex_wrap>();
    _style.content_alignment = table.tryGetValue<Field::Style::content_alignment>();
    _style.content_justification = table.tryGetValue<Field::Style::content_justification>();
    _style.items_alignment = table.tryGetValue<Field::Style::items_alignment>();
    _style.self_alignment = table.tryGetValue<Field::Style::self_alignment>();
    _style.aspect_ratio = table.tryGetValue<Field::Style::aspect_ratio>();
    _style.display_mode = table.tryGetValue<Field::Style::display_mode>();
    if(auto gap_table = table.tryGetValue<Field::Style::gap>())
        GapGutterMap(*gap_table).read(_style.gap);
    if(auto position_table = table.tryGetValue<Field::Style::position>())
        EdgeMap<Position::Unit>(*position_table).read<Position>(_style.position);
    _style.position_type = table.tryGetValue<Field::Style::position_type>();
    return true;
}

void Sol2D::Lua::pushLayoutNodeApi(lua_State * _lua, std::shared_ptr<Node> _node)
{
    UserData::pushUserData(_lua, _node);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "__gc", UserData::luaGC },
            { "setPositionType", luaApi_SetPositionType },
            // { "setPosition", luaApi_SetPosition },
            // { "setMargin", luaApi_SetMargin },
            // { "setPadding", luaApi_SetPadding },
            // { "setWidth", luaApi_SetWidth },
            // { "setHeight", luaApi_SetHeight },
            // { "setMinWidth", luaApi_SetMinWidth },
            // { "setMinHeight", luaApi_SetMinHeight },
            // { "setMaxWidth", luaApi_SetMaxWidth },
            // { "setMaxHeight", luaApi_SetMaxHeight },
            // { "setFlexBasis", luaApi_SetFlexBasis },
            // { "setFlexGrow", luaApi_SetFlexGrow },
            // { "setFlexShrink", luaApi_SetFlexShrink },
            // { "setFlexDirection", luaApi_SetFlexDirection },
            // { "setFlexWrap", luaApi_SetFlexWrap },
            // { "setGap", luaApi_SetGap },
            // { "setContentAlignment", luaApi_SetContentAlignment },
            // { "setContentJustification", luaApi_SetContentJustification },
            // { "setItemsAlignment", luaApi_SetItemsAlignment },
            // { "setSelfAlignment", luaApi_SetSelfAlignment },
            // { "setAspectRatio", luaApi_SetAspectRatio },
            // { "setDisplayMode", luaApi_SetDisplayMode },
            // { "addNode", luaApi_AddNode },
            // { "getX", luaApi_GetX },
            // { "getY", luaApi_GetY },
            // { "getWidth", luaApi_GetWidth },
            // { "getHeight", luaApi_GetHeight },
            // { "getChildren", luaApi_GetChildren },
            // { "getElement", luaApi_GetElement },
            // { "setElement", luaApi_SetElement },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
