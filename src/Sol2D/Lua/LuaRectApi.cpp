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

#include <Sol2D/Lua/LuaRectApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

static constexpr char gc_key_x[] = "x";
static constexpr char gc_key_y[] = "y";
static constexpr char gc_key_w[] = "w";
static constexpr char gc_key_h[] = "h";

} // namespace name

bool Sol2D::Lua::tryGetRect(lua_State * _lua, int _idx, Rect & _rect)
{
    LuaTable table(_lua, _idx);
    if(!table.isValid())
        return false;

    float x, y, w, h;
    if(
        table.tryGetNumber(gc_key_w, &w) &&
        table.tryGetNumber(gc_key_h, &h) &&
        table.tryGetNumber(gc_key_x, &x) &&
        table.tryGetNumber(gc_key_y, &y))
    {
        _rect.x = x;
        _rect.y = y;
        _rect.w = w;
        _rect.h = h;
        return true;
    }
    return false;
}

void Sol2D::Lua::pushRect(lua_State * _lua, const Rect & _rect)
{
    LuaTable table = LuaTable::pushNew(_lua);
    table.setNumberValue(gc_key_x, _rect.x);
    table.setNumberValue(gc_key_y, _rect.y);
    table.setNumberValue(gc_key_w, _rect.w);
    table.setNumberValue(gc_key_h, _rect.h);
}
