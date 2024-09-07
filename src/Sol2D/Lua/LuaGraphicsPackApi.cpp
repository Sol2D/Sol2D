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

#include <Sol2D/Lua/LuaGraphicsPackApi.h>
#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteSheetApi.h>
#include <Sol2D/Lua/LuaPointApi.h>
#include <Sol2D/Lua/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_message_frame_index_expected[] = "frame index expected";
const char gc_message_sprite_index_expected[] = "sprite index expected";

struct Self : LuaSelfBase
{
    explicit Self(std::shared_ptr<GraphicsPack> & _graphics_pack) :
        graphics_pack(_graphics_pack)
    {
    }

    std::shared_ptr<GraphicsPack> getGraphicsPack(lua_State * _lua) const
    {
        std::shared_ptr<GraphicsPack> ptr = graphics_pack.lock();
        if(!ptr)
            luaL_error(_lua, "the graphics pack is destroyed");
        return ptr;
    }

    std::weak_ptr<GraphicsPack> graphics_pack;
};

using UserData = LuaUserData<Self, LuaTypeName::graphics_pack>;

void readGraphicsPackFrameOptions(lua_State * _lua, int _idx, GraphicsPackFrameOptions & _options)
{
    if(!lua_istable(_lua, _idx))
        return;

    LuaTable table(_lua, _idx);

    {
        lua_Integer duration;
        if(table.tryGetInteger("duration", &duration))
            _options.duration = std::chrono::milliseconds(duration);
    }

    {
        bool is_visible;
        if(table.tryGetBoolean("isVisible", &is_visible))
            _options.is_visible = is_visible;
    }
}

void readGraphicsPackSpriteOptions(lua_State * _lua, int _idx, GraphicsPackSpriteOptions & _options)
{
    if(!lua_istable(_lua, _idx))
        return;

    LuaTable table(_lua, _idx);

    if(table.tryGetValue("position"))
    {
        Point point;
        if(tryGetPoint(_lua, -1, point))
            _options.position = point;
        lua_pop(_lua, 1);

        if(tryGetPoint(_lua, -1, point))
            _options.flip_center = point;
        lua_pop(_lua, 1);
    }

    {
        lua_Number value;
        if(table.tryGetNumber("angle", &value))
            _options.angle_rad = static_cast<float>(value);
    }

    {
        bool value;
        if(table.tryGetBoolean("isFlippedHorizontally", &value))
            _options.is_flipped_horizontally = value;
        if(table.tryGetBoolean("isFlippedVertically", &value))
            _options.is_flipped_vertically = value;
        if(table.tryGetBoolean("isVisible", &value))
            _options.is_visible = value;
    }
}

// 1 self
// 2 options (optional)
int luaApi_AddFrame(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    GraphicsPackFrameOptions options;
    if(lua_gettop(_lua) > 1)
        readGraphicsPackFrameOptions(_lua, 2, options);
    size_t idx = self->getGraphicsPack(_lua)->addFrame(options);
    lua_pushinteger(_lua, idx);
    return 1;
}

// 1 self
// 2 index
// 3 options (optional)
int luaApi_InsertFrame(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 2));
    GraphicsPackFrameOptions options;
    if(lua_gettop(_lua) > 2)
        readGraphicsPackFrameOptions(_lua, 3, options);
    index = self->getGraphicsPack(_lua)->insertFrame(index, options);
    lua_pushinteger(_lua, index);
    return 1;
}

// 1 self
// 2 index
int luaApi_RemoveFrame(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 2));
    bool result = self->getGraphicsPack(_lua)->removeFrame(index);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 index
// 3 visibility
int luaApi_SetFrameVisibility(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    luaL_argcheck(_lua, lua_isboolean(_lua, 3), 3, "visibility value expected");
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 2));
    bool is_visible = static_cast<bool>(lua_toboolean(_lua, 3));
    bool result = self->getGraphicsPack(_lua)->setFrameVisibility(index, is_visible);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 index
int luaApi_IsFrameVisible(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 2));
    std::optional<bool> result = self->getGraphicsPack(_lua)->isFrameVisible(index);
    if(result.has_value())
        lua_pushboolean(_lua, result.value());
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 index
// 3 duration
int luaApi_SetFrameDuration(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, "duration value expected");
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 2));
    int64_t duration = static_cast<int64_t>(lua_tointeger(_lua, 3));
    bool result = self->getGraphicsPack(_lua)->setFrameDuration(index, std::chrono::milliseconds(duration));
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 index
int luaApi_GetFrameDuration(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 2));
    std::optional<std::chrono::milliseconds> result = self->getGraphicsPack(_lua)->getFrameDuration(index);
    if(result.has_value())
        lua_pushinteger(_lua, result.value().count());
    else
        lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 frame
// 3 sprite                     or   sprite sheet
// 4 sprite options (optional)  or   sprite inedx
// 5 —                          or   sprite options (optional)
int luaApi_AddSprite(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t frame_index = static_cast<size_t>(lua_tointeger(_lua, 2));
    if(std::shared_ptr<Sprite> sprite = tryGetSprite(_lua, 3))
    {
        GraphicsPackSpriteOptions options;
        if(lua_gettop(_lua) > 3)
            readGraphicsPackSpriteOptions(_lua, 4, options);
        auto result = self->getGraphicsPack(_lua)->addSprite(frame_index, *sprite, options);
        lua_pushboolean(_lua, result.first);
        lua_pushinteger(_lua, result.second);
        return 2;
    }
    if(std::shared_ptr<SpriteSheet> sprite_sheet = tryGetSpriteSheet(_lua, 3))
    {
        luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, gc_message_sprite_index_expected);
        size_t sprite_index = static_cast<size_t>(lua_tointeger(_lua, 3));
        GraphicsPackSpriteOptions options;
        if(lua_gettop(_lua) > 4)
            readGraphicsPackSpriteOptions(_lua, 5, options);
        auto result = self->getGraphicsPack(_lua)->addSprite(frame_index, *sprite_sheet, sprite_index, options);
        lua_pushboolean(_lua, result.first);
        lua_pushinteger(_lua, result.second);
        return 2;
    }
    luaL_argerror(_lua, 3, "sprite or sprite sheet expected");
    return 0;
}

// 1 self
// 2 frame
// 3 sprite sheet
// 4 sprite indices
// 5 options (optional)
int luaApi_AddSprites(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t frame_index = static_cast<size_t>(lua_tointeger(_lua, 2));
    std::shared_ptr<SpriteSheet> sprite_sheet = tryGetSpriteSheet(_lua, 3);
    luaL_argcheck(_lua, sprite_sheet, 3, "sprite sheet expected");
    luaL_argcheck(_lua, lua_istable(_lua, 4), 4, "sprite indices array expected");
    size_t index_count = lua_rawlen(_lua, 4);
    if(index_count == 0)
    {
        lua_pushboolean(_lua, false);
        lua_pushinteger(_lua, 0);
        return 2;
    }
    std::vector<size_t> indices(index_count);
    for(size_t i = 1; i <= index_count; ++i)
    {
        lua_rawgeti(_lua, 4, i);
        if(!lua_isinteger(_lua, -1))
            luaL_error(_lua, "the item %d of the indices array is not an integer", i);
        indices[i - 1] = static_cast<size_t>(lua_tointeger(_lua, -1));
        lua_pop(_lua, 1);
    }
    GraphicsPackSpriteOptions options;
    if(lua_gettop(_lua) > 4)
        readGraphicsPackSpriteOptions(_lua, 5, options);
    auto result = self->getGraphicsPack(_lua)->addSprites(frame_index, *sprite_sheet, indices, options);
    lua_pushboolean(_lua, result.first);
    lua_pushinteger(_lua, result.second);
    return 2;
}

// 1 self
// 2 frame
// 3 sprite
int luaApi_RemoveSprite(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t frame_index = static_cast<size_t>(lua_tointeger(_lua, 2));
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, gc_message_sprite_index_expected);
    size_t sprite_index = static_cast<size_t>(lua_tointeger(_lua, 3));
    bool result = self->getGraphicsPack(_lua)->removeSprite(frame_index, sprite_index);
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushGraphicsPackApi(lua_State * _lua, std::shared_ptr<GraphicsPack> _graphics_pack)
{
    UserData::pushUserData(_lua, _graphics_pack);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "addFrame", luaApi_AddFrame },
            { "insertFrame", luaApi_InsertFrame },
            { "removeFrame", luaApi_RemoveFrame },
            { "setFrameVisibility", luaApi_SetFrameVisibility },
            { "isFrameVisible", luaApi_IsFrameVisible },
            { "setFrameDuration", luaApi_SetFrameDuration },
            { "getFrameDuration", luaApi_GetFrameDuration },
            { "addSprite", luaApi_AddSprite },
            { "addSprites", luaApi_AddSprites },
            { "removeSprite", luaApi_RemoveSprite },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<GraphicsPack> Sol2D::Lua::tryGetGraphicsPack(lua_State * _lua, int _idx)
{
    Self * self = UserData::getUserData(_lua, _idx);
    return self ? self->graphics_pack.lock() : nullptr;
}
