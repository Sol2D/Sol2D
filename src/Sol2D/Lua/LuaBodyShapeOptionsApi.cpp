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

#include <Sol2D/Lua/LuaBodyShapeOptionsApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua::Aux;

namespace {

constexpr char gc_key_is_sensor[] = "isSensor";
constexpr char gc_key_is_pre_solve_enabled[] = "isPreSolveEnabled";
constexpr char gc_key_density[] = "density";

} // namespace name

bool Sol2D::Lua::tryGetBodyShapeOptions(lua_State * _lua, int _idx, BodyShapeOptions & _body_shape_options)
{
    if(!lua_istable(_lua, _idx))
    {
        return false;
    }

    LuaTable table(_lua, _idx);

    {
        lua_Number lua_num;
        if(table.tryGetNumber(gc_key_density, &lua_num))
            _body_shape_options.density = static_cast<float>(lua_num);
    }

    table.tryGetBoolean(gc_key_is_sensor, &_body_shape_options.is_sensor);
    table.tryGetBoolean(gc_key_is_pre_solve_enabled, &_body_shape_options.is_pre_solve_enalbed);

    return true;
}
