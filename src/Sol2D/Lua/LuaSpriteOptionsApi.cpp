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

#include <Sol2D/Lua/LuaSpriteOptionsApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D::Lua;

bool Sol2D::Lua::tryGetSpriteOptions(lua_State * _lua, int _idx, SpriteOptions & _sprite_options)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return false;
    table.tryGetRect("rect", _sprite_options.rect);
    table.tryGetColor("colorToAlpha", _sprite_options.color_to_alpha);
    table.tryGetBoolean("autodetectRect", &_sprite_options.autodetect_rect);
    return true;
}
