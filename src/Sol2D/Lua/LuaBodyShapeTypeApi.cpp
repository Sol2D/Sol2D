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

#include <Sol2D/Lua/LuaBodyShapeTypeApi.h>
#include <Sol2D/Lua/Aux/LuaTopStackTable.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/BodyShapeType.h>

using namespace Sol2D::Lua::Aux;

namespace {

const char gc_metatable_body_shape_type[] = "sol.BodyShapeType";

} // namespace

void Sol2D::Lua::pushBodyShapeTypeEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, gc_metatable_body_shape_type) == MetatablePushResult::Created)
    {
        LuaTopStackTable table(_lua);
        table.setIntegerValue("CIRCLE", static_cast<lua_Integer>(BodyShapeType::Circle));
        table.setIntegerValue("POLYGON", static_cast<lua_Integer>(BodyShapeType::Polygon));
    }
    lua_setmetatable(_lua, -2);
}
