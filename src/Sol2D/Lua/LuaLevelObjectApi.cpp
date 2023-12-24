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

#include <Sol2D/Lua/LuaLevelObjectApi.h>
#include <Sol2D/Lua/LuaRectApi.h>
#include <Sol2D/Lua/LuaPointApi.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Tiles;

namespace {

const char * gc_mtable_level_object = "sol.LevelObject";
const char * gc_obj_type_none = "none";
const char * gc_obj_type_polygon = "polygon";
const char * gc_obj_type_polyline = "polyline";
const char * gc_obj_type_ellipse = "ellipse";
const char * gc_obj_type_point = "point";
const char * gc_obj_type_text = "text";

const char * mapObjectType(TileMapObjectType _type)
{
    switch(_type)
    {
    case TileMapObjectType::polygon:
        return gc_obj_type_polygon;
    case TileMapObjectType::polyline:
        return gc_obj_type_polyline;
    case TileMapObjectType::circle:
        return gc_obj_type_ellipse;
    case TileMapObjectType::point:
        return gc_obj_type_point;
    case TileMapObjectType::text:
        return gc_obj_type_text;
    default:
        return gc_obj_type_none;
    }
}

void luaPushLevelObjectMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_mtable_level_object) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);
    luaL_newmetatable(_lua, gc_mtable_level_object);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
}

} // namespace


void Sol2D::Lua::luaPushLevelObjectOntoStack(lua_State * _lua, const TileMapObject & _object)
{
    lua_newtable(_lua);
    luaPushLevelObjectMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
    lua_pushstring(_lua, "id");
    lua_pushinteger(_lua, _object.getId());
    lua_settable(_lua, -3);
    std::optional<uint32_t> gid = _object.getTileGid();
    if(gid.has_value())
    {
        lua_pushstring(_lua, "tile");
        lua_pushinteger(_lua, gid.value());
        lua_settable(_lua, -3);
    }
    lua_pushstring(_lua, "class");
    lua_pushstring(_lua, _object.getClass().c_str());
    lua_settable(_lua, -3);
    lua_pushstring(_lua, "name");
    lua_pushstring(_lua, _object.getName().c_str());
    lua_settable(_lua, -3);
    lua_pushstring(_lua, "type");
    lua_pushstring(_lua, mapObjectType(_object.getObjectType()));
    lua_settable(_lua, -3);
    lua_pushstring(_lua, "position");
    luaPushPointOntoStack(_lua, _object.getX(), _object.getY());
    lua_settable(_lua, -3);
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
                luaPushPointOntoStack(_lua, point.x, point.y);
                lua_rawseti(_lua, -2, i);
            }
            lua_pushstring(_lua, "points");
            lua_settable(_lua, -3);
        }
    }
}
