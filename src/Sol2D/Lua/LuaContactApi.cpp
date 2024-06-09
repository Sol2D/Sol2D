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
#include <Sol2D/Lua/Aux/LuaTopStackTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

void pushContactSide(lua_State * _lua, const ContactSide & _side)
{
    static const char key_body[] = "bodyId";
    static const char key_shape[] = "shapeKey";
    static const char key_tile_map_object_id[] = "tileMapObjectId";

    LuaTopStackTable side_a_table = LuaTopStackTable::pushNew(_lua);
    side_a_table.setIntegerValue(key_body, _side.body_id);
    side_a_table.setStringValue(key_shape, _side.shape_key);
    if(_side.tile_map_object_id.has_value())
        side_a_table.setIntegerValue(key_tile_map_object_id, _side.tile_map_object_id.value());
}

} // namespace name

void Sol2D::Lua::pushContact(lua_State * _lua, Contact & _contact)
{
    LuaTopStackTable contact_table = LuaTopStackTable::pushNew(_lua);
    pushContactSide(_lua, _contact.side_a);
    contact_table.setValueFromTop("sideA");
    pushContactSide(_lua, _contact.side_b);
    contact_table.setValueFromTop("sideB");
}
