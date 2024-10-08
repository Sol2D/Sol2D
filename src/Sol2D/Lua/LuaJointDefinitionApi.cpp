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

#include <Sol2D/Lua/LuaJointDefinitionApi.h>
#include <Sol2D/Lua/LuaBodyApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

bool tryGetBodyIdFromJointDefinition(const LuaTable & _table, const char * _field, uint64_t * _id)
{
    {
        lua_Integer id;
        if(_table.tryGetInteger(_field, &id))
        {
            *_id = static_cast<uint64_t>(id);
            return true;
        }
    }
    if(_table.tryGetValue(_field))
    {
        bool result = tryGetBodyId(_table.getLua(), -1, _id);
        lua_pop(_table.getLua(), 1);
        return result;
    }
    return false;
}

} // namespace

bool Sol2D::Lua::tryGetJointDefinition(lua_State * _lua, int _idx, World::JointDefinition & _result)
{
    if(!lua_istable(_lua, _idx))
        return false;
    LuaTable table(_lua, _idx);
    uint64_t body_a, body_b;
    if(
        !tryGetBodyIdFromJointDefinition(table, "bodyA", &body_a) ||
        !tryGetBodyIdFromJointDefinition(table, "bodyB", &body_b))
    {
        return false;
    }
    _result.body_a_id = body_a;
    _result.body_b_id = body_b;
    return true;
}
