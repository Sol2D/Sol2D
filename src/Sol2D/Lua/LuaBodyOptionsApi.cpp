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
#include <Sol2D/Lua/LuaBodyPhysicsDefinitionApi.h>
#include <Sol2D/Lua/LuaBodyShapePhysicsDefinitionApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua::Aux;

bool Sol2D::Lua::tryGetBodyOptions(lua_State * _lua, int _idx, BodyOptions & _body_options)
{
    if(!lua_istable(_lua, _idx))
    {
        return false;
    }
    LuaTable table(_lua, _idx);
    {
        lua_Integer lua_int;
        if(table.tryGetInteger("type", &lua_int))
        {
            std::optional<BodyType> body_type = castToBodyType(lua_int);
            if(body_type.has_value())
                _body_options.type = body_type.value();
        }
    }
    if(table.tryGetValue("bodyPhysics"))
    {
        tryGetBodyPhysicsDefinition(_lua, -1, _body_options.body_physics);
        lua_pop(_lua, 1);
    }
    if(table.tryGetValue("shapePhysics"))
    {
        tryGetBodyShapePhysicsDefinition(_lua, -1, _body_options.shape_physics);
        lua_pop(_lua, 1);
    }
    {
        std::string value;
        if(table.tryGetString("shapeKey", value))
            _body_options.shape_key = value;
    }
    return true;
}
