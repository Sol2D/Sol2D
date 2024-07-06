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

#include <Sol2D/Lua/LuaDimensionApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <cstring>

using namespace Sol2D;
using namespace Sol2D::Lua::Aux;

namespace {

bool parseDimension(const char * _str, double * _value, DimensionUnit * _unit)
{
    char * end;
    *_value = std::strtod(_str, &end);
    if(_str == end)
        return false;
    size_t len = std::strlen(end);
    *_unit = len == 1 && end[len - 1] == '%' ? DimensionUnit::Percent : DimensionUnit::Pixel;
    return true;
}

} // namespace

void Sol2D::Lua::pushDimensionUnitEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::dimension_unit_type) == MetatablePushResult::Created)
    {
        LuaTable table(_lua);
        table.setIntegerValue("PIXEL", static_cast<lua_Integer>(DimensionUnit::Pixel));
        table.setIntegerValue("PERCENT", static_cast<lua_Integer>(DimensionUnit::Percent));
    }
    lua_setmetatable(_lua, -2);
}

bool Sol2D::Lua::tryGetDimension(lua_State * _lua, int _idx, double * _value, DimensionUnit * _unit)
{
    if(lua_isnumber(_lua, _idx))
    {
        *_value = lua_tonumber(_lua, _idx);
        *_unit = DimensionUnit::Pixel;
        return true;
    }
    if(lua_isstring(_lua, _idx))
    {
        return parseDimension(lua_tostring(_lua, _idx), _value, _unit);
    }
    if(lua_istable(_lua, _idx))
    {
        LuaTable table(_lua, _idx);
        if(!table.tryGetNumber("value", _value))
            return false;
        lua_Integer unit;
        *_unit =
            table.tryGetInteger("unit", &unit) && unit == static_cast<lua_Integer>(DimensionUnit::Percent)
            ? DimensionUnit::Percent
            : DimensionUnit::Pixel;
        return true;
    }
    return false;
}
