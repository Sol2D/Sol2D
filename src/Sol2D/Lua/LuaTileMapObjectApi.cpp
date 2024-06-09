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

#include <Sol2D/Lua/LuaTileMapObjectApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D::Tiles;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_metatable_tile_map_object_type[] = "sol.TileMapObjectType";

} // namespace

void Sol2D::Lua::pushTileMapObjectTypeEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, gc_metatable_tile_map_object_type) == MetatablePushResult::Created)
    {
        LuaTable table(_lua);
        table.setIntegerValue("CIRCLE", static_cast<lua_Integer>(TileMapObjectType::Circle));
        table.setIntegerValue("POINT", static_cast<lua_Integer>(TileMapObjectType::Point));
        table.setIntegerValue("POLYGON", static_cast<lua_Integer>(TileMapObjectType::Polygon));
        table.setIntegerValue("POLYLINE", static_cast<lua_Integer>(TileMapObjectType::Polyline));
        table.setIntegerValue("TEXT", static_cast<lua_Integer>(TileMapObjectType::Text));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushTileMapObject(lua_State * _lua, const TileMapObject & _object)
{
    LuaTable table = LuaTable::pushNew(_lua);
    table.setIntegerValue("id", _object.getId());
    std::optional<uint32_t> gid = _object.getTileGid();
    if(gid.has_value())
        table.setIntegerValue("tile", gid.value());
    table.setStringValue("class", _object.getClass());
    table.setStringValue("name", _object.getName());
    table.setIntegerValue("type", static_cast<lua_Integer>(_object.getObjectType()));
    pushPoint(_lua, _object.getX(), _object.getY());
    table.setValueFromTop("position");
    if(const TileMapPolyX * poly = dynamic_cast<const TileMapPolyX *>(&_object))
    {
         const std::vector<SDL_FPoint> & points = poly->getPoints();
         if(!points.empty())
         {
             lua_newtable(_lua);
             size_t count = points.size();
             for(size_t i = 0; i < count; ++i)
             {
                 const SDL_FPoint & point = points[i];
                 pushPoint(_lua, point.x, point.y);
                 lua_rawseti(_lua, -2, i + 1);
             }
             table.setValueFromTop("points");
         }
    }
}
