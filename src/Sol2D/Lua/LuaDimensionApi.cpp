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

#include <Sol2D/Lua/LuaDimensionApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>
#include <Sol2D/Lua/Aux/LuaUtils.h>
#include <cstring>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

const char g_key_value[] = "value";
const char g_key_unit[] = "unit";

template<DimensionValueConcept Number>
using ParseFn = Number (*)(const char *, char **);

template<DimensionValueConcept Number>
bool parseDimension(const char * _str, Number * _value, DimensionUnit * _unit, ParseFn<Number> _parse)
{
    char * end;
    *_value = _parse(_str, &end);
    if(_str == end)
        return false;
    size_t len = std::strlen(end);
    *_unit = len == 1 && end[len - 1] == '%' ? DimensionUnit::Percent : DimensionUnit::Pixel;
    return true;
}

long long strToLL(const char * _str, char ** _endptr)
{
    return std::strtoll(_str, _endptr, 10);
}

inline DimensionUnit getUnit(LuaTableApi & _table)
{
    int unit;
    return _table.tryGetInteger(g_key_unit, &unit) && unit == static_cast<int>(DimensionUnit::Percent)
        ? DimensionUnit::Percent
        : DimensionUnit::Pixel;
}

} // namespace

void Sol2D::Lua::pushDimensionUnitEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, LuaTypeName::dimension_unit_type) == MetatablePushResult::Created)
    {
        LuaTableApi table(_lua);
        table.setIntegerValue("PIXEL", static_cast<lua_Integer>(DimensionUnit::Pixel));
        table.setIntegerValue("PERCENT", static_cast<lua_Integer>(DimensionUnit::Percent));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushDimensionD(lua_State * _lua, double _value, DimensionUnit _unit)
{
    LuaTableApi table = LuaTableApi::pushNew(_lua);
    table.setNumberValue(g_key_value, _value);
    table.setIntegerValue(g_key_unit, static_cast<lua_Integer>(_unit));
}

void Sol2D::Lua::pushDimensionI(lua_State * _lua, long long _value, DimensionUnit _unit)
{
    LuaTableApi table = LuaTableApi::pushNew(_lua);
    table.setIntegerValue(g_key_value, _value);
    table.setIntegerValue(g_key_unit, static_cast<lua_Integer>(_unit));
}

bool Sol2D::Lua::tryGetDimensionD(lua_State * _lua, int _idx, double * _value, DimensionUnit * _unit)
{
    if(lua_isnumber(_lua, _idx))
    {
        *_value = lua_tonumber(_lua, _idx);
        *_unit = DimensionUnit::Pixel;
        return true;
    }
    if(const char * str = argToString(_lua, _idx))
    {
        return parseDimension(str, _value, _unit, &std::strtod);
    }
    LuaTableApi table(_lua, _idx);
    if(table.isValid())
    {
        if(!table.tryGetNumber(g_key_value, _value))
            return false;
        *_unit = getUnit(table);
        return true;
    }
    return false;
}

bool Sol2D::Lua::tryGetDimensionI(lua_State * _lua, int _idx, long long * _value, DimensionUnit * _unit)
{
    if(lua_isinteger(_lua, _idx))
    {
        *_value = static_cast<long long>(lua_tointeger(_lua, _idx));
        *_unit = DimensionUnit::Pixel;
        return true;
    }
    if(const char * str = argToString(_lua, _idx))
    {
        return parseDimension(str, _value, _unit, &strToLL);
    }
    LuaTableApi table(_lua, _idx);
    if(table.isValid())
    {
        if(!table.tryGetInteger(g_key_value, _value))
            return false;
        *_unit = getUnit(table);
        return true;
    }
    return false;
}
