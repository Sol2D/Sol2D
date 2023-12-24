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

#include <Sol2D/Lua/LuaColorApi.h>
#include <cstring>

using namespace Sol2D::Lua;

namespace {

const char * gc_meta_color_api = "sol.Color";
const char * gc_meta_color_value = "sol.Color.Color";

void luaPushColorValueMetatableOntoStack(lua_State * _lua);
void luaPushColorApiMetatableOntoStack(lua_State * _lua);
int luaApi_CreateColorFromRGBA(lua_State * _lua);
int luaApi_CreateColorFromRGB(lua_State * _lua);
int luaApi_SetColorRed(lua_State * _lua);
int luaApi_SetColorGreen(lua_State * _lua);
int luaApi_SetColorBlue(lua_State * _lua);
int luaApi_SetColorAlpha(lua_State * _lua);
int luaApi_GetColorRed(lua_State * _lua);
int luaApi_GetColorGreen(lua_State * _lua);
int luaApi_GetColorBlue(lua_State * _lua);
int luaApi_GetColorAlpha(lua_State * _lua);

void luaPushColorApiMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_meta_color_api) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "fromRGBA", luaApi_CreateColorFromRGBA },
        { "fromRGB", luaApi_CreateColorFromRGB },
        { nullptr, nullptr }
    };
    luaL_newmetatable(_lua, gc_meta_color_api);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
    luaL_setfuncs(_lua, funcs, 0);
}

// 1 red
// 2 green
// 3 blue
// 4 alpha
int luaApi_CreateColorFromRGBA(lua_State * _lua)
{
    luaL_argcheck(_lua, lua_isinteger(_lua, 1), 1, "the red component is required and must be integer");
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "the green component is required and must be integer");
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, "the blue component is required and must be integer");
    luaL_argcheck(_lua, lua_isinteger(_lua, 4), 4, "the alpha component is required and must be integer");
    SDL_Color color;
    color.r = static_cast<Uint8>(lua_tointeger(_lua, 1));
    color.g = static_cast<Uint8>(lua_tointeger(_lua, 2));
    color.b = static_cast<Uint8>(lua_tointeger(_lua, 3));
    color.a = static_cast<Uint8>(lua_tointeger(_lua, 4));
    void * buffer = lua_newuserdata(_lua, sizeof(SDL_Color));
    memcpy(buffer, &color, sizeof(SDL_Color));
    luaPushColorValueMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
    return 1;
}

// 1 red
// 2 green
// 3 blue
int luaApi_CreateColorFromRGB(lua_State * _lua)
{
    luaL_argcheck(_lua, lua_isinteger(_lua, 1), 1, "the red component is required and must be integer");
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "the green component is required and must be integer");
    luaL_argcheck(_lua, lua_isinteger(_lua, 3), 3, "the blue component is required and must be integer");
    SDL_Color color;
    color.a = 0;
    color.r = lua_tointeger(_lua, 1);
    color.g = lua_tointeger(_lua, 2);
    color.b = lua_tointeger(_lua, 3);
    void * buffer = lua_newuserdata(_lua, sizeof(SDL_Color));
    memcpy(buffer, &color, sizeof(SDL_Color));
    luaPushColorValueMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
    return 1;
}

void luaPushColorValueMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_meta_color_value) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "setRed", luaApi_SetColorRed },
        { "setGreen", luaApi_SetColorGreen },
        { "setBlue", luaApi_SetColorBlue },
        { "setAlpha", luaApi_SetColorAlpha },
        { "getRed", luaApi_GetColorRed },
        { "getGreen", luaApi_GetColorGreen },
        { "getBlue", luaApi_GetColorBlue },
        { "getAlpha", luaApi_GetColorAlpha },
        { nullptr, nullptr }
    };

    luaL_newmetatable(_lua, gc_meta_color_value);
    luaL_setfuncs(_lua, funcs, 0);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
}

// 1 self
// 2 red
int luaApi_SetColorRed(lua_State * _lua)
{
    SDL_Color * color = static_cast<SDL_Color *>(luaL_checkudata(_lua, 1, gc_meta_color_value));
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "The red component must be integer");
    color->r = static_cast<Uint8>(lua_tointeger(_lua, 2));
    return 0;
}

// 1 self
// 2 green
int luaApi_SetColorGreen(lua_State * _lua)
{
    SDL_Color * color = static_cast<SDL_Color *>(luaL_checkudata(_lua, 1, gc_meta_color_value));
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "The green component must be integer");
    color->g = static_cast<Uint8>(lua_tointeger(_lua, 2));
    return 0;
}

// 1 self
// 2 blue
int luaApi_SetColorBlue(lua_State * _lua)
{
    SDL_Color * color = static_cast<SDL_Color *>(luaL_checkudata(_lua, 1, gc_meta_color_value));
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "The blue component must be integer");
    color->b = static_cast<Uint8>(lua_tointeger(_lua, 2));
    return 0;
}

// 1 self
// 2 alpha
int luaApi_SetColorAlpha(lua_State * _lua)
{
    SDL_Color * color = static_cast<SDL_Color *>(luaL_checkudata(_lua, 1, gc_meta_color_value));
    luaL_argcheck(_lua, lua_isinteger(_lua, 2), 2, "The alpha component must be integer");
    color->a = static_cast<Uint8>(lua_tointeger(_lua, 2));
    return 0;
}

// 1 self
int luaApi_GetColorRed(lua_State * _lua)
{
    SDL_Color * color = static_cast<SDL_Color *>(luaL_checkudata(_lua, 1, gc_meta_color_value));
    lua_pushinteger(_lua, color->r);
    return 1;
}

// 1 self
int luaApi_GetColorGreen(lua_State * _lua)
{
    SDL_Color * color = static_cast<SDL_Color *>(luaL_checkudata(_lua, 1, gc_meta_color_value));
    lua_pushinteger(_lua, color->g);
    return 1;
}

// 1 self
int luaApi_GetColorBlue(lua_State * _lua)
{
    SDL_Color * color = static_cast<SDL_Color *>(luaL_checkudata(_lua, 1, gc_meta_color_value));
    lua_pushinteger(_lua, color->b);
    return 1;
}

// 1 self
int luaApi_GetColorAlpha(lua_State * _lua)
{
    SDL_Color * color = static_cast<SDL_Color *>(luaL_checkudata(_lua, 1, gc_meta_color_value));
    lua_pushinteger(_lua, color->a);
    return 1;
}

} // namespace

void Sol2D::Lua::luaPushColorApiOntoStack(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    luaPushColorApiMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
}

bool Sol2D::Lua::luaTryGetColor(lua_State * _lua, int _idx, SDL_Color ** _color)
{
    if(luaL_testudata(_lua, _idx, gc_meta_color_value) == nullptr)
        return false;
    void * buffer = lua_touserdata(_lua, _idx);
    *_color = static_cast<SDL_Color *>(buffer);
    return true;
}
