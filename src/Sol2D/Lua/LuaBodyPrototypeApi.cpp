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

#include <Sol2D/Lua/LuaBodyPrototypeApi.h>
#include <Sol2D/Lua/LuaBodyShapePrototypeApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaRectApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_message_shape_key_expected[] = "a shape key expected";

struct Self : LuaSelfBase
{
    Self(std::shared_ptr<BodyPrototype> & _proto) :
        proto(_proto)
    {
    }

    std::shared_ptr<BodyPrototype> getBodyPrototype(lua_State * _lua) const
    {
        std::shared_ptr<BodyPrototype> ptr = proto.lock();
        if(!ptr)
            luaL_error(_lua, "the body prototype is destroyed");
        return ptr;
    }

    std::weak_ptr<BodyPrototype> proto;
    std::optional<std::filesystem::path> script_path;
};

using UserData = LuaUserData<Self, LuaTypeName::body_prototype>;

// struct Self : LuaUserData<Self, LuaTypeName::body_prototype>
// {
//     Self(std::shared_ptr<BodyPrototype> & _proto) :
//         proto(_proto)
//     {
//     }

//     std::shared_ptr<BodyPrototype> getBodyPrototype(lua_State * _lua) const
//     {
//         std::shared_ptr<BodyPrototype> ptr = proto.lock();
//         if(!ptr)
//             luaL_error(_lua, "the body prototype is destroyed");
//         return ptr;
//     }

//     std::weak_ptr<BodyPrototype> proto;
//     std::optional<std::filesystem::path> script_path;
// };

// 1 self
int luaApi_GetType(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    BodyType type = self->getBodyPrototype(_lua)->getType();
    lua_pushinteger(_lua, static_cast<lua_Integer>(type));
    return 1;
}

// 1 self
// 2 key
// 3 position
// 4 radius
int luaApi_CreateCircleShape(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_shape_key_expected);
    Point position;
    luaL_argcheck(_lua, tryGetPoint(_lua, 3, position), 3, "the circle center position expected");
    luaL_argcheck(_lua, lua_isnumber(_lua, 4), 4, "the circle radius expected");
    float radius = static_cast<float>(lua_tonumber(_lua, 4));
    pushBodyShapePrototypeApi(_lua, self->getBodyPrototype(_lua)->createCircleShape(key, position, radius));
    return 1;
}

// 1 self
// 2 key
// 3 rect | or ... points
int luaApi_CreatePolygonShape(lua_State * _lua) // TODO: split up: createBoxShape, replace points with array
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, gc_message_shape_key_expected);
    Rect rect;
    Point point;
    BodyShapePrototype * shape_prototype = nullptr;
    if(tryGetRect(_lua, 3, rect))
    {
        shape_prototype = &self->getBodyPrototype(_lua)->createPolygonShape(key, rect);
    }
    else if(tryGetPoint(_lua, 3, point))
    {
        int points_count = lua_gettop(_lua) - 1;
        if(points_count < 3)
            luaL_error(_lua, "a polygon requires at least 3 points, %d provided", points_count);
        std::vector<Point> points(points_count);
        points[0] = point;
        for(int i = 1; i < points_count; ++i)
        {
            int idx = i + 2;
            luaL_argcheck(_lua, tryGetPoint(_lua, idx, point), idx, "a point expected");
            points[i] = point;
        }
        shape_prototype = &self->getBodyPrototype(_lua)->createPolygonShape(key, points);
    }
    else
    {
        luaL_argcheck(_lua, false, 3, "a rect or position expected");
        return 0; // Only for static code analyzers. This will never happen, luaL_argcheck will never return.
    }
    pushBodyShapePrototypeApi(_lua, *shape_prototype);
    return 1;
}

// 1 self
// 2 script file
int luaApi_AttachScript(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * path = lua_tolstring(_lua, 2, nullptr);
    luaL_argcheck(_lua, path != nullptr, 2, "a script file path expected");
    self->script_path = path;
    return 0;
}

} // namespace name

void Sol2D::Lua::pushBodyPrototypeApi(lua_State * _lua, std::shared_ptr<BodyPrototype> _body_prototype)
{
    UserData::pushUserData(_lua, _body_prototype);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            { "__gc", UserData::luaGC },
            { "getType", luaApi_GetType },
            { "createCircleShape", luaApi_CreateCircleShape },
            { "createPolygonShape", luaApi_CreatePolygonShape },
            { "attachScript", luaApi_AttachScript },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::optional<LuaBodyPrototype> Sol2D::Lua::tryGetBodyPrototype(lua_State * _lua, int _idx)
{
    Self * self = UserData::getUserData(_lua, _idx);
    if(self)
    {
        LuaBodyPrototype result;
        result.proto = self->proto.lock();
        if(result.proto)
        {
            result.script_path = self->script_path;
            return result;
        }
    }
    return std::optional<LuaBodyPrototype>();
}
