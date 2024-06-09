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

#include <Sol2D/Lua/LuaSpriteSheetOptionsApi.h>
#include <Sol2D/Lua/LuaColorApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>

using namespace Sol2D;
using namespace Sol2D::Lua::Aux;

namespace {

constexpr char gc_key_sprite_width[] = "spriteWidth";
constexpr char gc_key_sprite_height[] = "spriteHeight";
constexpr char gc_key_row_count[] = "rowCount";
constexpr char gc_key_col_count[] = "colCount";
constexpr char gc_key_margin_top[] = "marginTop";
constexpr char gc_key_margin_left[] = "marginLeft";
constexpr char gc_key_horizontal_spacing[] = "horizontalSpacing";
constexpr char gc_key_vertical_spacing[] = "verticalSpacing";
constexpr char gc_key_color_to_alpha[] = "colorToAlpha";

} // namespace name

bool Sol2D::Lua::tryGetSpriteSheetOptions(lua_State * _lua, int _idx, SpriteSheetOptions & _options)
{
    if(!lua_istable(_lua, _idx))
    {
        return false;
    }

    {
        LuaTable table(_lua, _idx);
        lua_Integer lua_int;
        if(table.tryGetInteger(gc_key_sprite_width, &lua_int) && lua_int >= 0)
            _options.sprite_width = static_cast<uint32_t>(lua_int);
        if(table.tryGetInteger(gc_key_sprite_height, &lua_int) && lua_int >= 0)
            _options.sprite_height = static_cast<uint32_t>(lua_int);
        if(table.tryGetInteger(gc_key_row_count, &lua_int) && lua_int >= 0)
            _options.row_count = static_cast<uint32_t>(lua_int);
        if(table.tryGetInteger(gc_key_col_count, &lua_int) && lua_int >= 0)
            _options.col_count = static_cast<uint32_t>(lua_int);
        if(table.tryGetInteger(gc_key_margin_top, &lua_int) && lua_int >= 0)
            _options.margin_top = static_cast<uint32_t>(lua_int);
        if(table.tryGetInteger(gc_key_margin_left, &lua_int) && lua_int >= 0)
            _options.margin_left = static_cast<uint32_t>(lua_int);
        if(table.tryGetInteger(gc_key_horizontal_spacing, &lua_int) && lua_int >= 0)
            _options.horizontal_spacing = static_cast<uint32_t>(lua_int);
        if(table.tryGetInteger(gc_key_vertical_spacing, &lua_int) && lua_int >= 0)
            _options.vertical_spacing = static_cast<uint32_t>(lua_int);
    }

    lua_pushstring(_lua, gc_key_color_to_alpha);
    if(lua_gettable(_lua, -2) == LUA_TTABLE)
    {
        SDL_Color color;
        if(tryGetColor(_lua, -1, color))
            _options.color_to_alpha = color;
    }
    lua_pop(_lua, 1);

    return true;
}
