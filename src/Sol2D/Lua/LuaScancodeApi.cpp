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

#include <Sol2D/Lua/LuaScancodeApi.h>
#include <Sol2D/Lua/Aux/LuaTable.h>
#include <Sol2D/Lua/Aux/LuaMetatable.h>
#include <SDL3/SDL_scancode.h>

using namespace Sol2D::Lua;
using namespace Sol2D::Lua::Aux;

namespace {

const char * gc_metatable_scancode = "sol.Scancode";

} // namespace name

void Sol2D::Lua::pushScancodeEnum(lua_State * _lua)
{
    lua_newuserdata(_lua, 1);
    if(pushMetatable(_lua, gc_metatable_scancode) == MetatablePushResult::Created)
    {
        LuaTable table(_lua);
        table.setIntegerValue("RIGHT_ARROW", SDL_SCANCODE_RIGHT);
        table.setIntegerValue("LEFT_ARROW", SDL_SCANCODE_LEFT);
        table.setIntegerValue("UP_ARROW", SDL_SCANCODE_UP);
        table.setIntegerValue("DOWN_ARROW", SDL_SCANCODE_DOWN);
        table.setIntegerValue("SPACE", SDL_SCANCODE_SPACE);
        table.setIntegerValue("BACKSPACE", SDL_SCANCODE_BACKSPACE);
        table.setIntegerValue("TAB", SDL_SCANCODE_TAB);
        table.setIntegerValue("DELETE", SDL_SCANCODE_DELETE);
        table.setIntegerValue("L_CTRL", SDL_SCANCODE_LCTRL);
        table.setIntegerValue("R_CTRL", SDL_SCANCODE_RCTRL);
        table.setIntegerValue("L_ALT", SDL_SCANCODE_LALT);
        table.setIntegerValue("R_ALT", SDL_SCANCODE_RALT);
        table.setIntegerValue("L_SHIFT", SDL_SCANCODE_LSHIFT);
        table.setIntegerValue("R_SHIFT", SDL_SCANCODE_RSHIFT);
        table.setIntegerValue("ESCAPE", SDL_SCANCODE_ESCAPE);
        table.setIntegerValue("GRAVE", SDL_SCANCODE_GRAVE);
        table.setIntegerValue("_0", SDL_SCANCODE_0);
        table.setIntegerValue("_1", SDL_SCANCODE_1);
        table.setIntegerValue("_2", SDL_SCANCODE_2);
        table.setIntegerValue("_3", SDL_SCANCODE_3);
        table.setIntegerValue("_4", SDL_SCANCODE_4);
        table.setIntegerValue("_5", SDL_SCANCODE_5);
        table.setIntegerValue("_6", SDL_SCANCODE_6);
        table.setIntegerValue("_7", SDL_SCANCODE_7);
        table.setIntegerValue("_8", SDL_SCANCODE_8);
        table.setIntegerValue("_9", SDL_SCANCODE_9);
        table.setIntegerValue("KP_0", SDL_SCANCODE_KP_0);
        table.setIntegerValue("KP_1", SDL_SCANCODE_KP_1);
        table.setIntegerValue("KP_2", SDL_SCANCODE_KP_2);
        table.setIntegerValue("KP_3", SDL_SCANCODE_KP_3);
        table.setIntegerValue("KP_4", SDL_SCANCODE_KP_4);
        table.setIntegerValue("KP_5", SDL_SCANCODE_KP_5);
        table.setIntegerValue("KP_6", SDL_SCANCODE_KP_6);
        table.setIntegerValue("KP_7", SDL_SCANCODE_KP_7);
        table.setIntegerValue("KP_8", SDL_SCANCODE_KP_8);
        table.setIntegerValue("KP_9", SDL_SCANCODE_KP_9);
        table.setIntegerValue("KP_ENTER", SDL_SCANCODE_KP_ENTER);
        table.setIntegerValue("KP_MULTIPLY", SDL_SCANCODE_KP_MULTIPLY);
        table.setIntegerValue("KP_DIVIDE", SDL_SCANCODE_KP_DIVIDE);
        table.setIntegerValue("KP_MINUS", SDL_SCANCODE_KP_MINUS);
        table.setIntegerValue("KP_PLUS", SDL_SCANCODE_KP_PLUS);
        table.setIntegerValue("KP_EQUALS", SDL_SCANCODE_KP_EQUALS);
        table.setIntegerValue("KP_DECIMAL", SDL_SCANCODE_KP_DECIMAL);
        table.setIntegerValue("A", SDL_SCANCODE_A);
        table.setIntegerValue("B", SDL_SCANCODE_B);
        table.setIntegerValue("C", SDL_SCANCODE_C);
        table.setIntegerValue("D", SDL_SCANCODE_D);
        table.setIntegerValue("E", SDL_SCANCODE_E);
        table.setIntegerValue("F", SDL_SCANCODE_F);
        table.setIntegerValue("G", SDL_SCANCODE_G);
        table.setIntegerValue("H", SDL_SCANCODE_H);
        table.setIntegerValue("I", SDL_SCANCODE_I);
        table.setIntegerValue("J", SDL_SCANCODE_J);
        table.setIntegerValue("K", SDL_SCANCODE_K);
        table.setIntegerValue("L", SDL_SCANCODE_L);
        table.setIntegerValue("M", SDL_SCANCODE_M);
        table.setIntegerValue("N", SDL_SCANCODE_N);
        table.setIntegerValue("O", SDL_SCANCODE_O);
        table.setIntegerValue("P", SDL_SCANCODE_P);
        table.setIntegerValue("Q", SDL_SCANCODE_Q);
        table.setIntegerValue("R", SDL_SCANCODE_R);
        table.setIntegerValue("S", SDL_SCANCODE_S);
        table.setIntegerValue("T", SDL_SCANCODE_T);
        table.setIntegerValue("U", SDL_SCANCODE_U);
        table.setIntegerValue("V", SDL_SCANCODE_V);
        table.setIntegerValue("W", SDL_SCANCODE_W);
        table.setIntegerValue("X", SDL_SCANCODE_X);
        table.setIntegerValue("Y", SDL_SCANCODE_Y);
        table.setIntegerValue("Z", SDL_SCANCODE_Z);
        table.setIntegerValue("SLASH", SDL_SCANCODE_SLASH);
        table.setIntegerValue("BACKSLASH", SDL_SCANCODE_BACKSLASH);
        table.setIntegerValue("DECIMAL_SEPARATOR", SDL_SCANCODE_DECIMALSEPARATOR);
        table.setIntegerValue("COMA", SDL_SCANCODE_COMMA);
        table.setIntegerValue("END", SDL_SCANCODE_END);
        table.setIntegerValue("INSERT", SDL_SCANCODE_INSERT);
        table.setIntegerValue("HOME", SDL_SCANCODE_HOME);
        table.setIntegerValue("PAGE_DOWN", SDL_SCANCODE_PAGEDOWN);
        table.setIntegerValue("PAGE_UP", SDL_SCANCODE_PAGEUP);
        table.setIntegerValue("F1", SDL_SCANCODE_F1);
        table.setIntegerValue("F2", SDL_SCANCODE_F2);
        table.setIntegerValue("F3", SDL_SCANCODE_F3);
        table.setIntegerValue("F4", SDL_SCANCODE_F4);
        table.setIntegerValue("F5", SDL_SCANCODE_F5);
        table.setIntegerValue("F6", SDL_SCANCODE_F6);
        table.setIntegerValue("F7", SDL_SCANCODE_F7);
        table.setIntegerValue("F8", SDL_SCANCODE_F8);
        table.setIntegerValue("F9", SDL_SCANCODE_F9);
        table.setIntegerValue("F10", SDL_SCANCODE_F10);
        table.setIntegerValue("F11", SDL_SCANCODE_F11);
        table.setIntegerValue("F12", SDL_SCANCODE_F12);
        table.setIntegerValue("F13", SDL_SCANCODE_F13);
        table.setIntegerValue("F14", SDL_SCANCODE_F14);
        table.setIntegerValue("F15", SDL_SCANCODE_F15);
        table.setIntegerValue("F16", SDL_SCANCODE_F16);
        table.setIntegerValue("F17", SDL_SCANCODE_F17);
        table.setIntegerValue("F18", SDL_SCANCODE_F18);
        table.setIntegerValue("F19", SDL_SCANCODE_F19);
        table.setIntegerValue("F20", SDL_SCANCODE_F20);
        table.setIntegerValue("F21", SDL_SCANCODE_F21);
        table.setIntegerValue("F22", SDL_SCANCODE_F22);
        table.setIntegerValue("F23", SDL_SCANCODE_F23);
        table.setIntegerValue("F24", SDL_SCANCODE_F24);
        table.setIntegerValue("PAUSE", SDL_SCANCODE_PAUSE);
        table.setIntegerValue("MINUS", SDL_SCANCODE_MINUS);
        table.setIntegerValue("SCROLLOCK", SDL_SCANCODE_SCROLLLOCK);
        table.setIntegerValue("NUMLOCK", SDL_SCANCODE_NUMLOCKCLEAR);
        table.setIntegerValue("EQUALS", SDL_SCANCODE_EQUALS);
        table.setIntegerValue("CAPSLOCK", SDL_SCANCODE_CAPSLOCK);
    }
    lua_setmetatable(_lua, -2);
}
