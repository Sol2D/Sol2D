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

#include <Sol2D/Lua/LuaSpriteSheetOptionsApi.h>
#include <Sol2D/Lua/Aux/LuaTableApi.h>

using namespace Sol2D;
using namespace Sol2D::Lua;

bool Sol2D::Lua::tryGetSpriteSheetOptions(lua_State * _lua, int _idx, SpriteSheetOptions & _options)
{
    LuaTableApi table(_lua, _idx);
    if(!table.isValid())
        return false;
    table.tryGetUnsignedInteger("spriteWidth", &_options.sprite_width);
    table.tryGetUnsignedInteger("spriteHeight", &_options.sprite_height);
    table.tryGetUnsignedInteger("rowCount", &_options.row_count);
    table.tryGetUnsignedInteger("colCount", &_options.col_count);
    table.tryGetUnsignedInteger("marginTop", &_options.margin_top);
    table.tryGetUnsignedInteger("marginLeft", &_options.margin_left);
    table.tryGetUnsignedInteger("horizontalSpacing", &_options.horizontal_spacing);
    table.tryGetUnsignedInteger("verticalSpacing", &_options.vertical_spacing);
    table.tryGetColor("colorToAlpha", _options.color_to_alpha);
    return true;
}
