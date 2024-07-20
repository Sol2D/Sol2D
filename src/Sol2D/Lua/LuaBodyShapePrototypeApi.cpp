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

#include <Sol2D/Lua/LuaBodyShapePrototypeApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaSizeApi.h>
#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteAnimationApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/LuaStrings.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

struct BodyPolygonShapePrototypeSelf : LuaSelfBase
{
    explicit BodyPolygonShapePrototypeSelf(std::shared_ptr<BodyPolygonShapePrototype> & _prototype) :
        shape_prototype(_prototype)
    {
    }

    std::shared_ptr<BodyPolygonShapePrototype> getShapePrototype(lua_State * _lua) const
    {
        std::shared_ptr<BodyPolygonShapePrototype> ptr = shape_prototype.lock();
        if(!ptr)
            luaL_error(_lua, "the polygon shape prototype is destroyed");
        return ptr;
    }

    std::weak_ptr<BodyPolygonShapePrototype> shape_prototype;
};

using BodyPolygonShapePrototypeUserData = LuaUserData<BodyPolygonShapePrototypeSelf, LuaTypeName::body_polygon_shape_prototype>;

struct BodyCircleShapePrototypeSelf : LuaSelfBase
{
    explicit BodyCircleShapePrototypeSelf(std::shared_ptr<BodyCircleShapePrototype> & _prototype) :
        shape_prototype(_prototype)
    {
    }

    std::shared_ptr<BodyCircleShapePrototype> getShapePrototype(lua_State * _lua) const
    {
        std::shared_ptr<BodyCircleShapePrototype> ptr = shape_prototype.lock();
        if(!ptr)
            luaL_error(_lua, "the circle shape prototype is destroyed");
        return ptr;
    }

    std::weak_ptr<BodyCircleShapePrototype> shape_prototype;
};

using BodyCircleShapePrototypeUserData = LuaUserData<BodyCircleShapePrototypeSelf, LuaTypeName::body_circle_shape_prototype>;


std::shared_ptr<BodyShapePrototype> getBodyShapePrototype(lua_State * _lua, int _idx)
{
    void * user_data = luaL_testudata(_lua, _idx, LuaTypeName::body_polygon_shape_prototype);
    if(user_data == nullptr)
    {
        user_data = luaL_testudata(_lua, _idx, LuaTypeName::body_circle_shape_prototype);
        if(user_data == nullptr)
        {
            std::stringstream ss;
            ss << LuaTypeName::body_polygon_shape_prototype << " or "
               << LuaTypeName::body_circle_shape_prototype << "expected";
            luaL_argerror(_lua, _idx, ss.str().c_str());
        }
        else
        {
            return static_cast<BodyCircleShapePrototypeSelf *>(user_data)->getShapePrototype(_lua);
        }
    }
    else
    {
        return static_cast<BodyPolygonShapePrototypeSelf *>(user_data)->getShapePrototype(_lua);
    }
    return nullptr;
}

bool tryReadShapeGraphicOptions(lua_State * _lua, int _idx, BodyShapeGraphicOptions & _options)
{
    if(!lua_istable(_lua, _idx))
        return false;

    LuaTable table(_lua, _idx);
    table.tryGetBoolean("isFlippedHorizontally", &_options.is_flipped_horizontally);
    table.tryGetBoolean("isFlippedVertically", &_options.is_flipped_vertically);
    if(table.tryGetValue("position"))
    {
        tryGetPoint(_lua, -1, _options.position);
        lua_pop(_lua, 1);
    }

    return true;
}

// 1 self
int luaApi_GetType(lua_State * _lua)
{
    auto prototype = getBodyShapePrototype(_lua, 1);
    lua_pushinteger(_lua, static_cast<lua_Integer>(prototype->getType()));
    return 1;
}

// 1 self
// 2 is sensor?
int luaApi_SetIsSensor(lua_State * _lua)
{
    auto prototype = getBodyShapePrototype(_lua, 1);
    luaL_argcheck(_lua, lua_isboolean(_lua, 2), 2, "the is sensor flag expected");
    prototype->setIsSensor(lua_toboolean(_lua, 2));
    return 0;
}

// 1 self
int luaApi_IsSensor(lua_State * _lua)
{
    auto prototype = getBodyShapePrototype(_lua, 1);
    lua_pushboolean(_lua, prototype->isSensor());
    return 1;
}

// 1 self
// 2 key
// 3 sprite
// 4 options (optional)
int luaApi_AddSprite(lua_State * _lua)
{
    auto prototype = getBodyShapePrototype(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, "a sprite key expected");
    std::shared_ptr<Sprite> sprite = tryGetSprite(_lua, 3);
    luaL_argcheck(_lua, sprite, 3, "a sprite expected");
    BodyShapeGraphicOptions options;
    tryReadShapeGraphicOptions(_lua, 4, options);
    prototype->addGraphic(key, *sprite, options);
    return 0;
}

// 1 self
// 2 key
// 3 sprite animation
// 4 options (optional)
int luaApi_AddSpriteAnimation(lua_State * _lua)
{
    auto prototype = getBodyShapePrototype(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, "a sprite animation key expected");
    std::shared_ptr<SpriteAnimation> sprite_animation = tryGetSpriteAnimation(_lua, 3);
    luaL_argcheck(_lua, sprite_animation, 3, "a sprite animation expected");
    BodyShapeGraphicOptions options;
    tryReadShapeGraphicOptions(_lua, 4, options);
    prototype->addGraphic(key, *sprite_animation, options);
    return 0;
}

// 1 self
// 2 key
int luaApi_RemoveGraphic(lua_State * _lua)
{
    auto prototype = getBodyShapePrototype(_lua, 1);
    const char * key = lua_tostring(_lua, 2);
    luaL_argcheck(_lua, key != nullptr, 2, "a graphic key expected");
    prototype->removeGraphic(key);
    return 0;
}

// 1 self (LuaBodyPolygonShapePrototypeApi)
int luaApi_GetPoints(lua_State * _lua)
{
    BodyPolygonShapePrototypeSelf * self = BodyPolygonShapePrototypeUserData::getUserData(_lua, 1);
    const auto & points = self->getShapePrototype(_lua)->getPoints();
    lua_newtable(_lua);
    for(size_t i = 0; i < points.size(); ++i)
    {
        pushPoint(_lua, points[i].x, points[i].y);
        lua_rawseti(_lua, -2, i);
    }
    return 1;
}

// 1 self (LuaBodyCircleShapePrototypeApi)
int luaApi_GetRadius(lua_State * _lua)
{
    BodyCircleShapePrototypeSelf * self = BodyCircleShapePrototypeUserData::getUserData(_lua, 1);
    lua_pushnumber(_lua, self->getShapePrototype(_lua)->getRadius());
    return 1;
}

} // namespace name

void Sol2D::Lua::pushBodyShapePrototypeApi(lua_State * _lua, std::shared_ptr<BodyShapePrototype> _body_shape_prototype)
{
    std::vector<luaL_Reg> funcs;
    MetatablePushResult metatable_push_result;
    switch(_body_shape_prototype->getType())
    {
    case BodyShapeType::Polygon:
    {
        BodyPolygonShapePrototypeUserData::pushUserData(
            _lua,
            std::dynamic_pointer_cast<BodyPolygonShapePrototype>(_body_shape_prototype));
        metatable_push_result = BodyPolygonShapePrototypeUserData::pushMetatable(_lua);
        funcs =
        {
            { "__gc", BodyPolygonShapePrototypeUserData::luaGC },
            { "getPoints", luaApi_GetPoints }
        };
        break;
    }
    case BodyShapeType::Circle:
    {
        BodyCircleShapePrototypeUserData::pushUserData(
            _lua,
            std::dynamic_pointer_cast<BodyCircleShapePrototype>(_body_shape_prototype));
        metatable_push_result = BodyCircleShapePrototypeUserData::pushMetatable(_lua);
        funcs =
        {
            { "__gc", BodyCircleShapePrototypeUserData::luaGC },
            { "getRadius", luaApi_GetRadius }
        };
        break;
    }
    default:
        throw std::runtime_error("Unknown type of the body shape prototype");
    }
    if(metatable_push_result == MetatablePushResult::Created)
    {
        funcs.insert(funcs.end(),
        {
            { "addSprite", luaApi_AddSprite },
            { "addSpriteAnimation", luaApi_AddSpriteAnimation },
            { "getType", luaApi_GetType },
            { "setIsSensor", luaApi_SetIsSensor },
            { "isSensor", luaApi_IsSensor },
            { "removeGraphic", luaApi_RemoveGraphic },
            { nullptr, nullptr }
        });
        luaL_setfuncs(_lua, funcs.data(), 0);
    }
    lua_setmetatable(_lua, -2);
}
