// Sol2D Game Engine
// Copyright (C) 2023-2024 Sergey Smolyannikov aka brainstream
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

#include <Sol2D/Lua/LuaFragmentApi.h>
#include <Sol2D/Lua/Aux/LuaTopStackTable.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_key_dimension_unit[] = "unit";
const char gc_key_dimension_value[] = "value";
const char gc_key_top[] = "top";
const char gc_key_right[] = "right";
const char gc_key_left[] = "left";
const char gc_key_bottom[] = "bottom";
const char gc_key_width[] = "width";
const char gc_key_height[] = "height";
const char gc_key_z_index[] = "zIndex";
const char gc_key_is_visible[] = "isVisible";

template<std::integral Int>
void setDimension(LuaTopStackTable & table, const char * _key, const std::optional<Dimension<Int>> & _dimension)
{
    if(_dimension.has_value())
    {
        LuaTopStackTable dimension_table = LuaTopStackTable::pushNew(table.getLua());
        dimension_table.setIntegerValue(gc_key_dimension_unit, static_cast<lua_Integer>(_dimension.value().unit));
        dimension_table.setIntegerValue(gc_key_dimension_value, _dimension.value().value);
        table.setValueFromTop(_key);
    }
}

inline std::optional<DimensionUnit> tryParseDimensionUnit(lua_Integer _unit)
{
    switch(_unit)
    {
    case static_cast<lua_Integer>(DimensionUnit::Pixel):
        return DimensionUnit::Pixel;
    case static_cast<lua_Integer>(DimensionUnit::Percent):
        return DimensionUnit::Percent;
    default:
        return std::optional<DimensionUnit>();
    }
}

template<std::integral Int>
std::optional<Dimension<Int>> tryGetDimension(lua_State * _lua, int _table_idx, const char * _key)
{
    std::optional<Dimension<Int>> result;
    lua_pushstring(_lua, _key);
    if(lua_gettable(_lua, _table_idx) == LUA_TTABLE)
    {
        LuaTable table(_lua, -1);
        lua_Integer unit_int, value;
        if(table.tryGetInteger(gc_key_dimension_unit, &unit_int) && table.tryGetInteger(gc_key_dimension_value, &value))
        {
            std::optional<DimensionUnit> unit = tryParseDimensionUnit(unit_int);
            if(unit.has_value())
                result = Dimension<Int>(static_cast<Int>(value), unit.value());
        }
    }
    lua_pop(_lua, 1);
    return result;
}

} // namespace

void Sol2D::Lua::pushFragment(lua_State * _lua, const Fragment & _fragment)
{
    LuaTopStackTable table = LuaTopStackTable::pushNew(_lua);
    setDimension(table, gc_key_top, _fragment.top);
    setDimension(table, gc_key_right, _fragment.right);
    setDimension(table, gc_key_bottom, _fragment.bottom);
    setDimension(table, gc_key_left, _fragment.left);
    setDimension(table, gc_key_width, _fragment.width);
    setDimension(table, gc_key_height, _fragment.height);
    table.setIntegerValue(gc_key_z_index, _fragment.z_index);
    table.setBooleanValue(gc_key_is_visible, _fragment.is_visible);
}

bool Sol2D::Lua::tryGetFragment(lua_State * _lua, int _idx, Fragment & _fragment)
{
    int index = lua_absindex(_lua, _idx);
    if(!lua_istable(_lua, index))
        return false;
    _fragment.top = tryGetDimension<int32_t>(_lua, index, gc_key_top);
    _fragment.right = tryGetDimension<int32_t>(_lua, index, gc_key_right);
    _fragment.left = tryGetDimension<int32_t>(_lua, index, gc_key_left);
    _fragment.bottom = tryGetDimension<int32_t>(_lua, index, gc_key_bottom);
    _fragment.width = tryGetDimension<uint32_t>(_lua, index, gc_key_width);
    _fragment.height = tryGetDimension<uint32_t>(_lua, index, gc_key_height);
    LuaTable table(_lua, index);
    table.tryGetBoolean(gc_key_is_visible, &_fragment.is_visible);
    lua_Integer z_index;
    if(table.tryGetInteger(gc_key_z_index, &z_index))
        _fragment.z_index = static_cast<uint16_t>(z_index);
    return true;
}
