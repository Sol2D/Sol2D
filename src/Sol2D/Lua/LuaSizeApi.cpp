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

#include <Sol2D/Lua/LuaSizeApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/Aux/LuaTopStackTable.h>

using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

static constexpr char gc_key_w[] = "w";
static constexpr char gc_key_h[] = "h";

} // namespace name

bool Sol2D::Lua::tryGetSize(lua_State * _lua, int _idx, SDL_FPoint & _size)
{
    if(!lua_istable(_lua, _idx))
    {
        return false;
    }
    LuaTable table(_lua, _idx);
    lua_Number w, h;
    if(
        table.tryGetNumber(gc_key_w, &w) &&
        table.tryGetNumber(gc_key_h, &h))
    {
        _size.x = static_cast<float>(w);
        _size.y = static_cast<float>(h);
        return true;
    }
    return false;
}

void Sol2D::Lua::pushSize(lua_State * _lua, float _w, float _h)
{
    LuaTopStackTable table = LuaTopStackTable::pushNew(_lua);
    table.setNumberValue(gc_key_h, _h);
    table.setNumberValue(gc_key_w, _w);
}
