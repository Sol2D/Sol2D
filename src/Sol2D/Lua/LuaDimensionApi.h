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

#pragma once

#include <Sol2D/Dimension.h>
#include <Sol2D/Lua/Aux/LuaForward.h>
#include <optional>

namespace Sol2D::Lua {

void pushDimensionUnitEnum(lua_State * _lua);
void pushDimensionD(lua_State * _lua, double _value, DimensionUnit _unit);
void pushDimensionI(lua_State * _lua, long long _value, DimensionUnit _unit);
bool tryGetDimensionD(lua_State * _lua, int _idx, double * _value, DimensionUnit * _unit);
bool tryGetDimensionI(lua_State * _lua, int _idx, long long * _value, DimensionUnit * _unit);

template<std::integral Number>
bool tryGetDimension(lua_State * _lua, int _idx, std::optional<Dimension<Number>> & _value)
{
    long long value;
    DimensionUnit unit;
    if(tryGetDimensionI(_lua, _idx, &value, &unit))
    {
        _value = Dimension<Number>(static_cast<Number>(value), unit);;
        return true;
    }
    return false;
}

template<std::integral Number>
bool tryGetDimension(lua_State * _lua, int _idx, Dimension<Number> & _value)
{
    long long value;
    if(tryGetDimensionI(_lua, _idx, &value, &_value.unit))
    {
        _value.value = value;
        return true;
    }
    return false;
}

template<std::floating_point Number>
bool tryGetDimension(lua_State * _lua, int _idx, std::optional<Dimension<Number>> & _value)
{
    double value;
    DimensionUnit unit;
    if(tryGetDimensionD(_lua, _idx, &value, &unit))
    {
        _value = Dimension<Number>(static_cast<Number>(value), unit);
        return true;
    }
    return false;
}

template<std::floating_point Number>
bool tryGetDimension(lua_State * _lua, int _idx, Dimension<Number> & _value)
{
    double value;
    if(tryGetDimensionD(_lua, _idx, &value, &_value.unit))
    {
        _value.value = value;
        return true;
    }
    return false;
}

} // namespace Sol2D::Lua
