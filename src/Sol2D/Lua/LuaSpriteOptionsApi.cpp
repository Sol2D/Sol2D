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

#include <Sol2D/Lua/LuaSpriteOptionsApi.h>
#include <Sol2D/Lua/LuaRectApi.h>
#include <Sol2D/Lua/LuaColorApi.h>
#include <lua.hpp>

using namespace Sol2D::Lua;

namespace {

constexpr char gc_key_rect[] = "rect";
constexpr char gc_key_color_to_alpha[] = "colorToAlpha";

} // namespace name

bool Sol2D::Lua::tryGetSpriteOptions(lua_State * _lua, int _idx, SpriteOptions & _sprite_options)
{
    if(!lua_istable(_lua, _idx))
    {
        return false;
    }

    lua_pushstring(_lua, gc_key_rect);
    if(lua_gettable(_lua, -2) == LUA_TTABLE)
    {
        SDL_FRect rect;
        if(tryGetRect(_lua, -1, rect))
            _sprite_options.rect = rect;
    }
    lua_pop(_lua, 1);

    lua_pushstring(_lua, gc_key_color_to_alpha);
    if(lua_gettable(_lua, -2) == LUA_TTABLE)
    {
        SDL_Color color;
        if(tryGetColor(_lua, -1, color))
            _sprite_options.color_to_alpha = color;
    }
    lua_pop(_lua, 1);

    return true;
}
