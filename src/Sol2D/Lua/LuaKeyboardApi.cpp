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

#include <Sol2D/Lua/LuaKeyboardApi.h>
#include <SDL3/SDL.h>
#include <new>

using namespace Sol2D::Lua;

namespace {

struct Self
{
    Self()
    {
        kb_state = SDL_GetKeyboardState(&kb_state_length);
    }

    int kb_state_length;
    const Uint8 * kb_state;
};

void luaPushKeyboardApiMetatableOntoStack(lua_State * _lua);
void insertValueInTable(lua_State * _lua, const char * _key, lua_Number _value);
void insertValuesInTable(lua_State * _lua);
int luaApi_GetKeysState(lua_State * _lua);

const char * gc_meta_keyboard = "sol.Keyboard";

void luaPushKeyboardApiMetatableOntoStack(lua_State * _lua)
{
    if(luaL_getmetatable(_lua, gc_meta_keyboard) == LUA_TTABLE)
        return;
    lua_pop(_lua, 1);

    luaL_Reg funcs[] = {
        { "getKeysState", luaApi_GetKeysState },
        { nullptr, nullptr }
    };

    luaL_newmetatable(_lua, gc_meta_keyboard);
    lua_pushstring(_lua, "__index");
    lua_pushvalue(_lua, -2);
    lua_settable(_lua, -3);
    insertValuesInTable(_lua);
    luaL_setfuncs(_lua, funcs, 0);
}

// 1 self
// 2 scancode
// ...
// n scancode
int luaApi_GetKeysState(lua_State * _lua)
{
    Self * self = static_cast<Self *>(luaL_checkudata(_lua, 1, gc_meta_keyboard));
    int top = lua_gettop(_lua);
    for(int idx = 2; idx <= top; ++idx)
    {
        luaL_argcheck(_lua, lua_isinteger(_lua, idx), idx, "only integer arguments are allowed");
        lua_Integer scancode = lua_tointeger(_lua, idx);
        lua_pushboolean(_lua, scancode >= 0 && scancode < self->kb_state_length && self->kb_state[scancode]);
    }
    return top - 1;
}

void insertValuesInTable(lua_State * _lua)
{
    insertValueInTable(_lua, "right_arrow", SDL_SCANCODE_RIGHT);
    insertValueInTable(_lua, "left_arrow", SDL_SCANCODE_LEFT);
    insertValueInTable(_lua, "up_arrow", SDL_SCANCODE_UP);
    insertValueInTable(_lua, "down_arrow", SDL_SCANCODE_DOWN);
    insertValueInTable(_lua, "space", SDL_SCANCODE_SPACE);
    insertValueInTable(_lua, "backspace", SDL_SCANCODE_BACKSPACE);
    insertValueInTable(_lua, "tab", SDL_SCANCODE_TAB);
    insertValueInTable(_lua, "delete", SDL_SCANCODE_DELETE);
    insertValueInTable(_lua, "l_ctrl", SDL_SCANCODE_LCTRL);
    insertValueInTable(_lua, "r_ctrl", SDL_SCANCODE_RCTRL);
    insertValueInTable(_lua, "l_alt", SDL_SCANCODE_LALT);
    insertValueInTable(_lua, "r_alt", SDL_SCANCODE_RALT);
    insertValueInTable(_lua, "l_shift", SDL_SCANCODE_LSHIFT);
    insertValueInTable(_lua, "r_shift", SDL_SCANCODE_RSHIFT);
    insertValueInTable(_lua, "escape", SDL_SCANCODE_ESCAPE);
    insertValueInTable(_lua, "grave", SDL_SCANCODE_GRAVE);
    insertValueInTable(_lua, "0", SDL_SCANCODE_0);
    insertValueInTable(_lua, "1", SDL_SCANCODE_1);
    insertValueInTable(_lua, "2", SDL_SCANCODE_2);
    insertValueInTable(_lua, "3", SDL_SCANCODE_3);
    insertValueInTable(_lua, "4", SDL_SCANCODE_4);
    insertValueInTable(_lua, "5", SDL_SCANCODE_5);
    insertValueInTable(_lua, "6", SDL_SCANCODE_6);
    insertValueInTable(_lua, "7", SDL_SCANCODE_7);
    insertValueInTable(_lua, "8", SDL_SCANCODE_8);
    insertValueInTable(_lua, "9", SDL_SCANCODE_9);
    insertValueInTable(_lua, "kp_0", SDL_SCANCODE_KP_0);
    insertValueInTable(_lua, "kp_1", SDL_SCANCODE_KP_1);
    insertValueInTable(_lua, "kp_2", SDL_SCANCODE_KP_2);
    insertValueInTable(_lua, "kp_3", SDL_SCANCODE_KP_3);
    insertValueInTable(_lua, "kp_4", SDL_SCANCODE_KP_4);
    insertValueInTable(_lua, "kp_5", SDL_SCANCODE_KP_5);
    insertValueInTable(_lua, "kp_6", SDL_SCANCODE_KP_6);
    insertValueInTable(_lua, "kp_7", SDL_SCANCODE_KP_7);
    insertValueInTable(_lua, "kp_8", SDL_SCANCODE_KP_8);
    insertValueInTable(_lua, "kp_9", SDL_SCANCODE_KP_9);
    insertValueInTable(_lua, "kp_enter", SDL_SCANCODE_KP_ENTER);
    insertValueInTable(_lua, "kp_multiply", SDL_SCANCODE_KP_MULTIPLY);
    insertValueInTable(_lua, "kp_divide", SDL_SCANCODE_KP_DIVIDE);
    insertValueInTable(_lua, "kp_minus", SDL_SCANCODE_KP_MINUS);
    insertValueInTable(_lua, "kp_plus", SDL_SCANCODE_KP_PLUS);
    insertValueInTable(_lua, "kp_equals", SDL_SCANCODE_KP_EQUALS);
    insertValueInTable(_lua, "kp_decimal", SDL_SCANCODE_KP_DECIMAL);
    insertValueInTable(_lua, "a", SDL_SCANCODE_A);
    insertValueInTable(_lua, "b", SDL_SCANCODE_B);
    insertValueInTable(_lua, "c", SDL_SCANCODE_C);
    insertValueInTable(_lua, "d", SDL_SCANCODE_D);
    insertValueInTable(_lua, "e", SDL_SCANCODE_E);
    insertValueInTable(_lua, "f", SDL_SCANCODE_F);
    insertValueInTable(_lua, "g", SDL_SCANCODE_G);
    insertValueInTable(_lua, "h", SDL_SCANCODE_H);
    insertValueInTable(_lua, "i", SDL_SCANCODE_I);
    insertValueInTable(_lua, "j", SDL_SCANCODE_J);
    insertValueInTable(_lua, "k", SDL_SCANCODE_K);
    insertValueInTable(_lua, "l", SDL_SCANCODE_L);
    insertValueInTable(_lua, "m", SDL_SCANCODE_M);
    insertValueInTable(_lua, "n", SDL_SCANCODE_N);
    insertValueInTable(_lua, "o", SDL_SCANCODE_O);
    insertValueInTable(_lua, "p", SDL_SCANCODE_P);
    insertValueInTable(_lua, "q", SDL_SCANCODE_Q);
    insertValueInTable(_lua, "r", SDL_SCANCODE_R);
    insertValueInTable(_lua, "s", SDL_SCANCODE_S);
    insertValueInTable(_lua, "t", SDL_SCANCODE_T);
    insertValueInTable(_lua, "u", SDL_SCANCODE_U);
    insertValueInTable(_lua, "v", SDL_SCANCODE_V);
    insertValueInTable(_lua, "w", SDL_SCANCODE_W);
    insertValueInTable(_lua, "x", SDL_SCANCODE_X);
    insertValueInTable(_lua, "y", SDL_SCANCODE_Y);
    insertValueInTable(_lua, "z", SDL_SCANCODE_Z);
    insertValueInTable(_lua, "slash", SDL_SCANCODE_SLASH);
    insertValueInTable(_lua, "backslash", SDL_SCANCODE_BACKSLASH);
    insertValueInTable(_lua, "decimal_separator", SDL_SCANCODE_DECIMALSEPARATOR);
    insertValueInTable(_lua, "coma", SDL_SCANCODE_COMMA);
    insertValueInTable(_lua, "end", SDL_SCANCODE_END);
    insertValueInTable(_lua, "insert", SDL_SCANCODE_INSERT);
    insertValueInTable(_lua, "home", SDL_SCANCODE_HOME);
    insertValueInTable(_lua, "page_down", SDL_SCANCODE_PAGEDOWN);
    insertValueInTable(_lua, "page_up", SDL_SCANCODE_PAGEUP);
    insertValueInTable(_lua, "f1", SDL_SCANCODE_F1);
    insertValueInTable(_lua, "f2", SDL_SCANCODE_F2);
    insertValueInTable(_lua, "f3", SDL_SCANCODE_F3);
    insertValueInTable(_lua, "f4", SDL_SCANCODE_F4);
    insertValueInTable(_lua, "f5", SDL_SCANCODE_F5);
    insertValueInTable(_lua, "f6", SDL_SCANCODE_F6);
    insertValueInTable(_lua, "f7", SDL_SCANCODE_F7);
    insertValueInTable(_lua, "f8", SDL_SCANCODE_F8);
    insertValueInTable(_lua, "f9", SDL_SCANCODE_F9);
    insertValueInTable(_lua, "f10", SDL_SCANCODE_F10);
    insertValueInTable(_lua, "f11", SDL_SCANCODE_F11);
    insertValueInTable(_lua, "f12", SDL_SCANCODE_F12);
    insertValueInTable(_lua, "f13", SDL_SCANCODE_F13);
    insertValueInTable(_lua, "f14", SDL_SCANCODE_F14);
    insertValueInTable(_lua, "f15", SDL_SCANCODE_F15);
    insertValueInTable(_lua, "f16", SDL_SCANCODE_F16);
    insertValueInTable(_lua, "f17", SDL_SCANCODE_F17);
    insertValueInTable(_lua, "f18", SDL_SCANCODE_F18);
    insertValueInTable(_lua, "f19", SDL_SCANCODE_F19);
    insertValueInTable(_lua, "f20", SDL_SCANCODE_F20);
    insertValueInTable(_lua, "f21", SDL_SCANCODE_F21);
    insertValueInTable(_lua, "f22", SDL_SCANCODE_F22);
    insertValueInTable(_lua, "f23", SDL_SCANCODE_F23);
    insertValueInTable(_lua, "f24", SDL_SCANCODE_F24);
    insertValueInTable(_lua, "pause", SDL_SCANCODE_PAUSE);
    insertValueInTable(_lua, "minus", SDL_SCANCODE_MINUS);
    insertValueInTable(_lua, "equals", SDL_SCANCODE_EQUALS);
    insertValueInTable(_lua, "capslock", SDL_SCANCODE_CAPSLOCK);
    insertValueInTable(_lua, "numlock", SDL_SCANCODE_NUMLOCKCLEAR);
    insertValueInTable(_lua, "scrollock", SDL_SCANCODE_SCROLLLOCK);
}

inline void insertValueInTable(lua_State * _lua, const char * _key, lua_Number _value)
{
    lua_pushstring(_lua, _key);
    lua_pushinteger(_lua, _value);
    lua_settable(_lua, -3);
}

} // namespace

void Sol2D::Lua::luaPushKeyboardApiOntoStack(lua_State * _lua)
{
    void * self = lua_newuserdata(_lua, sizeof(Self));
    new(self) Self();
    luaPushKeyboardApiMetatableOntoStack(_lua);
    lua_setmetatable(_lua, -2);
}
