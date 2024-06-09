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
#include <Sol2D/Lua/LuaWidgetPaddingApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Forms;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

bool tryGetDimensionFromTable(LuaTable & _table, const char * _key, double * _value, DimensionUnit * _unit)
{
    bool result = false;
    if(_table.tryGetValue(_key))
    {
        result = tryGetDimension(_table.getLua(), -1, _value, _unit);
        lua_pop(_table.getLua(), 1);
    }
    return result;
}

} // namespace

bool Sol2D::Lua::tryGetWidgetPadding(lua_State * _lua, int _idx, Forms::WidgetPadding & _padding)
{
    if(lua_istable(_lua, _idx))
    {
        double value;
        DimensionUnit unit;
        LuaTable table(_lua, _idx);
        if(tryGetDimensionFromTable(table, "top", &value, &unit))
        {
            _padding.top.value = static_cast<float>(value);
            _padding.top.unit = unit;
        }
        if(tryGetDimensionFromTable(table, "right", &value, &unit))
        {
            _padding.right.value = static_cast<float>(value);
            _padding.right.unit = unit;
        }
        if(tryGetDimensionFromTable(table, "bottom", &value, &unit))
        {
            _padding.bottom.value = static_cast<float>(value);
            _padding.bottom.unit = unit;
        }
        if(tryGetDimensionFromTable(table, "left", &value, &unit))
        {
            _padding.left.value = static_cast<float>(value);
            _padding.left.unit = unit;
        }
        return true;
    }
    else
    {
        std::optional<Dimension<float>> dimension = tryGetDimension<float>(_lua, _idx);
        if(dimension.has_value())
        {
            _padding.top = dimension.value();
            _padding.bottom = dimension.value();
            _padding.right = dimension.value();
            _padding.left = dimension.value();
            return true;
        }
    }
    return false;
}
