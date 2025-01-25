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

#include <Sol2D/Lua/LuaFragmentApi.h>
#include <Sol2D/Lua/LuaDimensionApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

const char gc_key_top[] = "top";
const char gc_key_right[] = "right";
const char gc_key_left[] = "left";
const char gc_key_bottom[] = "bottom";
const char gc_key_width[] = "width";
const char gc_key_height[] = "height";
const char gc_key_z_index[] = "zIndex";
const char gc_key_is_visible[] = "isVisible";

template<std::integral Int>
void setDimension(LuaTable & table, const char * _key, const std::optional<Dimension<Int>> & _dimension)
{
    if(_dimension.has_value())
    {
        pushDimensionI(table.getLua(), _dimension->value, _dimension->unit);
        table.setValueFromTop(_key);
    }
}

} // namespace

void Sol2D::Lua::pushFragment(lua_State * _lua, const Fragment & _fragment)
{
    LuaTable table = LuaTable::pushNew(_lua);
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
    LuaTable table(_lua, _idx);
    if(!table.isValid())
        return false;
    table.tryGetDimension(gc_key_top, _fragment.top);
    table.tryGetDimension(gc_key_right, _fragment.right);
    table.tryGetDimension(gc_key_left, _fragment.left);
    table.tryGetDimension(gc_key_bottom, _fragment.bottom);
    table.tryGetDimension(gc_key_width, _fragment.width);
    table.tryGetDimension(gc_key_height, _fragment.height);
    table.tryGetBoolean(gc_key_is_visible, &_fragment.is_visible);
    table.tryGetInteger(gc_key_z_index, &_fragment.z_index);
    return true;
}
