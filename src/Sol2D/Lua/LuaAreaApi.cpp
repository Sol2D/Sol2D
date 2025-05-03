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

#include <Sol2D/Lua/LuaAreaApi.h>
#include <Sol2D/Lua/LuaDimensionApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

const char g_key_top[] = "top";
const char g_key_right[] = "right";
const char g_key_left[] = "left";
const char g_key_bottom[] = "bottom";
const char g_key_width[] = "width";
const char g_key_height[] = "height";
const char g_key_z_index[] = "zIndex";
const char g_key_is_visible[] = "isVisible";

template<std::integral Int>
void setDimension(LuaTableApi & table, const char * _key, const std::optional<Dimension_Obsolete<Int>> & _dimension)
{
    if(_dimension.has_value())
    {
        pushDimensionI(table.getLua(), _dimension->value, _dimension->unit);
        table.setValueFromTop(_key);
    }
}

} // namespace

void Sol2D::Lua::pushArea(lua_State * _lua, const Area & _area)
{
    LuaTableApi table = LuaTableApi::pushNew(_lua);
    setDimension(table, g_key_top, _area.top);
    setDimension(table, g_key_right, _area.right);
    setDimension(table, g_key_bottom, _area.bottom);
    setDimension(table, g_key_left, _area.left);
    setDimension(table, g_key_width, _area.width);
    setDimension(table, g_key_height, _area.height);
    table.setIntegerValue(g_key_z_index, _area.z_index);
    table.setBooleanValue(g_key_is_visible, _area.is_visible);
}

bool Sol2D::Lua::tryGetArea(lua_State * _lua, int _idx, Area & _area)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return false;
    table.tryGetDimension(g_key_top, _area.top);
    table.tryGetDimension(g_key_right, _area.right);
    table.tryGetDimension(g_key_left, _area.left);
    table.tryGetDimension(g_key_bottom, _area.bottom);
    table.tryGetDimension(g_key_width, _area.width);
    table.tryGetDimension(g_key_height, _area.height);
    table.tryGetBoolean(g_key_is_visible, &_area.is_visible);
    table.tryGetInteger(g_key_z_index, &_area.z_index);
    return true;
}
