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
#include <Sol2D/Lua/LuaBodyApi.h>
#include <Sol2D/Lua/LuaColorApi.h>
#include <new>

using namespace Sol2D;
using namespace Sol2D::Lua;

namespace fs = std::filesystem;

namespace {

const char * gc_meta_body_proto_api = "sol.BodyPrototype";

void luaPushBodyPrototypeApiMetatableOntoStack(lua_State * _lua);
int luaApi_LoadSpriteSheet(lua_State * _lua);
int luaApi_AttachScript(lua_State * _lua);

struct Self
{
    Self(const Workspace & _workspace, BodyPrototype & _proto) :
        workspace(_workspace),
        lproto(_proto)
    {
    }

    const Workspace & workspace;
    LuaBodyPrototype lproto;
};

void luaPushBodyPrototypeApiMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_meta_body_proto_api))
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "loadSpriteSheet", luaApi_LoadSpriteSheet },
        { "attachScript", luaApi_AttachScript },
        { nullptr, nullptr }
    };

    luaL_newmetatable(_lua, gc_meta_body_proto_api);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
    luaL_setfuncs(_lua, funcs, 0);
}

// 1 self
// 2 file path
// 3 settings
int luaApi_LoadSpriteSheet(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_meta_body_proto_api));
    const char * path = lua_tolstring(_lua, 2, nullptr);
    luaL_argcheck(_lua, path != nullptr, 2, "sprite sheet path expected");

    SpriteSheetSettings settings = { };

    lua_pushstring(_lua, "spriteWidth");
    if(lua_gettable(_lua, 3) == LUA_TNUMBER)
        settings.sprite_width = static_cast<uint32_t>(lua_tonumber(_lua, -1));
    else
        return luaL_error(_lua, "spriteWidth required");
    lua_pop(_lua, 1);

    lua_pushstring(_lua, "spriteHeight");
    if(lua_gettable(_lua, 3) == LUA_TNUMBER)
        settings.sprite_height = static_cast<uint32_t>(lua_tonumber(_lua, -1));
    else
        return luaL_error(_lua, "spriteHeight");
    lua_pop(_lua, 1);

    lua_pushstring(_lua, "rowCount");
    if(lua_gettable(_lua, 3) == LUA_TNUMBER)
        settings.row_count = static_cast<uint32_t>(lua_tonumber(_lua, -1));
    else
        return luaL_error(_lua, "rowCount required");
    lua_pop(_lua, 1);

    lua_pushstring(_lua, "colCount");
    if(lua_gettable(_lua, 3) == LUA_TNUMBER)
        settings.col_count = static_cast<uint32_t>(lua_tonumber(_lua, -1));
    else
        return luaL_error(_lua, "colCount required");
    lua_pop(_lua, 1);

    lua_pushstring(_lua, "marginTop");
    if(lua_gettable(_lua, 3) == LUA_TNUMBER)
        settings.margin_top = static_cast<uint32_t>(lua_tonumber(_lua, -1));
    lua_pop(_lua, 1);

    lua_pushstring(_lua, "marginLeft");
    if(lua_gettable(_lua, 3) == LUA_TNUMBER)
        settings.margin_left = static_cast<uint32_t>(lua_tonumber(_lua, -1));
    lua_pop(_lua, 1);

    lua_pushstring(_lua, "horizontalSpacing");
    if(lua_gettable(_lua, 3) == LUA_TNUMBER)
        settings.horizintal_spacing = static_cast<uint32_t>(lua_tonumber(_lua, -1));
    lua_pop(_lua, 1);

    lua_pushstring(_lua, "verticalSpacing");
    if(lua_gettable(_lua, 3) == LUA_TNUMBER)
        settings.vertical_spacing = static_cast<uint32_t>(lua_tonumber(_lua, -1));
    lua_pop(_lua, 1);

    lua_pushstring(_lua, "ignores");
    if(lua_gettable(_lua, 3) == LUA_TTABLE)
    {
        lua_Unsigned length = lua_rawlen(_lua, -1);
        for(lua_Unsigned i = 1; i <= length; ++i)
        {
            if(lua_rawgeti(_lua, -1, i) == LUA_TNUMBER)
                settings.ignores.insert(lua_tonumber(_lua, -1) - 1);
            lua_pop(_lua, 1);
        }
    }
    lua_pop(_lua, 1);

    lua_pushstring(_lua, "colorToAlpha");
    lua_gettable(_lua, 3);
    {
        SDL_Color * color;
        if(luaTryGetColor(_lua, -1, &color))
            settings.color_to_alpha = *color;
    }
    lua_pop(_lua, 1);

    bool result = self->lproto.proto.loadSpriteSheet(self->workspace.toAbsolutePath(path), settings);
    lua_pushboolean(_lua, result);
    return 1;
}

// 1 self
// 2 script file
int luaApi_AttachScript(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_meta_body_proto_api));
    const char * path = lua_tolstring(_lua, 2, nullptr);
    luaL_argcheck(_lua, path != nullptr, 2, "script file path expected");
    self->lproto.script_path = self->workspace.toAbsolutePath(path);
    return 0;
}

} // namespace

void Sol2D::Lua::luaPushBodyPrototypeApiOntoStack(lua_State * _lua, const Workspace & _workspace, BodyPrototype & _proto)
{
    void * self = lua_newuserdata(_lua, sizeof(Self));
    new(self) Self(_workspace, _proto);
    luaPushBodyPrototypeApiMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
}

bool Sol2D::Lua::luaTryGetBodyPrototype(lua_State * _lua, int _idx, LuaBodyPrototype ** _lproto)
{
    if(luaL_testudata(_lua, _idx, gc_meta_body_proto_api) == nullptr)
        return false;
    Self * self = static_cast<Self *>(lua_touserdata(_lua, _idx));
    *_lproto = &self->lproto;
    return true;
}
