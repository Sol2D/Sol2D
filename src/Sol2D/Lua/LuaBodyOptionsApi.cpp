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

#include <Sol2D/Lua/LuaBodyOptionsApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua::Aux;

namespace {

constexpr char gc_key_type[] = "type";
constexpr char gc_key_linear_damping[] = "linearDamping";
constexpr char gc_key_angular_damping[] = "angularDamping";
constexpr char gc_key_fixed_rotation[] = "fixedRotation";

} // namespace name

bool Sol2D::Lua::tryGetBodyOptions(lua_State * _lua, int _idx, BodyOptions & _body_options)
{
    if(!lua_istable(_lua, _idx))
    {
        return false;
    }

    LuaTable table(_lua, _idx);

    {
        lua_Integer lua_int;
        if(table.tryGetInteger(gc_key_type, &lua_int))
        {
            std::optional<BodyType> body_type = castToBodyType(lua_int);
            if(body_type.has_value())
                _body_options.type = body_type.value();
        }
    }

    {
        lua_Number lua_num;
        if(table.tryGetNumber(gc_key_linear_damping, &lua_num))
            _body_options.linear_damping = static_cast<float>(lua_num);
        if(table.tryGetNumber(gc_key_angular_damping, &lua_num))
            _body_options.angular_damping = static_cast<float>(lua_num);
    }

    table.tryGetBoolean(gc_key_fixed_rotation, &_body_options.fixed_rotation);

    return true;
}
