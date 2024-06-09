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

#include <Sol2D/Lua/LuaTextAlignmentApi.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <Sol2D/Lua/Aux/LuaTopStackTable.h>

using namespace Sol2D::Forms;
using namespace Sol2D::Lua::Aux;

namespace {

const char gc_metatable_vertical_text_alignment[] = "sol.HorizontalTextAlignment";
const char gc_metatable_horizontal_text_alignment[] = "sol.VerticalTextAlignment";

} // namespace

void Sol2D::Lua::pushVerticalTextAlignmentEmum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, gc_metatable_vertical_text_alignment) == MetatablePushResult::Created)
    {
        LuaTopStackTable table(_lua);
        table.setIntegerValue("NONE", static_cast<lua_Integer>(VerticalTextAlignment::None));
        table.setIntegerValue("TOP", static_cast<lua_Integer>(VerticalTextAlignment::Top));
        table.setIntegerValue("CENTER", static_cast<lua_Integer>(VerticalTextAlignment::Center));
        table.setIntegerValue("BOTTOM", static_cast<lua_Integer>(VerticalTextAlignment::Bottom));
    }
    lua_setmetatable(_lua, -2);
}

void Sol2D::Lua::pushHorizontalTextAlignmentEmum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, gc_metatable_horizontal_text_alignment) == MetatablePushResult::Created)
    {
        LuaTopStackTable table(_lua);
        table.setIntegerValue("NONE", static_cast<lua_Integer>(HorizontalTextAlignment::None));
        table.setIntegerValue("BEGIN", static_cast<lua_Integer>(HorizontalTextAlignment::Begin));
        table.setIntegerValue("CENTER", static_cast<lua_Integer>(HorizontalTextAlignment::Center));
        table.setIntegerValue("END", static_cast<lua_Integer>(HorizontalTextAlignment::End));
    }
    lua_setmetatable(_lua, -2);
}

bool Sol2D::Lua::tryGetVerticalTextAlignment(lua_State * _lua, int _idx, Forms::VerticalTextAlignment * _out)
{
    if(lua_isinteger(_lua, _idx))
    {
        switch(lua_tointeger(_lua, _idx))
        {
        case static_cast<lua_Integer>(VerticalTextAlignment::None):
            *_out = VerticalTextAlignment::None;
            return true;
        case static_cast<lua_Integer>(VerticalTextAlignment::Bottom):
            *_out = VerticalTextAlignment::Bottom;
            return true;
        case static_cast<lua_Integer>(VerticalTextAlignment::Center):
            *_out = VerticalTextAlignment::Center;
            return true;
        case static_cast<lua_Integer>(VerticalTextAlignment::Top):
            *_out = VerticalTextAlignment::Top;
            return true;;
        }
    }
    return false;
}

bool Sol2D::Lua::tryGetHorizontalTextAlignment(lua_State * _lua, int _idx, Forms::HorizontalTextAlignment * _out)
{
    if(lua_isinteger(_lua, _idx))
    {
        switch(lua_tointeger(_lua, _idx))
        {
        case static_cast<lua_Integer>(HorizontalTextAlignment::None):
            *_out = HorizontalTextAlignment::None;
            return true;
        case static_cast<lua_Integer>(HorizontalTextAlignment::Begin):
            *_out = HorizontalTextAlignment::Begin;
            return true;
        case static_cast<lua_Integer>(HorizontalTextAlignment::Center):
            *_out = HorizontalTextAlignment::Center;
            return true;
        case static_cast<lua_Integer>(HorizontalTextAlignment::End):
            *_out = HorizontalTextAlignment::End;
            return true;
        }
    }
    return false;
}
