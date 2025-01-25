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

#include <Sol2D/Lua/LuaColorApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

static constexpr char gc_key_r[] = "r";
static constexpr char gc_key_g[] = "g";
static constexpr char gc_key_b[] = "b";
static constexpr char gc_key_a[] = "a";

} // namespace name

bool Sol2D::Lua::tryGetColor(lua_State * _lua, int _idx, Color & _color)
{
    LuaTable table(_lua, _idx);
    if(!table.isValid())
        return false;
    Uint8 r, g, b;
    if(
        table.tryGetInteger(gc_key_r, &r) &&
        table.tryGetInteger(gc_key_g, &g) &&
        table.tryGetInteger(gc_key_b, &b))
    {
        if(!table.tryGetInteger(gc_key_a, &_color.a))
            _color.a = 255;
        _color.r = r;
        _color.g = g;
        _color.b = b;
        return true;
    }
    return false;
}
