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

using namespace Sol2D;
using namespace Sol2D::Layouting;
using namespace Sol2D::Lua;;

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

constexpr const char g_size[] = "size";
constexpr const char g_size_unit[] = "unit";
constexpr const char g_size_value[] = "value";

using SizeTable = LuaTable<
    LuaTableField<g_size_unit, Sol2D::Layouting::Size::Unit>,
    LuaTableField<g_size_value, float>
>;

using StyleTable = LuaTable<
    LuaTableField<g_size, SizeTable>
    /*
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
     */
>;

bool tryGetStyle(lua_State * _lua, int _idx, Style & _style)
{
    return false;
}

// 1 self
// 2 type (int or string)
int luaApi_SetPositionType(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    std::optional<Position::Type> type;
    if(lua_isstring(_lua, 2))
    {
        type = tryParsePositionType(lua_tostring(_lua, 2));
    }
    else if(lua_isinteger(_lua, 2))
    {
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
    }
    luaL_argexpected(
        _lua,
        type.has_value(),
        2,
        LuaTypeName::joinTypes(LuaTypeName::integer, LuaTypeName::string).c_str()
    );
    self->getNode(_lua)->setPositionType(type.value());
    return 0;;
}

// const std::unordered_map<Edge, Position> & _positions
// OR Edge _edge, Position _posotion
int luaApi_SetPosition(lua_State * _lua)
{
    return -1;
}

// const std::unordered_map<Edge, float> & _margins
// OR Edge _edge, float _value
int luaApi_SetMargin(lua_State * _lua)
{
    return -1;
}

// const std::unordered_map<Edge, float> & _paddings
// OR // Edge _edge, float _value
int luaApi_SetPadding(lua_State * _lua)
{
    return -1;
}

// const Size & _width
int luaApi_SetWidth(lua_State * _lua)
{
    return -1;
}

// const Size & _height
int luaApi_SetHeight(lua_State * _lua)
{
    return -1;
}

// const SizeLimit & _min_width
int luaApi_SetMinWidth(lua_State * _lua)
{
    return -1;
}

// const SizeLimit & _min_height
int luaApi_SetMinHeight(lua_State * _lua)
{
    return -1;
}

// const SizeLimit & _max_width
int luaApi_SetMaxWidth(lua_State * _lua)
{
    return -1;
}

// const SizeLimit & _max_height
int luaApi_SetMaxHeight(lua_State * _lua)
{
    return -1;
}

// float _basis
int luaApi_SetFlexBasis(lua_State * _lua)
{
    return -1;
}

// float _grow
int luaApi_SetFlexGrow(lua_State * _lua)
{
    return -1;
}

// float _shrink
int luaApi_SetFlexShrink(lua_State * _lua)
{
    return -1;
}

// FlexDirection _direction
int luaApi_SetFlexDirection(lua_State * _lua)
{
    return -1;
}

// FlexWrap _wrap
int luaApi_SetFlexWrap(lua_State * _lua)
{
    return -1;
}

// const std::unordered_map<GapGutter, float> & _gaps
// OR // GapGutter _gutter, float _gap
int luaApi_SetGap(lua_State * _lua)
{
    return -1;
}

// ContentAlignment _alignment
int luaApi_SetContentAlignment(lua_State * _lua)
{
    return -1;
}

// ContentJustification _justification
int luaApi_SetContentJustification(lua_State * _lua)
{
    return -1;
}

// ItemAlignment _alignment
int luaApi_SetItemsAlignment(lua_State * _lua)
{
    return -1;
}

// ItemAlignment _alignment
int luaApi_SetSelfAlignment(lua_State * _lua)
{
    return -1;
}

// float _ratio
int luaApi_SetAspectRatio(lua_State * _lua)
{
    return -1;
}

// DisplayMode _mode
int luaApi_SetDisplayMode(lua_State * _lua)
{
    return -1;
}

// const Style & _style = Style()
// return LayoutNode &
int luaApi_AddNode(lua_State * _lua)
{
    return -1;
}

// return float
int luaApi_GetX(lua_State * _lua)
{
    return -1;
}

// return float
int luaApi_GetY(lua_State * _lua)
{
    return -1;
}

// return float
int luaApi_GetWidth(lua_State * _lua)
{
    return -1;
}

// return float
int luaApi_GetHeight(lua_State * _lua)
{
    return -1;
}

// return Children
int luaApi_GetChildren(lua_State * _lua)
{
    return -1;
}

// return Element *
int luaApi_GetElement(lua_State * _lua)
{
    return -1;
}

// std::unique_ptr<Element> && _element
int luaApi_SetElement(lua_State * _lua)
{
    return -1;
}

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

void Sol2D::Lua::pushSizeUnitEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("AUTO", static_cast<lua_Integer>(Layouting::Size::Unit::Auto));
        table.setIntegerValue("POINT", static_cast<lua_Integer>(Layouting::Size::Unit::Point));
        table.setIntegerValue("PERCENT", static_cast<lua_Integer>(Layouting::Size::Unit::Percent));
        table.setIntegerValue("MAX_CONTENT", static_cast<lua_Integer>(Layouting::Size::Unit::MaxContent));
        table.setIntegerValue("FIT_CONTENT", static_cast<lua_Integer>(Layouting::Size::Unit::FitContent));
        table.setIntegerValue("STRETCH", static_cast<lua_Integer>(Layouting::Size::Unit::Stretch));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushSizeLimitUnitEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::horizontal_text_alignment) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("POINT", static_cast<lua_Integer>(SizeLimit::Unit::Point));
        table.setIntegerValue("PERCENT", static_cast<lua_Integer>(SizeLimit::Unit::Percent));
        table.setIntegerValue("MAX_CONTENT", static_cast<lua_Integer>(SizeLimit::Unit::MaxContent));
        table.setIntegerValue("FIT_CONTENT", static_cast<lua_Integer>(SizeLimit::Unit::FitContent));
        table.setIntegerValue("STRETCH", static_cast<lua_Integer>(SizeLimit::Unit::Stretch));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushLayoutNodeApi(lua_State * _lua, std::shared_ptr<Node> _node)
{
    UserData::pushUserData(_lua, _node);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
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
            { "getElement", luaApi_GetElement },
            { "setElement", luaApi_SetElement },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
