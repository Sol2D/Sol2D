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
#include <Sol2D/Lua/LuaGraphicsPackDefinitionApi.h>
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
    virtual ~Self() { }
    virtual GraphicsPack * getGraphicsPack(lua_State * _lua) const = 0;
};

struct SelfForBodyShape : Self
{
public:
    SelfForBodyShape(
        std::shared_ptr<Scene> _scene,
        uint64_t _body_id,
        const Utils::PreHashedKey<std::string> & _shape_key,
        const Utils::PreHashedKey<std::string> & _graphics_pack_key
    ) :
        body_id(_body_id),
        shape_key(_shape_key),
        graphics_pack_key(_graphics_pack_key),
        m_scene(_scene)
    {
    }

    GraphicsPack * getGraphicsPack(lua_State * _lua) const override
    {
        return getScene(_lua)->getBodyShapeGraphicsPack(body_id, shape_key, graphics_pack_key);
    }


    std::shared_ptr<Scene> getScene(lua_State * _lua) const
    {
        std::shared_ptr<Scene> ptr =  m_scene.lock();
        if(!ptr)
            luaL_error(_lua, "the scene is destroyed");
        return ptr;
    }

    const uint64_t body_id;
    const Utils::PreHashedKey<std::string> shape_key;
    const Utils::PreHashedKey<std::string> graphics_pack_key;

private:
    std::weak_ptr<Scene> m_scene;
};

using UserData = LuaUserData<Self, LuaTypeName::graphics_pack>;
using UserDataForBodyShape = LuaUserData<SelfForBodyShape, LuaTypeName::graphics_pack>;

// 1 self
// 2 definition (optional)
int luaApi_AddFrame(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    GraphicsPackFrameDefinition definition;
    if(lua_gettop(_lua) > 1)
        tryGetGraphicsPackFrameDefinition(_lua, 2, definition);
    size_t idx = self->getGraphicsPack(_lua)->addFrame(definition);
    lua_pushinteger(_lua, idx);
    return 1;
}

// 1 self
// 2 index
// 2 definition (optional)
int luaApi_InsertFrame(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 2));
    GraphicsPackFrameDefinition definition;
    if(lua_gettop(_lua) > 2)
        tryGetGraphicsPackFrameDefinition(_lua, 3, definition);
    index = self->getGraphicsPack(_lua)->insertFrame(index, definition);
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
int luaApi_GetCurrentFrameIndex(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushinteger(_lua, self->getGraphicsPack(_lua)->getCurrentFrameIndex());
    return 1;
}

// 1 self
// 2 index
int luaApi_SetCurrentFrameIndex(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t index = static_cast<size_t>(lua_tointeger(_lua, 2));
    lua_pushboolean(_lua, self->getGraphicsPack(_lua)->setCurrentFrameIndex(index));
    return 1;
}

// 1 self
int luaApi_SwitchToNextVisibleFrame(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getGraphicsPack(_lua)->switchToNextVisibleFrame());
    return 1;
}

// 1 self
// 2 frame
// 3 sprite definition
int luaApi_AddSprite(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, gc_message_frame_index_expected);
    size_t frame_index = static_cast<size_t>(lua_tointeger(_lua, 2));
    GraphicsPackSpriteDefinition definition;
    luaL_argcheck(_lua, tryGetGraphicsPackSpriteDefinition(_lua, 3, definition), 3, "sprite definition expected");
    self->getGraphicsPack(_lua)->addSprite(frame_index, definition);
    return 0;
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

template<typename UserDataType>
struct GraphicsPackApiPusher
{
    template<typename ...UserDataArgs>
    static void push(lua_State * _lua, UserDataArgs... _args)
    {
        UserDataType::pushUserData(_lua, _args...);
        if(UserDataType::pushMetatable(_lua) == MetatablePushResult::Created)
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
                { "getCurrentFrameIndex", luaApi_GetCurrentFrameIndex },
                { "setCurrentFrameIndex", luaApi_SetCurrentFrameIndex },
                { "switchToNextVisibleFrame", luaApi_SwitchToNextVisibleFrame },
                { "addSprite", luaApi_AddSprite },
                { "removeSprite", luaApi_RemoveSprite },
                { nullptr, nullptr }
            };
            luaL_setfuncs(_lua, funcs, 0);
        }
        lua_setmetatable(_lua, -2);
    }
};



} // namespace

void Sol2D::Lua::pushGraphicsPackApi(
    lua_State * _lua,
    std::shared_ptr<Scene> _scene,
    uint64_t _body_id,
    const Utils::PreHashedKey<std::string> & _shape_key,
    const Utils::PreHashedKey<std::string> & _graphics_pack_key)
{
    GraphicsPackApiPusher<UserDataForBodyShape>::push(_lua, _scene, _body_id, _shape_key, _graphics_pack_key);
}

GraphicsPack * Sol2D::Lua::tryGetGraphicsPack(lua_State * _lua, int _idx)
{
    Self * self = UserData::tryGetUserData(_lua, _idx);
    return self ? self->getGraphicsPack(_lua) : nullptr;
}
