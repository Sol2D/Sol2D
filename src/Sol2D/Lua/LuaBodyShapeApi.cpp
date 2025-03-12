// Sol2D Game Engine
// Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
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
#include <Sol2D/Lua/LuaBodyApi.h>
#include <Sol2D/Lua/LuaBodyShapeApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>
#include <Sol2D/Lua/Aux/LuaUtils.h>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Lua;
using namespace Sol2D::Utils;

namespace {

struct Self : LuaSelfBase
{
    Self(std::shared_ptr<Scene> _scene, uint64_t _body_id, const PreHashedKey<std::string> & _shape_key) :
        body_id(_body_id),
        shape_key(_shape_key),
        m_scene(_scene)
    {
    }

    std::shared_ptr<Scene> getScene(lua_State * _lua) const
    {
        std::shared_ptr<Scene> ptr = m_scene.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::scene_is_destroyed);
        return ptr;
    }

    const uint64_t body_id;
    const PreHashedKey<std::string> shape_key;

private:
    std::weak_ptr<Scene> m_scene;
};

using UserData = LuaUserData<Self, LuaTypeName::body_shape>;

// 1 self
int luaApi_IsValid(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getScene(_lua)->doesBodyShapeExist(self->body_id, self->shape_key));
    return 1;
}

// 1 self
int luaApi_GetKey(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    lua_pushstring(_lua, self->shape_key.key.c_str());
    return 1;
}

// 1 self
int luaApi_GetBody(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    pushBodyApi(_lua, self->getScene(_lua), self->body_id);
    return 1;
}

// 1 self
// 2 graphic key
int luaApi_GetGraphicsPack(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * graphic_key = argToStringOrError(_lua, 2);
    const GraphicsPack * pack = self->getScene(_lua)->getBodyShapeGraphicsPack(
        self->body_id, self->shape_key, makePreHashedKey(std::string(graphic_key))
    );
    if(pack)
    {
        pushGraphicsPackApi(
            _lua, self->getScene(_lua), self->body_id, self->shape_key, makePreHashedKey(std::string(graphic_key))
        );
    }
    else
    {
        lua_pushnil(_lua);
    }
    return 1;
}

// 1 self
int luaApi_GetCurrentGraphicsPack(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    Body * body = self->getScene(_lua)->getBody(self->body_id);
    if(body)
    {
        BodyShape * shape = body->findShape(self->shape_key);
        if(shape)
        {
            std::optional<PreHashedKey<std::string>> key = shape->getCurrentGraphicsKey();
            if(key.has_value())
            {
                pushGraphicsPackApi(_lua, self->getScene(_lua), self->body_id, self->shape_key, key.value());
                return 1;
            }
        }
    }
    lua_pushnil(_lua);
    return 1;
}

// 1 self
// 2 graphic key
int luaApi_SetCurrentGraphics(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * graphic_key = argToStringOrError(_lua, 2);
    lua_pushboolean(
        _lua,
        self->getScene(_lua)->setBodyShapeCurrentGraphics(
            self->body_id, self->shape_key, makePreHashedKey(std::string(graphic_key))
        )
    );
    return 1;
}

// 1 self
// 2 graphic key
// 3 flip horizontally
// 4 flip vertically
int luaApi_FlipGraphics(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    const char * graphic_key = argToStringOrError(_lua, 2);
    luaL_checktype(_lua, 3, LUA_TBOOLEAN);
    luaL_checktype(_lua, 4, LUA_TBOOLEAN);
    bool result = self->getScene(_lua)->flipBodyShapeGraphics(
        self->body_id,
        self->shape_key,
        makePreHashedKey(std::string(graphic_key)),
        lua_toboolean(_lua, 3),
        lua_toboolean(_lua, 4)
    );
    lua_pushboolean(_lua, result);
    return 1;
}

} // namespace

void Sol2D::Lua::pushBodyShapeApi(
    lua_State * _lua,
    std::shared_ptr<Scene> _scene,
    uint64_t _body_id,
    const Utils::PreHashedKey<std::string> & _shape_key
)
{
    UserData::pushUserData(_lua, _scene, _body_id, _shape_key);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] = {
            {"__gc",                   UserData::luaGC              },
            {"isValid",                luaApi_IsValid               },
            {"getKey",                 luaApi_GetKey                },
            {"getBody",                luaApi_GetBody               },
            {"getGraphicsPack",        luaApi_GetGraphicsPack       },
            {"getCurrentGraphicsPack", luaApi_GetCurrentGraphicsPack},
            {"setCurrentGraphics",     luaApi_SetCurrentGraphics    },
            {"flipGraphics",           luaApi_FlipGraphics          },
            {nullptr,                  nullptr                      }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}
