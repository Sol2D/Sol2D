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

#include <Sol2D/Lua/LuaContactApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/LuaManifoldApi.h>

using namespace Sol2D::World;
using namespace Sol2D::Lua;

namespace {

const char gc_key_side_a[] = "sideA";
const char gc_key_side_b[] = "sideB";

void pushContactSide(lua_State * _lua, const ContactSide & _side)
{
    static const char key_body[] = "bodyId";
    static const char key_shape[] = "shapeKey";
    static const char key_tile_map_object_id[] = "tileMapObjectId";

    LuaTable side_a_table = LuaTable::pushNew(_lua);
    side_a_table.setIntegerValue(key_body, _side.body_id);
    side_a_table.setStringValue(key_shape, _side.shape_key);
    if(_side.tile_map_object_id.has_value())
        side_a_table.setIntegerValue(key_tile_map_object_id, _side.tile_map_object_id.value());
}

void setContactSide(LuaTable & _table, const char * _key, const ContactSide & _side)
{
    pushContactSide(_table.getLua(), _side);
    _table.setValueFromTop(_key);
}

} // namespace name

void Sol2D::Lua::pushContact(lua_State * _lua, const Contact & _contact)
{
    LuaTable contact_table = LuaTable::pushNew(_lua);
    setContactSide(contact_table, gc_key_side_a, _contact.side_a);
    setContactSide(contact_table, gc_key_side_b, _contact.side_b);
}

void Sol2D::Lua::pushContact(lua_State * _lua, const SensorContact & _contact)
{
    LuaTable contact_table = LuaTable::pushNew(_lua);
    setContactSide(contact_table, "sensor", _contact.sensor);
    setContactSide(contact_table, "visitor", _contact.visitor);
}

void Sol2D::Lua::pushContact(lua_State * _lua, const PreSolveContact & _contact)
{
    LuaTable contact_table = LuaTable::pushNew(_lua);
    setContactSide(contact_table, gc_key_side_a, _contact.side_a);
    setContactSide(contact_table, gc_key_side_b, _contact.side_b);
    pushManifold(_lua, *_contact.manifold);
    contact_table.setValueFromTop("manifold");
}
