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

#include <Sol2D/Lua/LuaManifoldApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/LuaPointApi.h>

using namespace Sol2D::Lua;

namespace {

void pushManifoldPoint(lua_State * _lua, const b2ManifoldPoint & _point)
{
    LuaTable table = LuaTable::pushNew(_lua);
    Sol2D::Lua::pushPoint(_lua, _point.point.x, _point.point.y);
    table.setValueFromTop("point");
    Sol2D::Lua::pushPoint(_lua, _point.anchorA.x, _point.anchorA.y);
    table.setValueFromTop("anchorA");
    Sol2D::Lua::pushPoint(_lua, _point.anchorB.x, _point.anchorB.y);
    table.setValueFromTop("anchorB");
    table.setNumberValue("separation", _point.separation);
    table.setNumberValue("normalImpulse", _point.normalImpulse);
    table.setNumberValue("tangentImpulse", _point.tangentImpulse);
    table.setNumberValue("normalVelocity", _point.normalVelocity);
}

} // namespace

void Sol2D::Lua::pushManifold(lua_State * _lua, const b2Manifold & _manifold)
{
    LuaTable table = LuaTable::pushNew(_lua);
    pushPoint(_lua, _manifold.normal.x, _manifold.normal.y);
    table.setValueFromTop("normal");

    lua_newtable(_lua);
    for(int i = 0; i < _manifold.pointCount; ++i)
    {
        pushManifoldPoint(_lua, _manifold.points[i]);
        lua_rawseti(_lua, -2, i + 1);
    }
    table.setValueFromTop("points");
}
