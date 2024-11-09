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

#include <Sol2D/Lua/LuaSpriteApi.h>
#include <Sol2D/Lua/LuaSpriteOptionsApi.h>
#include <Sol2D/Lua/LuaRectApi.h>
#include <Sol2D/Lua/LuaSizeApi.h>
#include <Sol2D/Lua/Aux/LuaStrings.h>
#include <Sol2D/Lua/Aux/LuaUserData.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace {

struct Self : LuaSelfBase
{
    Self(const Workspace & _workspace, std::shared_ptr<Sprite> & _sprite) :
        workspace(_workspace),
        sprite(_sprite)
    {
    }

    std::shared_ptr<Sprite> getSprite(lua_State * _lua) const
    {
        std::shared_ptr<Sprite> ptr = sprite.lock();
        if(!ptr)
            luaL_error(_lua, LuaMessage::sprite_is_destroyed);
        return ptr;
    }

    const Workspace & workspace;
    std::weak_ptr<Sprite> sprite;
};

using UserData = LuaUserData<Self, LuaTypeName::sprite>;

// 1 self
// 2 path
// 3 options (optional)
int luaApi_LoadFromFile(lua_State * _lua)
{
    Self * self = UserData::getUserData(_lua, 1);
    luaL_argexpected(_lua, lua_isstring(_lua, 2), 2, LuaTypeName::string);
    const char * path = lua_tostring(_lua, 2);    
    SpriteOptions options;
    tryGetSpriteOptions(_lua, 3, options);
    bool result = self->getSprite(_lua)->loadFromFile(self->workspace.getResourceFullPath(path), options);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
int luaApi_IsValid(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    lua_pushboolean(_lua, self->getSprite(_lua)->isValid());
    return 1;
}

// 1 self
int luaApi_GetSourceRect(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    pushRect(_lua, self->getSprite(_lua)->getSourceRect());
    return 1;
}

// 1 self
int luaApi_GetDestinationSize(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    const Size & size = self->getSprite(_lua)->getDestinationSize();
    pushSize(_lua, size);
    return 1;
}

// 1 self
// 2 size
int luaApi_SetDestinationSize(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    Size size;
    luaL_argexpected(_lua, tryGetSize(_lua, 2, size), 2, LuaTypeName::size);
    self->getSprite(_lua)->setDesinationSize(size);
    return 0;
}

// 1 self
// 2 scale factor or scale factor X (optional)
// 3 scale factor Y (optional)
int luaApi_Scale(lua_State * _lua)
{
    const Self * self = UserData::getUserData(_lua, 1);
    if(lua_isnumber(_lua, 2))
    {
        float scale_factor = static_cast<float>(lua_tonumber(_lua, 2));
        if(lua_isnumber(_lua, 3))
            self->getSprite(_lua)->scale(scale_factor, static_cast<float>(lua_tonumber(_lua, 3)));
        else
            self->getSprite(_lua)->scale(scale_factor);
    }
    else
    {
        self->getSprite(_lua)->scale(1.0f);
    }
    return 0;
}

} // namespace name

void Sol2D::Lua::pushSpriteApi(lua_State * _lua, const Workspace & _workspace, std::shared_ptr<Sprite> _sprite)
{
    UserData::pushUserData(_lua, _workspace, _sprite);
    if(UserData::pushMetatable(_lua) == MetatablePushResult::Created)
    {
        luaL_Reg funcs[] =
        {
            { "__gc", UserData::luaGC },
            { "loadFromFile", luaApi_LoadFromFile },
            { "isValid", luaApi_IsValid },
            { "getSourceRect", luaApi_GetSourceRect },
            { "getDestinationSize", luaApi_GetDestinationSize },
            { "setDestinationSize", luaApi_SetDestinationSize },
            { "scale", luaApi_Scale },
            { nullptr, nullptr }
        };
        luaL_setfuncs(_lua, funcs, 0);
    }
    lua_setmetatable(_lua, -2);
}

std::shared_ptr<Sprite> Sol2D::Lua::tryGetSprite(lua_State * _lua, int _idx)
{
    Self * self = UserData::tryGetUserData(_lua, _idx);
    return self ? self->sprite.lock() : nullptr;
}
